#pragma once

#include <set>
#include <map>

using std::set;
using std::map;

class graph_exception { };

template<typename VT, typename ET>
class graph {
public:
    struct DVE;

    class VRef {
        VT* v = nullptr;

        VRef(VT* v) : v(v) { }
    public:
        VRef() = default;

        friend class graph;

        inline VT* operator->() const { return v; }
        inline VT& operator*() const { return *v; }
        inline bool operator<(const VRef& other) const { return v < other.v; }
        inline bool operator==(const VRef& other) const { return v == other.v; }
    };
    
    class ERef {
        DVE* e = nullptr;

        ERef(DVE* e) : e(e) { }
    public:
        ERef() = default;

        friend class graph;

        VRef get_from() const { return e->from; }
        VRef get_to() const { return e->to; }

        inline ET* operator->() const { return e->e; }
        inline ET& operator*() const { return e->e; }
        inline bool operator<(const ERef& other) const { return e < other.e; }
        inline bool operator==(const ERef& other) const { return e == other.e; }
    };
    
    // Directed Edge
    struct DVE {
        ET e;
        VRef from;
        VRef to;
    };

    inline VRef add_vertex(const VT& v) {
        VT* const res = new VT(v);

        V.insert(res);
        fromV.insert({ res, set<ERef>() });

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

    inline set<ERef> edges_from(VRef const& v) const {
        assert(v.v);
        assert(V.contains(v.v));

        return fromV.at(v.v);
    }

    inline set<ERef> edges_to(VRef const& v) const {
        assert(v.v);
        assert(V.contains(v.v));

        return toV.at(v.v);
    }

    inline VRef get_vertex(VT const& v) const {
        for (auto const& vv : V) {
            if (*vv == v) { return vv; }
        }

        return nullptr;
    }

private:
    set<VT*> V;
    set<DVE*> E;
    
    // Acceleration structures
    map<VT*, set<ERef>> fromV;
    map<VT*, set<ERef>> toV;
};