#pragma once

#include <unordered_set>
#include <unordered_map>
#include <set>
#include <map>

class graph_exception { };
class graph_assert : public graph_exception { };
class graph_constraint_failure : public graph_exception { };

struct empty { };

#ifdef _DEBUG
#define ASSERT(expr) { if (!(expr)) throw graph_assert(); }
#else
#define ASSERT(expr)
#endif

//struct FlatContainerConfig {
//    template<typename T, typename> using BaseSetT = flat_multiset_pod<T, 8>;
//    template<typename T, typename U, typename> using BaseMapT = std::map<T, U>;
//};

struct OrderedContainerConfig {
    template<typename T, typename> using BaseSetT = std::set<T>;
    template<typename T, typename U, typename> using BaseMapT = std::map<T, U>;
};

struct UnorderedContainerConfig {
    template<typename T, typename H> using BaseSetT = std::unordered_set<T, H>;
    template<typename T, typename U, typename H> using BaseMapT = std::unordered_map<T, U, H>;
};

template<typename VT, typename ET, typename C = UnorderedContainerConfig>
class graph {
private:
    struct CVT;
    struct DVE;
public:

    template<typename T> struct RefHasher;

    class VRef {
        CVT* v = nullptr;

        VRef(CVT* v) : v(v) { }
    public:
        VRef() = default;

        friend class graph;
        friend struct RefHasher<VRef>;

        inline VT* operator->() const { return &(v->v); }
        inline VT& operator*() const { return v->v; }
        inline bool operator<(const VRef& other) const { return v < other.v; }
        inline bool operator==(const VRef& other) const { return v == other.v; }
        inline bool operator!=(const VRef& other) const { return v != other.v; }
        inline operator bool() const { return v != nullptr; }
    };

    class ERef {
        DVE* e = nullptr;

        ERef(DVE* e) : e(e) { }
    public:
        ERef() = default;

        friend class graph;
        friend struct RefHasher<ERef>;

        VRef get_from() const { return e->from; }
        VRef get_to() const { return e->to; }

        inline ET* operator->() const { return &(e->e); }
        inline ET& operator*() const { return e->e; }
        inline bool operator<(const ERef& other) const { return e < other.e; }
        inline bool operator==(const ERef& other) const { return e == other.e; }
        inline bool operator!=(const ERef& other) const { return e != other.e; }
        inline operator bool() const { return e != nullptr; }
    };

    template<> struct RefHasher<VRef>           { size_t operator()(VRef const& val) const { return std::hash<CVT*>()(val.v); }             };
    template<> struct RefHasher<ERef>           { size_t operator()(ERef const& val) const { return std::hash<DVE*>()(val.e); }             };
    template<typename T> struct SetSpecializer  { using SpecialSet                      = typename C::template BaseSetT<T, std::hash<T>>;            };
    template<> struct SetSpecializer<VRef>      { using SpecialSet                      = typename C::template BaseSetT<VRef, RefHasher<VRef>>;      };
    template<> struct SetSpecializer<ERef>      { using SpecialSet                      = typename C::template BaseSetT<ERef, RefHasher<ERef>>;      };
    template<typename T> struct MapSpecializer  { template<typename U> using SpecialMap = typename C::template BaseMapT<T, U, std::hash<T>>;         };
    template<> struct MapSpecializer<VRef>      { template<typename U> using SpecialMap = typename C::template BaseMapT<VRef, U, RefHasher<VRef>>;   };
    template<> struct MapSpecializer<ERef>      { template<typename U> using SpecialMap = typename C::template BaseMapT<ERef, U, RefHasher<ERef>>;   };
    template<typename T> using SetT = typename SetSpecializer<T>::SpecialSet;
    template<typename T, typename U> using MapT = typename MapSpecializer<T>::template SpecialMap<U>;

private:
    struct CVT { VT v; SetT<ERef> from; SetT<ERef> to; };
    struct DVE { ET e; VRef from; VRef to; };

public:
    inline VRef add_vertex(const VT& v) {
        CVT* const res = new CVT{ v, { }, { } };

        V.insert(res);

        return res;
    }
    inline void remove_vertex(const VRef& v) {
        ASSERT(v);
        ASSERT(V.find(v) != V.end());

        if (v.v->from.empty()) { throw graph_exception(); }
        if (v.v->to.empty()) { throw graph_exception(); }

        V.erase(v.v);

        delete v.v;
    }
    inline ERef add_edge(VRef const& from, VRef const&  to, ET const& e) {
        ASSERT(from);
        ASSERT(to);

        DVE* const res = new DVE { e, from, to };

        E.insert(res);
        from.v->from.insert(res);
        to.v->to.insert(res);

        return res;
    }
    inline void remove_edge(ERef const& e) {
        ASSERT(e);
        ASSERT(E.find(e) != E.end());

        e.e->from.v->from.erase(e);
        e.e->to.v->to.erase(e);
        E.erase(e.e);

        delete e.e;
    }
    inline SetT<ERef> const& edges_from(VRef const& v) const {
        ASSERT(v);
        ASSERT(V.find(v) != V.end());

        return v.v->from;
    }
    inline SetT<ERef> const& edges_to(VRef const& v) const {
        ASSERT(v);
        ASSERT(V.find(v) != V.end());

        return v.v->to;
    }
    inline SetT<VRef> const& all_vertices() const {
        return V;
    }
    inline SetT<ERef> const& all_edges() const {
        return E;
    }

    inline graph() = default;
    inline graph(graph&& other) noexcept {
        *this = std::move(other);
    }
    inline graph& operator=(graph&& other) noexcept {
        V = std::move(other.V);
        E = std::move(other.E);
        other.V.clear();
        other.E.clear();
        return *this;
    }
    inline graph(graph const& other) {
        *this = other;
    }
    inline graph& operator=(graph const& other) {
        for (ERef e : E) {
            delete e.e;
        }
        for (VRef v : V) {
            delete v.v;
        }
        V.clear();
        E.clear();
    
        MapT<VRef, VRef> Vmap;
        MapT<ERef, ERef> Emap;
        for (VRef v : other.V) {
            CVT* w = new CVT(*(v.v));
            Vmap[v] = w;
            V.insert(w);
        }
    
        for (ERef e : other.E) {
            DVE* f = new DVE(*(e.e));
            f->to = Vmap.at(e.e->to.v);
            f->from = Vmap.at(e.e->from.v);
            Emap[e] = f;
            E.insert(f);
        }
    
        for (VRef v : other.V) {
            {
                SetT<ERef> otherFrom;
                for (ERef e : v.v->from) {
                    otherFrom.insert(ERef(Emap.at(e.e)));
                }
                Vmap.at(v).v->from = otherFrom;
            }
            {
                SetT<ERef> otherTo;
                for (ERef e : v.v->to) {
                    otherTo.insert(ERef(Emap.at(e.e)));
                }
                Vmap.at(v).v->to = otherTo;
            }
        }
    
        return *this;
    }
    inline ~graph() {
        for (ERef e : E) {
            delete e.e;
        }
        for (VRef v : V) {
            delete v.v;
        }
    }
private:
    SetT<VRef> V;
    SetT<ERef> E;
};