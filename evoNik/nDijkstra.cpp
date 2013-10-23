//
//  nDijkstra.cpp
//  evoNik
//
//  Created by Nikhil Joshi on 4/28/12.
//  Copyright (c) 2012 California Institute of Technology. All rights reserved.
//

#include "nDijkstra.hpp"

int nDijkstra_map2d::add(unsigned int x, unsigned int y) {
    std::pair<unsigned int,size_t> tempPair(x,y);
    int id = -1;
    if (sectorMap.count(tempPair)==0) {
        id = (int)sectorMap.size();
        sectorMap[tempPair] = id;
    } 
    else 
        id = sectorMap[tempPair];
    
    return id;
}

int nDijkstra_map2d::get(unsigned int x, unsigned int y) {
    std::pair<unsigned int,size_t> tempPair(x,y);
    int id = -1;
    if (sectorMap.count(tempPair)) {
        id = sectorMap[tempPair];
    }
    
    return id;
}


void nDijkstra::computeFitnessArray(std::vector<std::vector<double> > & fitnessArray,
                                    int goal_w,
                                    int goal_h, 
                                    bool cache){
    
    // added by NJJ: 
    // don't care for calculating the fitness landscape for whole maze... only upto the reach of 
    // the best guy "Einstein" (plus a few blocks ahead)
    width = goal_w+2;
    
    fitnessArray.clear();
    fitnessArray.resize(width);
    for (size_t i = 0; i < fitnessArray.size(); i++) {
        fitnessArray[i].resize(height);
    }

    if ((goal_w == old_goal_w) && (goal_h == old_goal_h) && cache ) {
        return;
    }
    old_goal_h = goal_h;
    old_goal_w = goal_w;
    if (!suppressMessages) 
        std::cout << "Running Dijkstra to goal at " <<goal_w<<","<<goal_h<<std::endl;
  
    std::vector<vertex_descriptor> p(num_vertices(test_graph));
    std::vector<double> distance(num_vertices(test_graph));
    
    int s_int = sectorMap.get(goal_w,goal_h);
    if (s_int < 0)
        std::cerr << "source id is negative= " << s_int << std::endl;

    vertex_descriptor s = s_int;
    boost::property_map<graph_t, boost::vertex_index_t>::type indexmap(get(boost::vertex_index, test_graph));
    boost::property_map<graph_t, boost::edge_weight_t>::type weightmap(get(boost::edge_weight, test_graph));
    boost::dijkstra_shortest_paths(test_graph, s, &p[0], &distance[0], weightmap, indexmap,
                                   std::less<double>(), boost::closed_plus<double>(),
                                   (std::numeric_limits <double>::max)(), 0,
                                   boost::default_dijkstra_visitor());
    
    if (!suppressMessages)
        std::cout << "dijkstra done."<<std::endl;
    
    double max_val = 0;
    for(unsigned int w = 0; w < width; ++w) {
        for(unsigned int h = 0; h < height; ++h) { 
            int id = sectorMap.get(w,h);
            if (id >= 0) {
                if (distance[id] < (std::numeric_limits <double>::max)()) {
                    fitnessArray[w][h] = distance[id];
                    if (distance[id] > max_val) {
                        max_val = distance[id];
                    }
                } else {
                    fitnessArray[w][h] = -2.0;
                }
                
            } else {
                fitnessArray[w][h] = -1.0;
            }
            
        }
    }
    
    if (!suppressMessages)
        std::cout << " max_distance="<<max_val<<std::endl;
    
    for(unsigned int w = 0; w < width; ++w) {
        for(unsigned int h = 0; h < height; ++h) { 
            int id = sectorMap.get(w,h);
            if (id >= 0) {
                if (fitnessArray[w][h] >= 0) {
                    fitnessArray[w][h] = (max_val - fitnessArray[w][h])/max_val;
                }
            } else {
                fitnessArray[w][h] = -1;
            }
        }
    }
}


void nDijkstra::buildGraph(const std::vector<std::vector<unsigned int> > & freeSpaceArray, 
                           bool diag) {
    
    // set width and height
    width = freeSpaceArray.size();
    height = freeSpaceArray.front().size();
    
    old_goal_w = old_goal_h =-1;
    test_graph.clear();
    sectorMap.clear();
    
    //Now to populate the edges of the graph.
    //iterate over each node, adding southernly and easterly edges
    double east_weight = 1; 
    double south_weight = 1; 
    double southeast_weight = sqrt(2.0); 
    double northeast_weight = southeast_weight;
    
    for(unsigned int w = 0; w < freeSpaceArray.size(); ++w) {
        for(unsigned int h = 0; h < freeSpaceArray[w].size(); ++h) { 
            if ((freeSpaceArray[w][h]&1) == 0) {
                int cur_node_number = sectorMap.add(w,h);
                if( w+1 < freeSpaceArray.size() ) { // add east edge
                    if ((freeSpaceArray[w+1][h]&1) ==0) {
                        int east_node_number = sectorMap.add(w+1, h);
                        add_edge (cur_node_number, east_node_number,east_weight,test_graph);
                    }
                }
                if( h+1 < freeSpaceArray[w].size()) {
                    if ((freeSpaceArray[w][h+1]&1) ==0) {
                        int south_node_number = sectorMap.add(w, h+1);
                        add_edge (cur_node_number, south_node_number,south_weight,test_graph);
                    }
                }
                if (diag) {
                    if ((w+1 < freeSpaceArray.size()) && (h+1 < freeSpaceArray[w].size())) {
                        if ((freeSpaceArray[w+1][h+1]&1) ==0) {
                            int southeast_node_number = sectorMap.add(w+1, h+1);
                            add_edge (cur_node_number, southeast_node_number,southeast_weight,test_graph);
                        }
                    }
                    if ((w+1 < freeSpaceArray.size()) && (h >= 1)) {
                        if ((freeSpaceArray[w+1][h-1]&1) ==0) {
                            int northeast_node_number = sectorMap.add(w+1, h-1);
                            add_edge (cur_node_number, northeast_node_number,northeast_weight,test_graph);
                        }
                    }
                }
            }
        }
    }
}

