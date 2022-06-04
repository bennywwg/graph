#include <iostream>
#include <string>

#include "graph.hpp"
#include "graph_algorithms.hpp"

using std::string;

int main(int argc, char** argv) {
    using gal = graph_algorithm<string, int>;

    graph<string, int> graph = gal::get_random(
        50,
        200,
        [](size_t i) {
            return "V" + std::to_string(i + 1);
        },
        [](size_t i) {
            return rval(5, 20);
        }
    );

    return 0;
}