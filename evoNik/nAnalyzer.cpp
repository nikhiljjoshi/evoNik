//
//  nAnalyze.cpp
//  evoNik
//
//  Created by Nikhil Joshi on 5/30/12.
//  Copyright (c) 2012 California Institute of Technology. All rights reserved.
//


#include "nAnalyzer.hpp"


void nAnalyzer::setRequirements(const char* requirements){
    boost::split(m_requirements, requirements, boost::is_any_of("\t ,"));
}


void nAnalyzer::run(){

    // check requirements and throw a warning
    if (m_requirements.empty()) {
        std::cerr << "Warning in nAnalyzer: no requirements set!" << std::endl
        << "nothing needs to be calculated" << std::endl;
    }
    
    else{

        if (std::find(m_requirements.begin(), m_requirements.end(), "all") != m_requirements.end() ||
            std::find(m_requirements.begin(), m_requirements.end(), "collectData") != m_requirements.end())
            // sample data for analysis
            collectData();
    
        if (std::find(m_requirements.begin(), m_requirements.end(), "all") != m_requirements.end() ||
            std::find(m_requirements.begin(), m_requirements.end(), "phi") != m_requirements.end())
            // Perform phi-related calculation
            calculatePhi();

        for (unsigned int i = 0; i <= calculateOverTimeDelays; i++) {

            if (std::find(m_requirements.begin(), m_requirements.end(), "all") != m_requirements.end() ||
                std::find(m_requirements.begin(), m_requirements.end(), "ipred") != m_requirements.end())
                // calculate predictive information
                calculateMutualInfo(0, 0, i);

            if (std::find(m_requirements.begin(), m_requirements.end(), "all") != m_requirements.end() ||
                std::find(m_requirements.begin(), m_requirements.end(), "smmi") != m_requirements.end())
                // calculate sensory-motor mutual information (SMMI)
                calculateMutualInfo(31, 3072, i);
            
        }
        
        if (std::find(m_requirements.begin(), m_requirements.end(), "all") != m_requirements.end() ||
            std::find(m_requirements.begin(), m_requirements.end(), "genLen") != m_requirements.end())
            // write genome lengths
            calculateGenomics();
    }
    
    // new line
    *m_analysisOutput << std::endl;
}


void nAnalyzer::calculateGenomics(){
    // calculate uncompressed length
    std::stringstream uncompressedGenome;
    for (std::vector<unsigned int>::iterator it = m_agent.m_genome.m_genome.begin();
         it != m_agent.m_genome.m_genome.end(); it++)
        uncompressedGenome << *it;
    
    // write to file
    // number of sites
    *m_analysisOutput << m_agent.m_genome.m_genome.size() << "\t"
    // uncompressed length
    << uncompressedGenome.str().size() << "\t"
    // compressed length
    << compressIt(m_agent.m_genome.m_genome).size() << "\t";
}


void nAnalyzer::collectData(){
    
    // if brain scan is not used
    // generate maze data
    if (!m_useBrainScan){
        
        // analysis period
        unsigned int analysisPeriod = 1000;
        
        // create a maze game
        nMaze analysisMaze(analysisPeriod + 10, 15);
        nGame analysisGame(analysisMaze);
        
        // announce the agent as player
        analysisGame.updatePlayer(m_agent);
        
        // for 40 different instances of maze
        for (int i = 0; i < 40; i++) {
            // execute the game 10 times
            for (int executionIndex = 0; executionIndex < 10; executionIndex++)
                analysisGame.execute(analysisPeriod);
            
            // change the maze
            analysisMaze.create();
            analysisGame.updatePlayGround(analysisMaze);
        }
        
    }
    
    // if the state history is empty
    if (m_agent.m_stateHistory.size() == 0) {
        std::cout << "Warning in nAgent::analyze(): No maze data available." << std::endl; 
        std::cout << "I will use brainScan for analysis!" << std::endl;
        m_agent.getBrainScan();
    }    
}

void nAnalyzer::calculatePhi(unsigned int timeStepDelay){
    
    
    // collect data if not available
    if (m_agent.m_stateHistory.empty())
        collectData();
    
    // initialize modularity toolset
    ModularityToolset toolset;
    
    // transition table
    MT_TRANSITION_TABLE transTable;
    
    for (std::vector<std::pair<unsigned long, unsigned long> >::iterator it = m_agent.m_stateHistory.begin();
         it != m_agent.m_stateHistory.end() - timeStepDelay; it++){
        
        MT_STATE x0state = MT_STATE(maxNodes, it->first);
        transTable[x0state.to_ulong()].push_back(MT_STATE(maxNodes, (it + timeStepDelay)->second));
    }    
    
    // entropy calculations
    MT_ENTROPIES entropies = toolset.entropies(transTable);
    
    // MIP
    //std::vector<MT_PARTITION> MIP = toolset.MIPs(entropies, ENM_TONONI_BALDUZZI);
    
    // Phi
    //double Phi = toolset.ei(MIP[0], ENM_NONE, entropies);
    
    // main complex
    std::pair<std::vector<std::vector<size_t> >,double> mainComplexes = toolset.mainComplexes(totalPartition(maxNodes),
                                                                                              entropies,
                                                                                              ENM_TONONI_BALDUZZI);    
    
    // Phi of main complex
    double ei_max(-1.0);
    int ei_max_mcI(-1);
    MT_PARTITION ei_max_P;
    for (unsigned int mcI = 0; mcI < mainComplexes.first.size(); mcI++) {
        std::vector<MT_PARTITION > MIPs = toolset.MIPs( mainComplexes.first[mcI],
                                                       entropies,
                                                       ENM_TONONI_BALDUZZI);
        
        for (unsigned int mipI = 0; mipI < MIPs.size(); mipI++) {
            double ei_temp = toolset.ei(mainComplexes.first[mcI],
                                        MIPs[mipI],
                                        ENM_NONE,
                                        entropies);
            if (ei_temp >  ei_max) {
                ei_max = ei_temp;
                ei_max_mcI = mcI;
                ei_max_P = MIPs[mipI];
            }
        }
    }
    
    double phiMC = ei_max;
    std::vector<size_t> thisMC = mainComplexes.first[ei_max_mcI];
    
    // write to file
    *m_analysisOutput << m_agent.m_id << "\t" << m_agent.m_fitness << "\t" << "\t" << phiMC << "\t";
    
    // 
    for (size_t i=0; i < ei_max_P.size(); ++i) {
        *m_analysisOutput << ei_max_P[i];
        if (i+1 < ei_max_P.size())
            *m_analysisOutput <<",";
    }
    
    *m_analysisOutput << "\t";
    
    // main complex nodes
    for (size_t i=0; i < thisMC.size(); ++i) {
        *m_analysisOutput << thisMC[i];
        if (i+1 < thisMC.size())
            *m_analysisOutput <<",";
    }
    *m_analysisOutput << "\t";
        
}



void nAnalyzer::calculateMutualInfo(unsigned long xMask, unsigned long yMask, unsigned int timeStepDelay){
    
    // construct probability table
    std::map<std::pair<unsigned long, unsigned long>, double> freqTable;
    std::map<unsigned long, double> numInputOccurrence;
    for (std::vector<std::pair<unsigned long, unsigned long> >::iterator it = m_agent.m_stateHistory.begin();
         it != m_agent.m_stateHistory.end() - timeStepDelay; it++) {
        //    frequencyTable
        unsigned long input = (xMask) ? (it->first&xMask) : it->first;
        unsigned long output = (yMask) ? ((it + timeStepDelay)->second&yMask) : (it + timeStepDelay)->second;
        freqTable[std::make_pair(input, output)]++;
        numInputOccurrence[input]++;
    }
    
    // normalize each input transition
    // and also calculate p(out)
    std::map<unsigned long, double> outProb;
    for (std::map<std::pair<unsigned long, unsigned long>, double>::iterator it = freqTable.begin();
         it != freqTable.end(); it++) {
        // p(y|x)
        it->second /= numInputOccurrence[(it->first).first];
        // p(y) = \sum p(in) * p(out | in)
        outProb[(it->first).second] += (numInputOccurrence[(it->first).first] / m_agent.m_stateHistory.size() ) * it->second; 
    }
    
    // mutual information
    double mutInfo(0.0);
    for (std::map<std::pair<unsigned long, unsigned long>, double>::iterator it = freqTable.begin();
         it != freqTable.end(); it++) {
        // p(x)
        double pX = numInputOccurrence[(it->first).first] / m_agent.m_stateHistory.size();
        // mutInfo = \sum p(x)*p(y|x) \log [ p(y|x) / p(y) ]
        mutInfo += pX * (it->second) * log( (it->second) / outProb[(it->first).second] ) / log(2);
    }
    
    *m_analysisOutput << mutInfo << "\t";
}
