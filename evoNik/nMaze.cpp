//
//  nMaze.cpp
//  evoNik
//
//  Created by Nikhil Joshi on 3/21/12.
//  Copyright (c) 2012 California Institute of Technology. All rights reserved.
//

// maze dimensions are 
// 0(left) to m_x (right) 
// 0 (top) to m_y (bottom)
// 0 is a wall
// 1 is a door

#include "nMaze.hpp"

bool nMaze::isValid(){
    
    // check if zero area
    if (m_x ==0 || m_y==0)
        return false;
    
    // if floor plan exists
    if (m_plan.empty())
        return false;
    
    return true;
}


void nMaze::create(){
    
    // clear previous plan
    m_plan.clear();
    m_doors.clear();
       
    // create floor plan area
    m_plan.resize(m_x);
    for (size_t i = 0; i < m_x; i++) {
        m_plan[i].resize(m_y);
    }
    
    // create a wall border
    for (size_t i = 0; i < m_x; i++) {
        m_plan[i][0] = 1;   // top
        m_plan[i][m_y-1] = 1; // bottom
    }
    
    // create obstructing walls 
    // (with one hole at random position)
    unsigned int xPos(1), doorPos(0);
    unsigned int prevWallPos(-1), prevDoorPos(m_y);
    
    while (xPos < m_x-1) {
        // creat a wall
        for (size_t i = 0; i < m_y; i++)
            m_plan[xPos][i] = 1;
        
        // bore a hole at some random location
        if (xPos != prevWallPos+1)     // if adjescent walls, match door positions, else
            doorPos = (xPos == 1)? (unsigned int)genUniRand(2, m_y-2) : (unsigned int)genUniRand(1, m_y-1);   // first wall door is not at extreme y values 
        
        m_plan[xPos][doorPos] = 0;
        
        // update door list
        m_doors.push_back(position(xPos, doorPos));

        // if "this" door is on the right of "previous"
        // insert a flag (second bit in floor plan is set)
        if (doorPos > prevDoorPos)
            m_plan[prevWallPos][prevDoorPos]=2;
        
        // "this" becomes previous wall n door        
        prevWallPos = xPos;
        prevDoorPos = doorPos;
        
        // choose next wall position 
        // at a distance (1-4) units from this wall
        xPos += (unsigned int)genUniRand(1, 4);
    }
    
}


void nMaze::sprinkleFood(){
    // move all over the maze    
    for (unsigned int xPos = 0; xPos < m_x; xPos++)
        for (unsigned int yPos = 1; yPos < m_y - 1; yPos++)
            
            // if there is no wall at xPos, yPos create a food bag
            if (m_plan[xPos][yPos] != 1)                
                // adjust the "third" bit of m_plan to have food item
                // either a healthy (1) or poisonous food (0) 
                // randomly
                if (genUniRand(0, 1) > 0.5)
                    applyBit(m_plan[xPos][yPos], 2, 1);
}


void nMaze::printMaze(std::ostream& fout){
    
    // header
    fout << "# The maze (floor) plan: " << std::endl;
    
    for (size_t y = 0; y < m_y; y++) {
        for (size_t x = 0; x < m_x; x++) 
            fout << m_plan[x][y] << "\t";
        fout << std::endl;
    }
}

void nMaze::replanishFood(){
    
    // the "forth" bit in m_plan corresponds to a consumed food
    // make it available again
    for (unsigned int xPos = 0; xPos < m_x; xPos++)
        for (unsigned int yPos = 1; yPos < m_y - 1; yPos++)
            if (m_plan[xPos][yPos] != 1)
                applyBit(m_plan[xPos][yPos], 3, 0);
}
