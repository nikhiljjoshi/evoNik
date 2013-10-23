//
//  nDijkstra.hpp
//  evoNik
//
//  Created by Nikhil Joshi on 4/28/12.
//  Copyright (c) 2012 California Institute of Technology. All rights reserved.
//

#ifndef evoNik_nDijkstra_hpp
#define evoNik_nDijkstra_hpp

# include <cmath>
#include <iostream>

#include <boost/config.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

#include "constants.hpp"

class nDijkstra_map2d{
public:
    std::map< std::pair< unsigned int, unsigned int > , int > sectorMap;
    int add(unsigned int x, unsigned int y);
    int get(unsigned int x, unsigned int y);
    void clear(void)                           {  sectorMap.clear();  }
};

class nDijkstra{
public:
    nDijkstra_map2d sectorMap;
    size_t height;
    size_t width;
    int old_goal_w;
    int old_goal_h;
    
    
    typedef boost::adjacency_list <boost::vecS, boost::vecS, boost::undirectedS, boost::no_property, boost::property <boost::edge_weight_t, double> > graph_t;
    typedef boost::graph_traits < graph_t >::vertex_descriptor vertex_descriptor;
    typedef boost::graph_traits < graph_t >::edge_descriptor edge_descriptor;
    
    graph_t test_graph;
    
    // constructor
    nDijkstra(){
        old_goal_w = old_goal_h = -1;
    }

    void buildGraph(const std::vector<std::vector<unsigned int> > & freeSpaceArray, 
                    bool diag);
    void computeFitnessArray(std::vector<std::vector<double> > & fitnessArray,
                             int goal_w,
                             int goal_h, 
                             bool cache=true);
    
};

#endif
