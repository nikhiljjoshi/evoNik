//
//  nMaze.hpp
//  evoNik
//
//  Created by Nikhil Joshi on 3/21/12.
//  Copyright (c) 2012 California Institute of Technology. All rights reserved.
//

#ifndef evoNik_nMaze_hpp
#define evoNik_nMaze_hpp

#include "utility.hpp"
#include "nAgent.hpp"

class nMaze{
public:
    
    // constructor
    nMaze(unsigned int x, unsigned y):m_x(x), m_y(y){
        create();
    }
    
    // destructor
    ~nMaze()
    { }
    
    
    // member functions
    // get dimensions
    unsigned int getX(void)                                      { return m_x; }
    unsigned int getY(void)                                      { return m_y; }    
    // get door plan
    std::vector<position>& getDoors(void)                         { return m_doors;  }
    // get floor plan
    std::vector<std::vector<unsigned int>>& getFloorPlan(void)    { return m_plan; }
    // get fitness landscape
    std::vector<std::vector<double>>& getFitnessLandscape(void)   { return m_fitnessLandscape; }
    // create maze
    void create(void);
    // is it a valid maze
    bool isValid(void);
    // write or print
    void printMaze(std::ostream& fout = std::cout);
    // render maze
    void display(void);
    // sprinkle food items
    void sprinkleFood(void);
    // replanish fresh food
    void replanishFood(void);
    
private:
    //maze dimensions
    unsigned int m_x, m_y;
    // floor plan
    std::vector<std::vector<unsigned int> > m_plan;
    // fitness landscape
    std::vector<std::vector<double> > m_fitnessLandscape;
    // door position list
    std::vector<position> m_doors;
    
};

#endif
