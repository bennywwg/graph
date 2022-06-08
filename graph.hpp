#pragma once

#include <unordered_set>
#include <unordered_map>
#include <set>
#include <map>
#include "slot_map.h"

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
    template<typename T, typename U>
    using MapT = std::map<T, U>;

    template<typename T>
    using SetT = std::vector<T>;

    using VRef = dod::slot_map_key<CVT>;
    using ERef = dod::slot_map_key<DVE>;
private:
    
    struct CVT { VT v;  SetT<ERef> from;    SetT<ERef> to; };
    struct DVE { ET e;        VRef from;          VRef to; };

public:

public:
    inline VRef add_vertex(const VT& v) {
        return V.emplace(CVT{v, SetT<ERef>(), SetT<ERef>()});
    }
    inline void remove_vertex(const VRef& v) {
        ASSERT(V.has_key(v));

        if (!V.get(v)->from.empty()) { throw graph_exception(); }
        if (!V.get(v)->to.empty()) { throw graph_exception(); }

        V.eraase(v);
    }
    inline VT& get(VRef v) {
        return V.get(v)->v;
    }
    inline ERef add_edge(VRef const& from, VRef const& to, ET const& e) {
        ASSERT(V.has_key(from));
        ASSERT(V.has_key(to));

        return ERef::invalid();
        //ERef res = E.emplace(e, from, to);
        
        //V.get(from)->from.push_back(res);
        //V.get(to)->to.push_back(res);

        //return res;
    }
    inline void remove_edge(ERef const& e) {
        ASSERT(E.has_key(e));

        V.get(E.get(e)->from)->from.erase(e);
        V.get(E.get(e)->to)->to.erase(e);
        E.erase(e);
    }
    inline ET& get(ERef e) const {
        return const_cast<ET&>(E.get(e)->e);
    }
    inline VRef get_from(ERef e) const {
        return E.get(e)->from;
    }
    inline VRef get_to(ERef e) const {
        return E.get(e)->to;
    }
    inline SetT<ERef> const& edges_from(VRef const& v) const {
        ASSERT(V.has_key(v));

        return V.get(v)->from;
    }
    inline SetT<ERef> const& edges_to(VRef const& v) const {
        ASSERT(V.has_key(v));

        return V.get(v)->to;
    }
    inline void iterate_vertices(std::function<bool(VRef)> const& op) const {
        for (VRef v : V) if(op(v)) break;
    }
    inline void iterate_vertex_values(std::function<bool(VRef, VT const&)> const& op) const {
        for (const auto& [key, value] : V.items()) if(op(key, value.get().v)) break;
    }
    inline void iterate_edges(std::function<bool(ERef)> const& op) const {
        for (ERef e : E) if(op(e)) break;
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
    inline graph(graph const& other) = delete; /*{
        *this = other;
    }*/
    inline graph& operator=(graph const& other) = delete;/* {
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
    }*/
private:
    dod::slot_map<CVT> V;
    dod::slot_map<DVE> E;
};