#include <iostream>
#include <string>
#include <chrono>

#include "graph.hpp"
#include "graph_algorithms.hpp"

int main(int, char**) {
    
    using gal = graph_algorithm<std::string, int>;

    graph<std::string, int> G = gal::get_random(
        50,
        50,
        [](size_t i) {
            return "V" + std::to_string(i + 1);
        },
        [](size_t) {
            return rval(5, 20);
        }
    );

    // Ensure the graph is strongly connected (albeit with high cost edges)
    if (true) {
        for (size_t i = 0; i < 50; ++i) {
            auto a = gal::find_vertex(G, "V" + std::to_string(i + 1));
            auto b = gal::find_vertex(G, "V" + std::to_string(((i + 1) % 50) + 1));
            G.add_edge(a, b, 100);
        }
    }

    auto res = gal::dijkstra<int>(G, gal::find_vertex(G, "V1"), gal::find_vertex(G, "V50"));

    for (auto e : res) {
        std::cout << G.get_from(e) << " --( " << G.get(e) << " )--> " << G.get_to(e) << std::endl;
    }

    return 0;
}