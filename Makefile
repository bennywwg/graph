a: main.cpp graph.hpp graph_algorithms.hpp
	clang++ -Wall -std=c++20 -o a main.cpp

test: a
	./a