#pragma once

#include <unordered_set>
#include <unordered_map>

class graph_exception { };
class graph_assert : public graph_exception { };
class graph_constraint_failure : public graph_exception { };

struct empty { };

void assert(bool expression) { if (!expression) throw graph_assert(); }

template<typename VT, typename ET>
class graph {
public:
    struct DVE;

    template<typename T> struct RefHasher;

    class VRef {
        VT* v = nullptr;

        VRef(VT* v) : v(v) { }
    public:
        VRef() = default;

        friend class graph;
        friend struct RefHasher<VRef>;

        inline VT* operator->() const { return v; }
        inline VT& operator*() const { return *v; }
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

        inline ET* operator->() const { return e->e; }
        inline ET& operator*() const { return e->e; }
        inline bool operator<(const ERef& other) const { return e < other.e; }
        inline bool operator==(const ERef& other) const { return e == other.e; }
        inline bool operator!=(const ERef& other) const { return e != other.e; }
        inline operator bool() const { return e != nullptr; }
    };

    struct DVE {
        ET e;
        VRef from;
        VRef to;
    };

    template<typename T, typename H> using BaseSetT = std::unordered_set<T, H>;
    template<typename T, typename U, typename H> using BaseMapT = std::unordered_map<T, U, H>;
    template<> struct RefHasher<VRef> { size_t operator()(VRef const& val) const { return std::hash<VT*>()(val.v); } };
    template<> struct RefHasher<ERef> { size_t operator()(ERef const& val) const { return std::hash<DVE*>()(val.e); } };
    template<typename T> struct SetSpecializer { using SpecialSet = BaseSetT<T, std::hash<T>>; };
    template<> struct SetSpecializer<VRef> { using SpecialSet = BaseSetT<VRef, RefHasher<VRef>>; };
    template<> struct SetSpecializer<ERef> { using SpecialSet = BaseSetT<ERef, RefHasher<ERef>>; };
    template<typename T> struct MapSpecializer { template<typename U> using SpecialMap = BaseMapT<T, U, std::hash<T>>; };
    template<> struct MapSpecializer<VRef> { template<typename U> using SpecialMap = BaseMapT<VRef, U, RefHasher<VRef>>; };
    template<> struct MapSpecializer<ERef> { template<typename U> using SpecialMap = BaseMapT<ERef, U, RefHasher<ERef>>; };
    template<typename T> using SetT = typename SetSpecializer<T>::SpecialSet;
    template<typename T, typename U> using MapT = typename MapSpecializer<T>::template SpecialMap<U>;

    inline VRef add_vertex(const VT& v) {
        VT* const res = new VT(v);

        V.insert(res);
        fromV.insert({ res, SetT<ERef>() });

        return res;
    }

    inline void remove_vertex(const VRef& v) {
        assert(v.v);
        assert(V.contains(v.v));

        if (!fromV[v.v].empty()) { throw graph_exception(); }

        fromV.erase(v.v);
        V.erase(v.v);
    }

    inline ERef add_edge(VRef const& from, VRef const& to, ET const& e) {
        assert(from.v);
        assert(to.v);

        DVE* const res = new DVE { e, from, to };

        E.insert(res);
        fromV[from.v].insert(res);
        toV[to.v].insert(res);

        return res;
    }

    inline void remove_edge(ERef const& e) {
        assert(e.e);
        assert(E.contains(e.e));

        fromV[e.e->from.v].erase(e);
        toV[e.e->to.v].erase(e);
        E.erase(e.e);

        delete e.e;
    }

    inline SetT<ERef> edges_from(VRef const& v) const {
        assert(v.v);
        assert(V.find(v.v) != V.end());

        return fromV.at(v.v);
    }

    inline SetT<ERef> edges_to(VRef const& v) const {
        assert(v.v);
        assert(V.contains(v.v));

        return toV.at(v.v);
    }

    inline VRef find_vertex(VT const& v) const {
        for (auto const& vv : V) {
            if (*vv == v) { return vv; }
        }

        return nullptr;
    }

    inline SetT<VRef> all_vertices() const {
        SetT<VRef> res;
        for (VT* v : V) res.insert(v);
        return res;
    }

    inline SetT<ERef> all_edges() const {
        SetT<ERef> res;
        for (DVE* e : E) res.insert(e);
        return res;
    }

    inline graph() = default;
    inline graph(graph&& other) {
        *this = std::move(other);
    }
    inline graph& operator=(graph&& other) {
        V = other.V;
        E = other.E;
        fromV = other.fromV;
        toV = other.toV;
        other.V.clear();
        other.E.clear();
        other.fromV.clear();
        other.toV.clear();
        return *this;
    }
    inline ~graph() {
        for (DVE* e : E) {
            delete e;
        }
        for (VT* v : V) {
            delete v;
        }
    }
private:
    SetT<VT*> V;
    SetT<DVE*> E;
    
    // Acceleration structures
    MapT<VT*, SetT<ERef>> fromV;
    MapT<VT*, SetT<ERef>> toV;

    // Prevent copying
    graph(graph const& other) = delete;
    graph& operator=(graph const& other) = delete;
};