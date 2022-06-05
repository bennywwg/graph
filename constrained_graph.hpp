#pragma once

#include "graph_algorithms.hpp"

/*
template<typename VT, typename ET>
class acyclic_graph : public graph<VT, ET> {
public:
    using super = graph<VT, ET>;

    inline super::ERef add_edge(super::VRef const& from, super::VRef const& to, ET const& e) {
        if (graph_algorithm<VT, ET>::will_cycle(*this, from, to)) throw graph_constraint_failure();

        return super::add_edge(from, to, e);
    }
};
*/