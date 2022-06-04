#pragma once

#include "graph.hpp"
#include <functional>
#include <random>
#include <vector>

template<typename T>
T rval(T range_from, T range_to) {
    std::random_device                  rand_dev;
    std::mt19937                        generator(rand_dev());
    std::uniform_int_distribution<T>    distr(range_from, range_to);
    return distr(generator);
}

using std::set;
using std::function;
using std::vector;

template<typename VT, typename ET>
class graph_algorithm {
public:
    using graphT = graph<VT, ET>;
    using VRef = typename graphT::VRef;
    using ERef = typename graphT::ERef;

    inline static set<VRef> get_from(const set<ERef>& edges) {
        set<VRef> res;
        for (const auto& e : edges) { res.insert(e->from); }
        return res;
    }

    inline static set<VRef> get_to(const set<ERef>& edges) {
        set<VRef> res;
        for (const auto& e : edges) { res.insert(e->to); }
        return res;
    }

    inline static set<VRef> get_from_if(const set<ERef>& edges, function<bool(const ERef&)> pred) {
        set<VRef> res;
        for (const auto& e : edges) { if (pred(e)) res.insert(e->from); }
        return res;
    }

    inline static set<VRef> get_to_if(const set<ERef>& edges, function<bool(const ERef&)> pred) {
        set<VRef> res;
        for (const auto& e : edges) { if (pred(e)) res.insert(e->from); }
        return res;
    }

    inline static vector<VRef> dijkstra(const graphT& graph, VRef start, VRef end) {
        vector<VRef> res;
        set<VRef> visited;
        set<VRef> to_visit;

        to_visit.insert(start);

        while (!to_visit.empty()) {
            set<ERef> next = graph.get_edges(*curr);
        }

        return res;
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