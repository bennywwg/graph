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

    inline static SetT<VRef> get_from(const SetT<ERef>& edges) {
        SetT<VRef> res;
        for (const auto& e : edges) { res.insert(e->from); }
        return res;
    }

    inline static SetT<VRef> get_to(const SetT<ERef>& edges) {
        SetT<VRef> res;
        for (const auto& e : edges) { res.insert(e->to); }
        return res;
    }

    inline static SetT<VRef> get_from_if(const SetT<ERef>& edges, function<bool(const ERef&)> pred) {
        SetT<VRef> res;
        for (const auto& e : edges) { if (pred(e)) res.insert(e->from); }
        return res;
    }

    inline static SetT<VRef> get_to_if(const SetT<ERef>& edges, function<bool(const ERef&)> pred) {
        SetT<VRef> res;
        for (const auto& e : edges) { if (pred(e)) res.insert(e->from); }
        return res;
    }

    template<typename CostT>
    inline static vector<ERef> dijkstra(const graphT& graph, VRef start, VRef end, function<CostT(CostT const&, ET const&)> addCostFunc) {
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

    template<typename CostT>
    inline static vector<ERef> dijkstra(const graphT& graph, VRef start, VRef end) {
        return dijkstra<CostT>(graph, start, end, [](CostT const& cost, ET const& e) { return e + cost; });
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