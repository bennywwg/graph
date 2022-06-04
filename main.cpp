#include <iostream>
#include <string>

#include "graph.hpp"
#include "graph_algorithms.hpp"

using std::string;

int main(int argc, char** argv) {
    using gal = graph_algorithm<string, int>;

    while (true) {
        graph<string, int> graph = gal::get_random(
            50,
            100,
            [](size_t i) {
                return "V" + std::to_string(i + 1);
            },
            [](size_t i) {
                return rval(5, 20);
            }
            );

        auto res = gal::dijkstra<int>(graph, graph.find_vertex("V1"), graph.find_vertex("V50"));

        if (res.size() > 10) {
            for (auto e : res) {
                std::cout << *(e.get_from()) << " ---(" << *e << ")--> " << (*e.get_to()) << std::endl;
            }
            break;
        }
    }

    return 0;
}