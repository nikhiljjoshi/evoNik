//
//  nAnalyzer.hpp
//  evoNik
//
//  Created by Nikhil Joshi on 5/30/12.
//  Copyright (c) 2012 California Institute of Technology. All rights reserved.
//

#ifndef evoNik_nAnalyzer_hpp
#define evoNik_nAnalyzer_hpp

#include "utility.hpp"
#include "nAgent.hpp"
#include "nMaze.hpp"
#include "nGame.hpp"
#include "ModularityToolset/ModularityToolset.h"
#include "ModularityToolset/PartitionEnumerator.h"

// forward declaration for nAgent
class nAgent;

class nAnalyzer{
public:
    // the agent to be analyzed
    nAgent m_agent;
    // the analysis output
    std::ostream* m_analysisOutput;
    // use brainScan or mazeData
    bool m_useBrainScan;
    // list of requirements
    std::vector<std::string> m_requirements;
    
    
    // constructor
    nAnalyzer(unsigned int playerID, nGenome playerGenome, double playerFitness, std::ostream& output = std::cout, bool scanBrain = useBrainScan)
    : m_analysisOutput(&output),
    m_useBrainScan(useBrainScan){
        m_agent.m_id = playerID;
        m_agent.m_genome = playerGenome;
        m_agent.m_fitness = playerFitness;
        m_agent.buildHMMs();
        
       // this->run();
        // newline 
        //*m_analysisOutput << std::endl;
    }
    
    
    // destructor
    ~nAnalyzer(){
    }
    
    // member functions
    // set the computational requirements
    void setRequirements(const char* requirements);
    // run analysis
    void run(void);
    // print genomic details
    void calculateGenomics(void);
    // collect data for analysis
    void collectData(void);
    // phiCalculation module
    void calculatePhi(unsigned int timeStepDelay = 0);
    // calculate mutual information
    void calculateMutualInfo(unsigned long xMask = 0, unsigned long yMask = 0, unsigned int timeStepDelay = 0);
    
    void analyze(bool test) { }
};


#endif
