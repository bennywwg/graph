a: stack_vector.cpp main2.cpp graph.hpp graph_algorithms.hpp stack_vector.o
	clang++ -D _DEBUG -O3 -std=c++20 stack_vector.cpp main2.cpp -o a

test: a
	./a