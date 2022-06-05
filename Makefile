a: ./graph/main.cpp ./graph/graph.hpp ./graph/graph_algorithms.hpp
	clang++ -Wall -std=c++20 -o a ./graph/main.cpp

test: a
	./a