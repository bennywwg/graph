a: main2.cpp graph.hpp graph_algorithms.hpp
	clang++ -D _DEBUG -Wall -std=c++20 -o a main2.cpp

test: a
	./a