# graph
A simple header only library for directed graphs.

### graph.hpp
`graph<VT, ET>`: A directed graph with internal vertex data of type `VT` and internal edge data of `ET`. Edges `(v,w)` are not unique. Edges adjacent to vertex `v` are cached and returned at zero cost.

### graph_algorithms.hpp
A collection of algorithms that operate on a graph `G`.
- [X] `find_vertex(G, d)` - Find a vertex with matching internal data in `O(|V|)`
- [X] `dijkstra(G, v, w)` - Dijkstra's pathfinding from `v` to `w`
- [X] `search(G, v, pred)` - Breadth first search return finding the first instead matching `pred`
- [X] `path_exists(G, v, w)` - Breadth first search to determine if a path from `v` to `w` exists
- [X] `will_cycle(G, v, w)` - Check if creating edge `(v,w)` will cycle by checking for path from `w` to `v`
- [X] `tarjan_scc(G)` - Return a graph where `V` contains strongly connected clusters of vertices in `G`
- [X] `is_strongly_connected(G)` - Run `tarjan_scc(G)` and check if there is only one cluster
- [X] `cycles(G)` - Run `tarjan_scc(G)` and check if there are no clusters
- [X] `get_random(n, m, Gv, Ge)` - Generate a random graph with `|V|=n, |E|=m`, with generator functions
