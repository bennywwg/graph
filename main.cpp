#include <iostream>
#include <string>

#include "graph.hpp"
#include "graph_algorithms.hpp"

using std::string;

int main(int argc, char** argv) {
    using gal = graph_algorithm<string, int>;

    graph<string, int> graph = gal::get_random(
        50,
        50,
        [](size_t i) {
            return "V" + std::to_string(i + 1);
        },
        [](size_t i) {
            return rval(5, 20);
        }
        );

    // Ensure the graph is strongly connected (albeit with high cost edges)
    if (true) {
        for (size_t i = 0; i < 50; ++i) {
            auto a = graph.find_vertex("V" + std::to_string(i + 1));
            auto b = graph.find_vertex("V" + std::to_string(((i + 1) % 50) + 1));
            graph.add_edge(a, b, 100);
        }
    }

    auto res = gal::dijkstra<int>(graph, graph.find_vertex("V1"), graph.find_vertex("V50"));

    assert(!res.empty());

    auto res2 = gal::tarjan_scc(graph);

    assert(gal::is_strongly_connected(graph));

    for (auto e : res) {
        std::cout << *(e.get_from()) << " --( " << *e << " )--> " << (*e.get_to()) << std::endl;
    }

    return 0;
}