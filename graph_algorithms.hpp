#pragma once

#include "graph.hpp"
#include <functional>
#include <vector>
#include <set>

static unsigned long x = 123456789, y = 362436069, z = 521288629;

unsigned long rval(size_t l, size_t u) {          //period 2^96-1
    unsigned long t;
    x ^= x << 16;
    x ^= x >> 5;
    x ^= x << 1;

    t = x;
    x = y;
    y = z;
    z = t ^ x ^ y;

    return l + (z % u);
}

using std::function;
using std::vector;

template<typename VT, typename ET, typename C = UnorderedContainerConfig>
class graph_algorithm {
public:
    using graphT = graph<VT, ET, C>;
    using VRef = typename graphT::VRef;
    using ERef = typename graphT::ERef;

    template<typename T>
    using SetT = typename graphT::template SetT<T>;
    template<typename T, typename U>
    using MapT = typename graphT::template MapT<T, U>;

    inline static VRef
    find_vertex(graphT const& G, VT const& v) {
        VRef res = VRef::invalid();
        G.iterate_vertices([&](VRef kv, VT const& sv) {
            if (v == sv) {
                res = kv;
                return true;
            }
            return false;
        });
        return res;
    }

    // https://en.wikipedia.org/wiki/Dijkstra's_algorithm
    template<typename CostT>
    inline static vector<ERef>
    dijkstra(const graphT& G, VRef start, VRef end, function<CostT(CostT const&, ET const&)> const& addCostFunc) {
        struct Hop {
            CostT total;
            VRef from;
            ERef edge;
            Hop() : total(), from(), edge() { }
            Hop(CostT total, VRef from, ERef edge) : total(total), from(from), edge(edge) { }
            inline bool operator<(Hop const& other) const {
                if (from && !other.from) return true;
                else if (!from) return false;
                else return total < other.total;
            }
        };

        if (start == end) return { };

        std::multiset<Hop> Q;
        MapT<VRef, Hop> P;

        {
            Hop startHop{ CostT(), start, ERef() };
            Q.insert(startHop);
            P[start] = startHop;
        }

        while (!Q.empty()) {
            Hop u = *Q.begin();
            Q.erase(Q.begin());

            const auto& fromu = G.edges_from(u.from);
            for(size_t i = 0; i < fromu.size(); ++i) {
                ERef e = fromu[i];
            //for (ERef e : fromu) {
                CostT c = addCostFunc(u.total, G.get(e));
                VRef v = G.get_to(e);
                if (P.find(v) == P.end() || c < P[v].total) {
                    P[v] = Hop{ c, v, e };

                    Q.insert(P[v]);
                }
            }

            if (P.find(end) != P.end()) break;
        }

        // Trace solution
        vector<ERef> res;
        auto found = P.find(end);
        while (found != P.end() && found->first != start) {
            res.push_back(found->second.edge);
            found = P.find(G.get_from(found->second.edge));
        }
        std::reverse(res.begin(), res.end());
        return res;
    }

    // https://en.wikipedia.org/wiki/Dijkstra's_algorithm
    template<typename CostT>
    inline static vector<ERef>
    dijkstra(const graphT& graph, VRef start, VRef end) {
        return dijkstra<CostT>(graph, start, end, [](CostT const& cost, ET const& e) { return e + cost; });
    }

    inline static VRef
    search(graphT const& G, VRef v, function<bool(VRef)> const& pred) {
        ASSERT(v);

        SetT<VRef> T;
        vector<VRef> U = { v };
        size_t index = 0;

        while (index < U.size()) {
            VRef curr = U[index++];
            if (pred(curr)) return curr;

            T.push_back(curr);

            for (ERef e : G.edges_from(curr)) {
                VRef w = G.get_to(e);
                
                if (std::find(T.begin(), T.end(), w) == T.end()) U.push_back(w);
            }
        }

        return VRef();
    }

    inline static bool
    path_exists(graphT const& G, VRef v, VRef w) {
        ASSERT(v);
        ASSERT(w);
        return (v == w) || search(G, v, [w](VRef r) { return r == w; });
    }

    inline static bool
    will_cycle(graphT const& G, VRef v, VRef w)
    { return path_exists(G, w, v); }
    
    // https://en.wikipedia.org/wiki/Tarjan's_strongly_connected_components_algorithm
    // Currently, no edges are generated between the components
    inline static graph<SetT<VRef>, empty>
    tarjan_scc(graphT const& G) {
        struct State {
            size_t index;
            size_t lowlink;
            bool onStack;
        };

        graph<SetT<VRef>, empty> res;

        size_t index = 0;
        vector<VRef> S;
        MapT<VRef, State> R;

        function<void(VRef const&)> strongConnect;
        
        strongConnect = [&](VRef v) {
            R[v] = State{ index, index, true };
            ++index;
            S.push_back(v);

            for (auto e : G.edges_from(v)) {
                VRef w = G.get_to(e);
                if (R.find(w) == R.end()) {
                    strongConnect(w);
                    R[v].lowlink = std::min(R.at(v).lowlink, R.at(w).lowlink);
                } else if (R.at(w).onStack) {
                    R[v].lowlink = std::min(R.at(v).lowlink, R.at(w).index);
                }
            }

            if (R.at(v).lowlink == R.at(v).index) {
                SetT<VRef> component;

                VRef w;
                do {
                    w = S.back();
                    S.pop_back();
                    R.at(w).onStack = false;
                    component.push_back(w);
                } while (w != v);
                res.add_vertex(component);
            }
        };

        G.iterate_vertices([&](VRef v, VT& vv) {
            if (R.find(v) == R.end()) {
                strongConnect(v);
            }
            return false;
        });

        return res;
    }

    // Empty graph considered to be fully connected 
    inline static bool
    is_strongly_connected(graphT const& G)
    { return tarjan_scc(G).vertex_count() <= 1; }

    // A modification of tarjan's
    inline static bool
    cycles(graphT const& G)
    { return tarjan_scc(G).vertex_count() < G.vertex_count(); }

    inline static graphT
    get_random(size_t n, size_t m, function<VT(size_t)> const& vertex_generator, function<ET(size_t)> const& edge_generator, vector<VRef>& V) {
        graphT res;
        V.clear();
        for (size_t i = 0; i < n; ++i) V.push_back(res.add_vertex(vertex_generator(i)));
        for (size_t i = 0; i < m; ++i) res.add_edge(V[rval(0, V.size() - 1)], V[rval(0, V.size() - 1)], edge_generator(i));
        return res;
    }
    inline static graphT
    get_random(size_t n, size_t m, function<VT(size_t)> const& vertex_generator, function<ET(size_t)> const& edge_generator) {
        vector<VRef> V;
        return get_random(n, m, vertex_generator, edge_generator, V);
    }

    inline static graphT
    get_random_dag(size_t n, size_t m, function<VT(size_t)> const& vertex_generator, function<ET(size_t)> const& edge_generator, VRef& start, VRef& end) {
        if (n <= 1) {
            graphT res;
            if (n == 1) res.add_vertex(vertex_generator(0));
        }

        MapT<VRef, int> R;
        vector<VRef> V;
        get_random(n, 0, vertex_generator, edge_generator, V);

        auto pop_random = [&V]() {
            size_t i = rval(0, V.size());
            VRef v = V[i];
            V.erase(V.begin() + i);
            return v;
        };


        // Assign start and end verts randomly
        start = pop_random();
        end = pop_random();

        R[start] = 1;
        R[end] = n;

        // Add edges to the graph
        for (size_t i = 1; i < n - 1; ++i) {
            VRef v = pop_random();
            R[v] = i;
        }
    }
};