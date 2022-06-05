#pragma once

#include "graph.hpp"
#include <functional>
#include <random>
#include <vector>
#include <set>

template<typename T>
T rval(T range_from, T range_to) {
    std::random_device                  rand_dev;
    std::mt19937                        generator(rand_dev());
    std::uniform_int_distribution<T>    distr(range_from, range_to);
    return distr(generator);
}

using std::function;
using std::vector;

template<typename VT, typename ET>
class graph_algorithm {
public:
    using graphT = graph<VT, ET>;
    using VRef = typename graphT::VRef;
    using ERef = typename graphT::ERef;

    template<typename T>
    using SetT = typename graphT::template SetT<T>;
    template<typename T, typename U>
    using MapT = typename graphT::template MapT<T, U>;

    // https://en.wikipedia.org/wiki/Dijkstra's_algorithm
    template<typename CostT>
    inline static vector<ERef>
    dijkstra(const graphT& graph, VRef start, VRef end, function<CostT(CostT const&, ET const&)> addCostFunc) {
        struct Hop {
            CostT total;
            VRef from;
            ERef edge;
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

            const auto& fromu = graph.edges_from(u.from);
            for (ERef e : fromu) {
                CostT c = addCostFunc(u.total, *e);
                VRef v = e.get_to();
                if (P.find(v) == P.end() || c < P[v].total) {
                    P[v] = Hop{ c, v, e };

                    // Erase worse value from the priority queue
                    auto found = std::find_if(Q.begin(), Q.end(), [v](Hop const& val) { return val.edge.get_to() == v; });
                    if (found != Q.end()) Q.erase(found);

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
            found = P.find(found->second.edge.get_from());
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
        MapT<VRef, State> C;

        function<void(VRef const&)> strongConnect;
        
        strongConnect = [&](VRef const& v) {
            C[v] = State{ index, index, true };
            ++index;
            S.push_back(v);

            for (auto e : G.edges_from(v)) {
                VRef w = e.get_to();
                if (C.find(w) == C.end()) {
                    strongConnect(w);
                    C[v].lowlink = std::min(C.at(v).lowlink, C.at(w).lowlink);
                } else if (C.at(w).onStack) {
                    C[v].lowlink = std::min(C.at(v).lowlink, C.at(w).index);
                }
            }

            if (C.at(v).lowlink == C.at(v).index) {
                SetT<VRef> component;

                VRef w;
                do {
                    w = S.back();
                    S.pop_back();
                    C.at(w).onStack = false;
                    component.insert(w);
                } while (w != v);
                res.add_vertex(component);
            }
        };

        for (VRef v : G.all_vertices()) {
            if (C.find(v) == C.end()) {
                strongConnect(v);
            }
        }

        return res;
    }

    inline static bool
    is_strongly_connected(graphT const& G) {
        return tarjan_scc(G).all_vertices().size() <= 1; // Empty graph considered to be fully connected
    }

    inline static graphT get_random(int n, int m, function<VT(size_t)> vertex_generator, function<ET(size_t)> edge_generator) {
        graphT res;
        vector<VRef> V;
        for (size_t i = 0; i < n; ++i) {
            V.push_back(res.add_vertex(vertex_generator(i)));
        }
        for (size_t i = 0; i < m; ++i) {
            res.add_edge(V[rval<size_t>(0, V.size() - 1)], V[rval<size_t>(0, V.size() - 1)], edge_generator(i));
        }
        return res;
    }
};