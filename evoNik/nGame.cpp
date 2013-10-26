//
//  nGame.cpp
//  evoNik
//
//  Created by Nikhil Joshi on 4/13/12.
//  Copyright (c) 2012 California Institute of Technology. All rights reserved.
//

#include "nGame.hpp"

bool nGame::isValid(){
    // if a valid playGround exists
    return m_playGround->isValid();
}


void nGame::updatePlayGround(nMaze& maze){
    m_playGround = &maze;
    constructFitnessLandscape();
}


void nGame::execute(unsigned int lapTime){
    
    // confirm a valid maze
    if (!m_playGround->isValid()){
        std::cerr << "Error in nGame: not a valid maze!" << std::endl;
        exit(1);
    }
    
    // check the agent is valid
    if (!m_player->isValid()) {
        std::cerr << "Error in nGame: not a valid player!" << std::endl;
        exit(1);
    }
        
    // if food is required and the agent is not a solver 
    if (huntForFood &&
        m_player->m_id != 1234567890){
        // adjust the lap time with deficit
        lapTime = static_cast<unsigned int>(lapTime*(1.0 - agentDeficit));
        // replenish the food in the maze
        m_playGround->replanishFood();
    }
    
    // place the player in the maze (in front of the first door)
    m_player->m_position = position(0, (m_playGround->getDoors())[0].y);
    // update history
    m_player->m_trajectory.push_back(m_player->m_position);
    
    // reset clock
    unsigned int timeStep(0);
    // fitness for this test
    double fitness(0.0);
    unsigned int completedLaps(0);
    
    while (timeStep++ < lapTime) {
                
        // if the agent is alive
        if (m_player->m_alive){
                
            // current state becomes previous state
            m_player->m_prevState = m_player->m_curState;
            
            // make it fresh-minded
            m_player->m_curState = 0;
            
            // expose the local ground
            exposePlayGround();
            
            // update brain state, if environmental update is included
            if (includeEnvUpdate && !m_player->m_stateHistory.empty()) 
                m_player->m_stateHistory.back().second = m_player->m_prevState;
                
            // let the player decide action
            m_player->updateBrain();
    
            // should I eat any food?
            if (huntForFood && m_player->m_id != 1234567890) {
                // if mouth (bit # 9) is open and if the food was not already consumed
                if (((m_player->m_curState >> 9)&1) && 
                    !((m_playGround->getFloorPlan()[m_player->m_position.x][m_player->m_position.y] >> 3)&1)) {
                    // if food is healthy
                    if (((m_playGround->getFloorPlan()[m_player->m_position.x][m_player->m_position.y] >> 2)&1) ) {
                        lapTime += 2; 
                        // keep lapTime lower than (maximum) evaluation time
                        if (lapTime > evaluationTime)
                            lapTime = evaluationTime;
                    }                    
                    else // food is poisonous
                        lapTime = (unsigned int)std::max((int)lapTime - 4, 0);
                                        
                    // make the food consumed
                    applyBit(m_playGround->getFloorPlan()[m_player->m_position.x][m_player->m_position.y], 3, 1);
                }
            }
            
            // move the player as per its action
            movePlayer();
            
            // if agent moved aginst gravity penalize
            if (gravityPresent &&
                m_player->m_prevPosition.y > m_player->m_position.y)
                timeStep++;
            
            // update the fitness of the player
            if (m_player->m_id != 1234567890)    // for solver don't worry about fitness
                fitness = m_playGround->getFitnessLandscape()[m_player->m_position.x][m_player->m_position.y];
            
            // if it reached the goal
            if (fitness == 1) {
                // completed one (more) lap
                completedLaps++;
                // reset fitness
                fitness = 0;
                // place the agent at the maze entry again
                m_player->m_position = position(0, (m_playGround->getDoors())[0].y);
            }
            
        }
        
    }
    
    // update the fitness for "this" agent
    m_player->updateFitness(fitness + completedLaps, useGeometricMean);
    
    // replanish food (for next execution)
    m_playGround->replanishFood();
    
}

void nGame::exposePlayGround(){
    
//    // bit 0 : front retina
//    applyBit(m_player->m_curState, 0, 
//             m_playGround->m_plan[m_player->m_position.x + 1][m_player->m_position.y]&1);
//    // bit 1 : left retina
//    applyBit(m_player->m_curState, 1, 
//             m_playGround->m_plan[m_player->m_position.x + 1][m_player->m_position.y - 1]&1);
//    // bit 2 : right retina
//    applyBit(m_player->m_curState, 2, 
//             m_playGround->m_plan[m_player->m_position.x + 1][m_player->m_position.y + 1]&1);
//    // bit 3 : door sensor
//    applyBit(m_player->m_curState, 3, 
//             (m_playGround->m_plan[m_player->m_position.x][m_player->m_position.y]>>1)&1);
//    // bit 4 : left collision sensor
//    applyBit(m_player->m_curState, 4, 
//             m_playGround->m_plan[m_player->m_position.x][m_player->m_position.y - 1]&1);
//    // bit 5 : right collision sensor
//    applyBit(m_player->m_curState, 5, 
//             m_playGround->m_plan[m_player->m_position.x][m_player->m_position.y + 1]&1);
//   
//    
//    // do the same to the previous state (in order to avoid environmental update counting)
//    
//    // bit 0 : front retina
//    applyBit(m_player->m_prevState, 0, 
//             m_playGround->m_plan[m_player->m_position.x + 1][m_player->m_position.y]&1);
//    // bit 1 : left retina
//    applyBit(m_player->m_prevState, 1, 
//             m_playGround->m_plan[m_player->m_position.x + 1][m_player->m_position.y - 1]&1);
//    // bit 2 : right retina
//    applyBit(m_player->m_prevState, 2, 
//             m_playGround->m_plan[m_player->m_position.x + 1][m_player->m_position.y + 1]&1);
//    // bit 3 : door sensor
//    applyBit(m_player->m_prevState, 3, 
//             (m_playGround->m_plan[m_player->m_position.x][m_player->m_position.y]>>1)&1);
//    // bit 4 : left collision sensor
//    applyBit(m_player->m_prevState, 4, 
//             m_playGround->m_plan[m_player->m_position.x][m_player->m_position.y - 1]&1);
//    // bit 5 : right collision sensor
//    applyBit(m_player->m_prevState, 5, 
//             m_playGround->m_plan[m_player->m_position.x][m_player->m_position.y + 1]&1);
   
    
    
    // expose the local play ground to the player
    // bit 0 : retina
    applyBit(m_player->m_curState, 0, 
             (m_playGround->getFloorPlan())[m_player->m_position.x + 1][m_player->m_position.y]&1);
    applyBit(m_player->m_prevState, 0, 
             (m_playGround->getFloorPlan())[m_player->m_position.x + 1][m_player->m_position.y]&1);
    
    // bit 1 : left collision sensor
    applyBit(m_player->m_curState, 1, 
             (m_playGround->getFloorPlan())[m_player->m_position.x][m_player->m_position.y - 1]&1);
    applyBit(m_player->m_prevState, 1, 
             (m_playGround->getFloorPlan())[m_player->m_position.x][m_player->m_position.y - 1]&1);
 
    // bit 2 : right collision sensor
    applyBit(m_player->m_curState, 2, 
             (m_playGround->getFloorPlan())[m_player->m_position.x][m_player->m_position.y + 1]&1);
    applyBit(m_player->m_prevState, 2, 
             (m_playGround->getFloorPlan())[m_player->m_position.x][m_player->m_position.y + 1]&1);
    
    // bit 3 : door sensor
    applyBit(m_player->m_curState, 3, 
             ((m_playGround->getFloorPlan())[m_player->m_position.x][m_player->m_position.y]>>1)&1);
    applyBit(m_player->m_prevState, 3, 
             ((m_playGround->getFloorPlan())[m_player->m_position.x][m_player->m_position.y]>>1)&1);
    
    // bit 4 : food smell sensor
    applyBit(m_player->m_curState, 4, 0);
    applyBit(m_player->m_prevState, 4, 0);
    if (huntForFood) {
        applyBit(m_player->m_curState, 4, 
                 ((m_playGround->getFloorPlan())[m_player->m_position.x][m_player->m_position.y]>>2)&1);
        applyBit(m_player->m_prevState, 4, 
                 ((m_playGround->getFloorPlan())[m_player->m_position.x][m_player->m_position.y]>>2)&1);
    }        
    
    // bit 5 : gravity pull sensor
    applyBit(m_player->m_curState, 5, 0);
    applyBit(m_player->m_prevState, 5, 0);
    if (gravityPresent) {
        // if the player moved against gravity
        if (m_player->m_prevPosition.y > m_player->m_position.y){
            applyBit(m_player->m_curState, 5, 1);
            applyBit(m_player->m_prevState, 5, 1);
        }
    }
    
}

void nGame::movePlayer(){
    
    // player's current position
    m_player->m_prevPosition = m_player->m_position;
    
    // action = bit 10(left actuator) + 2* bit 11(right actuator)
    int action = (m_player->m_curState >> 10)&1 + 2*((m_player->m_curState >> 11)&1);    
    
    switch (action) {
        case 1:    // left actuator on
            m_player->m_position.y--;
            break;
        case 2:   // right actuator on
            m_player->m_position.y++;
            break;
        case 3:   // both actuators on
            m_player->m_position.x++;
            break;
        default:  // both actuators off
            break;
    }
    
    // check if the player sits on top of a wall
    // if so, take it back to previous position
    if ((m_playGround->getFloorPlan())[m_player->m_position.x][m_player->m_position.y] == 1)
        m_player->m_position = m_player->m_prevPosition;
    
    // update the trajectory
    m_player->m_trajectory.push_back(m_player->m_position);
    
}


void nGame::printGame(std::ostream& output){
    
    // if no player or playGround
    if (m_player == NULL || m_playGround == NULL) {
        std::cerr << "Error in nGame: either player or playGround is unavailable" << std::endl;
        exit(1);
    }
    
    // if the player is blank/fresh
    if (m_player->m_trajectory.empty()) {
        execute();
    }
    
    // confirm trajectory and brain-history mapping
    if (m_player->m_trajectory.size() != m_player->m_stateHistory.size()) {
        std::cerr << "Error in nGame: Can not print game data" << std::endl;
        std::cerr << "Trajectory and brain history data size mismatch!" << std::endl;
        exit(0);
    }

    size_t size = m_player->m_trajectory.size();
    
    // header
    output << "timeStep\tbrainPre\tbrainCur\txPos\tyPos" << std::endl;
    
    for (size_t i = 0; i < size; i++) {
        output << i << "\t"
        << m_player->m_stateHistory[i].first << "\t"
        << m_player->m_stateHistory[i].second << "\t"
        << m_player->m_trajectory[i].x << "\t"
        << m_player->m_trajectory[i].y << std::endl;
    }
    
    
}

void nGame::profilePlayerKnockout(){
    
    *m_knockoutOutput << m_player->m_id << ":" << std::endl;
    
    // fitness and lifeline of the player
    double regFitness = m_player->m_fitness;
    
    // knockout fitnesses
    std::vector<double> KOfitnesses;
    
    // for each node
    for (unsigned int node = 0; node < maxNodes; node++) {
        
        // for each possible bit value
        for (int i = 0; i < 2; i++) {
            
            // reset player fitness
            m_player->resetFitness();
            // set bit to i
            m_player->setMask(node, i);
            // calculate fitness
            this->execute();
            // fitness (bit = i)
            KOfitnesses.push_back(m_player->m_fitness);
        }
        // write to file
        *m_knockoutOutput << node << ":" << KOfitnesses[0] << ", " << KOfitnesses[1] << "\t";   
        
        // clear fitnesses
        KOfitnesses.clear();
    }
    *m_knockoutOutput << std::endl;
    
    // revert the original fitness and lifeline of the agent
    m_player->m_fitness = regFitness;
}


void nGame::constructFitnessLandscape(){
    
    // check a valid playGround
    if (!isValid()) 
        std::cerr << "Error in nGame: not a valid playGround!\n" 
        << "can not construct fitness landscape" << std::endl;
    
    // erase previous fitness landscape
    m_playGround->getFitnessLandscape().clear();
    
    // set a reasonable goal in the maze
    // (reach of "Einstein")
    
    // boost assign for += operator overload
    using namespace boost::assign;
    
    // create solver
    nAgent solver(1234567890);
        
    // solver has the "perfect" genome (that of Einstein)
    solver.m_genome.m_genome += 42, 213,1,1,6,0,0,0,11,10,0,0,
    0,0,0,4095,
    0,0,4095,0,
    0,0,0,4095,
    0,4095,0,0,
    42,213,3,0,6,1,2,3,6,0,0,0,
    4095,0,
    4095,0,
    4095,0,
    4095,0,
    0,4095,
    0,4095,
    4095,0,
    0,4095,
    0,4095,
    0,4095,
    4095,0,
    4095,0,
    0,4095,
    0,4095,
    4095,0,
    0,4095;

//    solver.m_genome.m_genome += 42,213,3,1,0,1,2,6,11,10,0,0,
//    0,0,0,10000,
//    0,0,0,10000,
//    0,0,0,10000,
//    0,0,0,10000,
//    0,0,0,10000,
//    0,0,0,10000,
//    0,0,0,10000,
//    0,0,0,10000,
//    0,0,10000,0,
//    0,10000,0,0,
//    0,0,10000,0,
//    0,0,10000,0,
//    0,10000,0,0,
//    0,10000,0,0,
//    0,0,10000,0,
//    0,10000,0,0,
//    42,213,3,0,6,5,4,3,6,0,0,0,
//    10000,0,
//    10000,0,
//    0,10000,
//    0,10000,
//    10000,0,
//    10000,0,
//    10000,0,
//    0,10000,
//    0,10000,
//    0,10000,
//    0,10000,
//    0,10000,
//    10000,0,
//    10000,0,
//    10000,0,
//    0,10000;
    
    
    // hold original player
    nAgent* origPlayer = m_player;
    
    // send solver into the maze
    updatePlayer(solver);
    
    if (!suppressMessages)
        std::cout << "Solver is solving the maze" << std::endl;
   
    // make him solve
    execute(m_playGround->getX() - 10);

    // that's the goal
    position goal = m_player->m_position;
    
    // construct fitness landscape
    nDijkstra dijk;
    dijk.buildGraph(m_playGround->getFloorPlan(), false);
    dijk.computeFitnessArray(m_playGround->getFitnessLandscape(),
                             goal.x, 
                             goal.y);
    
    if (!suppressMessages)
        std::cout << "Done solving! Updated fitness landscape" << std::endl;
    
    // if food is required
    if (huntForFood)
        m_playGround->sprinkleFood();    

    
    // restore previous player
    updatePlayer(*origPlayer);

}
