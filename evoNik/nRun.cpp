//
//  nRun.cpp
//  evoNik
//
//  Created by Nikhil Joshi on 4/20/12.
//  Copyright (c) 2012 California Institute of Technology. All rights reserved.
//

#include <iostream>

#include "nRun.hpp"

void nRun::init(){
    
    // create storage directory (if doesn't exist)
    fs::path currentDir = fs::current_path();
        
    std::string directorySuffix = dateTimeStamp(false);
    directorySuffix.erase(directorySuffix.begin()+11, directorySuffix.end());
    
    std::ostringstream buffer;
    buffer << m_runName << "_" << directorySuffix;
    
    m_dataDirectory = currentDir/buffer.str();
    
    if(!fs::exists(m_dataDirectory)){
        fs::create_directory(m_dataDirectory);
    }
    
    // create this run specific directory (Always a new)
    std::string creationTime = dateTimeStamp(false);
    creationTime.erase(creationTime.begin(), creationTime.begin()+11);
    
    buffer.clear();
    buffer.str("");
    buffer << "Run_" << m_id << creationTime;
    
    m_thisRunDirectory = m_dataDirectory/buffer.str();
    
    fs::create_directory(m_thisRunDirectory);
    
    // open files
    m_lodFile.open((m_thisRunDirectory.string()+"/lod_output.txt").c_str(), std::ios::out | std::ios::app);
    m_knockoutFile.open((m_thisRunDirectory.string()+"/knockout.txt").c_str(), std::ios::out | std::ios::app);
    m_analysisFile.open((m_thisRunDirectory.string()+"/analysisData.txt").c_str(), std::ios::out | std::ios::app);
    m_progressFile.open((m_thisRunDirectory.string()+"/progressData.txt").c_str(), std::ios::out | std::ios::app);
    
    
    // header in analysis file
    m_analysisFile << "# gen\tagentID\tfitness\tPhiMC\tMC\tMCnodes\tMItot\tMIpred\tgenNumSite\tgenLenUncompr\tgenLenCompr" << std::endl;
    
    // header in progress file
    m_progressFile << "# gen \t ave. fitness\tMax. fitness" << std::endl;
    
}

void nRun::go(){
    
    // einstein
//    std::fstream einsteinBrain;
//    einsteinBrain.open("einstein.txt", std::ios::in);
//    nAgent einstein;
//    einstein.loadGenomeFromFile(einsteinBrain);
    
    // populations
    std::vector<nPopulation*> generations;
    
    // maze 
    nMaze runMaze(evaluationTime + 10, 15);
    
    // game
    nGame runGame(runMaze);
    runGame.setKnockoutStream(m_knockoutFile);
       
    // initial population
    nPopulation* initPopulation = new nPopulation;
    initPopulation->setLODoutputStream(m_lodFile);
    initPopulation->setAnalysisOutputStream(m_analysisFile);
    initPopulation->populate();
    
    // add to generation
    generations.push_back(initPopulation);
    
    // iterate over generations
    for (unsigned int gen = 0; gen < maxGenerations + 10; gen++) {
        
        // update the test maze after every 100 generations
        if (gen % 100 == 0 && gen != 0) {
            runMaze.create();
            runGame.updatePlayGround(runMaze);
        }

        generations.back()->evaluate(runGame);
          
        m_progressFile << generations.back()->getGenerationID() << "\t"
        << generations.back()->getAverageFitness() << "\t"
        << generations.back()->getMaxFitness() << std::endl;
        
        if (!suppressMessages)
            std::cout << "Gen. no. " << generations.back()->getGenerationID()
            << "\tAve. fitness = " << generations.back()->getAverageFitness() 
            << "\tMax. fitness = " << generations.back()->getMaxFitness() << std::endl;
        
        nPopulation* newPop = new nPopulation(generations.back()->reproduce());
        newPop->setLODoutputStream(m_lodFile);
        newPop->setAnalysisOutputStream(m_analysisFile);
        generations.push_back(newPop);
        
        // clean up empty generations
        if (generations[0]->getMembers().size() == 0) {
            delete generations[0];
            generations.erase(generations.begin());
        }
        
    }
    
    // complete the LOD and knockout and analysis
    // (the latter generations have more than one species and hence not removed)
    generations.back()->rank();
    this->dumpRemainingLODandKnockout(generations.back()->getGenerationID(),
                                      *(generations.back()->getMembers())[0],
                                      runGame);
}


void nRun::dumpRemainingLODandKnockout(unsigned int genID, nAgent& a, nGame& game){
    // if the agent has parents, first dump them
    // (this is done to preserve the order in the knockoutfile)
    if (a.m_parents.size() != 0)
        dumpRemainingLODandKnockout(genID - 1, *a.m_parents[0], game);    // for mutational inheritance there is only one parent
    
    // store this guy to the LOD file
    m_lodFile << "# Gen no. " << genID
    << " (agent fitness = " << a.m_fitness
    << "):" << std::endl;
    
    a.printGenome(m_lodFile);
    
    // perform knockout analysis for the this guy
    nAgent* origPlayer = game.getPlayer();
    a.m_alive = true;
    game.updatePlayer(a);
    *(game.m_knockoutOutput) << "# Gen no. " << genID << ":" << std::endl;
    game.profilePlayerKnockout();
    a.m_alive = false;
    game.updatePlayer(*origPlayer); 
    
    // perform analysis after a specified interval
    if (genID % analysisInterval == 0) {
        m_analysisFile << genID << "\t";
        nAnalyzer analyzer(a.m_id, a.m_genome, a.m_fitness, m_analysisFile);
        analyzer.setRequirements("all");
        analyzer.run();
    }
    
}


void nRun::close(){
    
    
    // close files
    m_lodFile.close();
    m_knockoutFile.close();
    m_analysisFile.close();
}
