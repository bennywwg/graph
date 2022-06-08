#include <iostream>
#include <string>
#include <chrono>

#include "graph.hpp"
#include "graph_algorithms.hpp"

template<typename C>
void profile() {
    typedef std::chrono::high_resolution_clock Clock;
    using gal = graph_algorithm<size_t, empty, C>;

    vector<size_t> nanos;
    for (size_t n = 100; n < 1000000; n += 100000) {

        auto t1 = Clock::now();
        vector<typename gal::VRef> V;
        graph<size_t, empty, C> G = gal::get_random(n, n, [](size_t i) {return i; }, [](size_t) { return empty(); }, V);

        for (size_t i = 0; i < n; ++i) G.add_edge(V[i], V[(i + 1) % n], empty());

        bool val = true;//gal::path_exists(G, gal::find_vertex(G, 0), gal::find_vertex(G, n - 1));
        ASSERT(val);

        nanos.push_back((Clock::now() - t1).count());
        std::cout << n << ", " << val << " -> " << static_cast<double>(nanos.back()) * 0.000000001 << "s, perf/vert = " << static_cast<double>(n * 1000000) / static_cast<double>(nanos.back()) << std::endl;
    }

    for (size_t i = 0; i < nanos.size(); ++i) {
        //std::cout << static_cast<double>(nanos[i]) * 0.000000001 << "s" << std::endl;
    }
}

int main(int, char**) {
    using gal = graph_algorithm<std::string, int>;

    profile<UnorderedContainerConfig>();
    std::cout << std::endl;

    profile<OrderedContainerConfig>();

    graph<std::string, int> G = gal::get_random(
        50, 50,
        [](size_t i)    { return "V" + std::to_string(i + 1); },
        [](size_t)      { return rval(5, 20); }
    );

    // Ensure the graph is strongly connected (albeit with high cost edges)s
    for (size_t i = 0; i < 50; ++i) {
        auto a = gal::find_vertex(G, "V" + std::to_string(i + 1));
        auto b = gal::find_vertex(G, "V" + std::to_string(((i + 1) % 50) + 1));
        G.add_edge(a, b, 100);
    }
    
    auto res = gal::dijkstra<int>(G, gal::find_vertex(G, "V1"), gal::find_vertex(G, "V50"));

    for (auto e : res) {
        std::cout << G.get(G.get_from(e)) << " --( " << G.get(e) << " )--> " << G.get(G.get_to(e)) << std::endl;
    }

    bool scc = gal::is_strongly_connected(G);
    ASSERT(scc);

    return 0;
}