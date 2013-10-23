//
//  nGame.hpp
//  evoNik
//
//  Created by Nikhil Joshi on 4/12/12.
//  Copyright (c) 2012 California Institute of Technology. All rights reserved.
//

#ifndef evoNik_nGame_hpp
#define evoNik_nGame_hpp

#include <algorithm>

#include "utility.hpp"
#include "nMaze.hpp"
#include "nAgent.hpp"

class nGame{
public:
    nAgent* m_player;
    nMaze* m_playGround;
    
    // player knockout data stream
    std::ostream* m_knockoutOutput;
    
    // constructor with only a maze
    nGame(nMaze& maze)
    :m_playGround(&maze),        
    m_knockoutOutput(&std::cout){
        constructFitnessLandscape();
        
    }
    
    // constructor
    nGame(nAgent& player, nMaze& playGround): 
    m_player(&player), 
    m_playGround(&playGround),
    m_knockoutOutput(&std::cout){
        constructFitnessLandscape();
    }
        
    // destructor
    ~nGame(){
        
    }
    
    // member functions
    // check validity
    bool isValid(void);
    // update player
    void updatePlayer(nAgent& a)                      { m_player = &a; }
    // update playGround
    void updatePlayGround(nMaze& maze);
    // lay fitness landscape
    void constructFitnessLandscape(void);
    // execute the game
    void execute(unsigned int lapTime = evaluationTime);
    // dump the game data (brain, trajectory etc)
    void printGame(std::ostream& output = std::cout);
    // expose local playGround to the player
    void exposePlayGround(void);
    // let the player generate action
    void movePlayer(void);
    // perform player knockout analysis
    void profilePlayerKnockout();
    // set player knockout output stream
    void setKnockoutStream(std::ostream& output)       { m_knockoutOutput = &output; }
    
};


#endif
