//#include <iostream>
//#include <string>
//#include <chrono>
//
//#include "graph.hpp"
//#include "graph_algorithms.hpp"
//#include "constrained_graph.hpp"
//
//using std::string;
//
//template<typename C>
//void profile() {
//    typedef std::chrono::high_resolution_clock Clock;
//    using gal = graph_algorithm<size_t, empty, C>;
//
//    vector<size_t> nanos;
//    for (size_t n = 100; n < 1000000; n += 100000) {
//        auto t1 = Clock::now();
//        vector<typename gal::VRef> V;
//        graph<size_t, empty, C> G = gal::get_random(n, n, [](size_t i) {return i; }, [](size_t) { return empty(); }, V);
//
//        for (size_t i = 0; i < n; ++i) G.add_edge(V[i], V[(i + 1) % n], empty());
//
//        bool val = gal::path_exists(G, gal::find_vertex(G, 0), gal::find_vertex(G, n - 1));
//        ASSERT(val);
//
//        nanos.push_back((Clock::now() - t1).count());
//        std::cout << n << ", " << val << " -> " << static_cast<double>(nanos.back()) * 0.000000001 << "s, perf/vert = " << static_cast<double>(n * 1000000) / static_cast<double>(nanos.back()) << std::endl;
//    }
//
//    for (size_t i = 0; i < nanos.size(); ++i) {
//        //std::cout << static_cast<double>(nanos[i]) * 0.000000001 << "s" << std::endl;
//    }
//}
//
//int main(int, char**) {
//    profile<UnorderedContainerConfig>();
//    std::cout << std::endl;
//    profile<OrderedContainerConfig>();
//    //std::cout << std::endl;
//    //profile<FlatContainerConfig>();
//
//    using gal = graph_algorithm<string, int>;
//
//    graph<string, int> G = gal::get_random(
//        50,
//        50,
//        [](size_t i) {
//            return "V" + std::to_string(i + 1);
//        },
//        [](size_t) {
//            return rval(5, 20);
//        }
//    );
//
//    // Ensure the graph is strongly connected (albeit with high cost edges)
//    if (true) {
//        for (size_t i = 0; i < 50; ++i) {
//            auto a = gal::find_vertex(G, "V" + std::to_string(i + 1));
//            auto b = gal::find_vertex(G, "V" + std::to_string(((i + 1) % 50) + 1));
//            G.add_edge(a, b, 100);
//        }
//    }
//
//    graph<string, int> g2 = G;
//    graph<string, int> g3 = G;
//    g2 = g3;
//
//    auto res = gal::dijkstra<int>(G, gal::find_vertex(G, "V1"), gal::find_vertex(G, "V50"));
//
//    ASSERT(!res.empty());
//
//    auto res2 = gal::tarjan_scc(G);
//
//    ASSERT(gal::is_strongly_connected(G));
//
//    for (auto e : res) {
//        std::cout << *(e.get_from()) << " --( " << *e << " )--> " << (*e.get_to()) << std::endl;
//    }
//
//    std::cout << std::endl;
//    for (auto e : gal::dijkstra<int>(g2, gal::find_vertex(g2, "V1"), gal::find_vertex(g2, "V50"))) {
//        std::cout << *(e.get_from()) << " --( " << *e << " )--> " << (*e.get_to()) << std::endl;
//    }
//    std::cout << std::endl;
//    for (auto e : gal::dijkstra<int>(g3, gal::find_vertex(g3, "V1"), gal::find_vertex(g3, "V50"))) {
//        std::cout << *(e.get_from()) << " --( " << *e << " )--> " << (*e.get_to()) << std::endl;
//    }
//
//    /*
//    acyclic_graph<string, empty> cg;
//    cg.add_edge(gal::find_vertex(cg, "a"), gal::find_vertex(cg, "b"), empty());
//
//    try {
//        cg.add_edge(gal::find_vertex(cg, "b"), gal::find_vertex(cg, "a"), empty());
//        std::cout << std::endl << "ERROR! Should have excepted here" << std::endl;
//    } catch (graph_constraint_failure const&) {
//        std::cout << std::endl << "Correctly handled constraint failure" << std::endl;
//    }
//    */
//
//    return 0;
//}