//
//  nAgent.cpp
//  evoNik
//
//  Created by Nikhil Joshi on 2/13/12.
//  Copyright (c) 2012 California Institute of Technology. All rights reserved.
//

#include <iostream>
#include <cmath>

#include "nAgent.hpp"
#include "nAnalyzer.hpp"

unsigned int nAgent::masterID=0;



void nAgent::initialize(){
    m_alive = true;
    m_position.x = m_position.y = 0;
    m_prevPosition.x = m_prevPosition.y = 0;
    m_curState = m_prevState = 0;
    m_stateHistory.clear();
    m_maskedNode = -1;
    m_maskValue = 0;
    m_fitness = 0;
    m_fitnessEvalCount = 0;
    m_lineageSize = 0;
    
    // if genome present build HMM units
    if (!m_genome.m_genome.empty())
        this->buildHMMs();
    
    // add to lineage
    if (!m_addedToLineage){
        updateLineage(+1);
        m_addedToLineage = true;
    }
}

bool nAgent::isValid(){
    
    // check if it is alive
    if (m_alive == false)
        return false;
    
    // if the genome is empty
    if (m_genome.m_genome.empty())
        return false;
    
    // if HMM units are not built (build them)
    if (m_hmms.empty())
        buildHMMs();
    
    return true;
}

void nAgent::setupRandomGenome(unsigned int nucleotides){

    // resize genome
    m_genome.setSize(nucleotides);
    
    // fill in nucleotides
    for(size_t i = 0; i < nucleotides; i++)
        m_genome.setGene(rand()&(long)(pow(2, maxNodes)-1), (int)i); 
    
    // implant start codons 
    unsigned int numberOfHMMs = (unsigned int)genUniRand(1, maxNumHMMs);
    for (unsigned int i = 0; i < numberOfHMMs; i++) {
        // select a random position along the genome
        unsigned int j = (unsigned int)genUniRand(0, m_genome.getSize() - 5);
        // implant codon
        m_genome.setGene(startCode1, j);
        m_genome.setGene(startCode2, j+1);
    }
    
    // buildHMMs
    buildHMMs();
}


void nAgent::buildHMMs(){
    // clear previour record, if any
    m_hmms.clear();
    
    // run through the whole genome
    for (unsigned int i = 0; i < m_genome.getSize(); i++) {
        // check for a start-codon
        if (m_genome.getGene(i) == startCode1 &&
            m_genome.getGene(i+1) == startCode2) {
            m_hmms.push_back(nHMMUnit(m_genome,i));
        }
    }
}


void nAgent::printHMMUnits(std::ostream& fout){
    // if no HMMs built, create them
    if (!m_hmms.size())
        this->buildHMMs();
    
    // print the HMM units one by one    
    for (unsigned int i = 0; i < m_hmms.size(); i++) {
        fout << "Printing HMM Unit #" << i << std::endl;
        m_hmms[i].printUnit(fout);
    }
}


void nAgent::loadGenomeFromFile(std::fstream& genFile){
    
    // open the file for reading
    if (!genFile.is_open()){
        std::cerr << "Error in nAgent: genomeFile is not opened" << std::endl;
        exit(1);
    }

    // clear the genome first
    m_genome.m_genome.clear();
    
    std::string line;
    
    while (getline(genFile, line)) {

        // trim the line for any spaces and carries
        boost::trim_if(line, boost::is_any_of(" \t"));
        
        // split into separate strings
        std::vector<std::string> geneString;
        boost::split(geneString, line, boost::is_any_of("\t "));

        // if it was not a comment
        if (geneString.size() != 0 && geneString[0] != "#")
            for (std::vector<std::string>::iterator it = geneString.begin();
                 it != geneString.end(); it++)
                m_genome.m_genome.push_back((unsigned int)std::atoi(it->c_str()));
        
    }
    
    // update HMM units
    buildHMMs();
}


void nAgent::loadGenomeFromFile(std::fstream& genFile, unsigned int id){
    
    // open the file for reading
    if (!genFile.is_open()){
        std::cerr << "Error in nAgent: genomeFile is not opened" << std::endl;
        exit(1);
    }
    
    // clear the genome first
    m_genome.m_genome.clear();
    
    std::string line;
    
    // the correct genome
    bool genomeOfInterest = false;
    
    while (getline(genFile, line)) {
        
        // trim the line for any spaces and carries
        boost::trim_if(line, boost::is_any_of(" \t"));
        
        // split into separate strings
        std::vector<std::string> geneString;
        boost::split(geneString, line, boost::is_any_of("\t "));
        
        if (geneString.size() != 0){
            // if it is a comment
            if (geneString[0] == "#"){
                // if the last word is the given id
                if((unsigned int)std::atoi(geneString[geneString.size()-1].c_str()) == id)
                    genomeOfInterest = true;
            }
            else if (genomeOfInterest){            
                for (std::vector<std::string>::iterator it = geneString.begin();
                     it != geneString.end(); it++)
                    m_genome.m_genome.push_back((unsigned int)std::atoi(it->c_str()));
                
                genomeOfInterest = false;
            }
        }
    }

    // update HMM units
    buildHMMs();
}
    
        

void nAgent::setParents(nAgent &p1, nAgent &p2){
    m_parents.clear();
    m_parents.push_back(&p1);
    m_parents.push_back(&p2);
}


void nAgent::printParents(){
    std::cout << "Parents of agent # " << m_id 
    << " are:";
    for (std::vector<nAgent*>::iterator it = m_parents.begin();
         it != m_parents.end(); it++)
        std::cout << (*it)->m_id << ", ";
    std::cout << std::endl;
}


void nAgent::printGenome(std::ostream& fout){
    fout << "# Printing genome for agent no. " << m_id << std::endl;
    m_genome.printGenome(fout);
}


void nAgent::mutate(double rate){
    size_t numGenes = m_genome.getSize();
    for(size_t i = 0; i < numGenes; i++)
        if (genUniRand(0, 1) < rate) 
            m_genome.setGene((unsigned int) floor(genUniRand(0, 255)), (int)i);
    
    // update HMMs
    buildHMMs();
}


void nAgent::applyDeletion(double rate){
    size_t numGenes = m_genome.getSize();
    for(size_t i = 0; i < numGenes; i++)
        if (genUniRand(0, 1) < rate) 
            m_genome.deleteGene((int)i);
    
    // update HMMs 
    buildHMMs();
}

void nAgent::applyInsertion(double rate){
    size_t numGenes = m_genome.getSize();
    for(size_t i = 0; i < numGenes; i++)
        if (genUniRand(0, 1) < rate) 
            m_genome.insertGene((unsigned int) floor(genUniRand(0, 255)), (int)i);
    
    // update HMMs
    buildHMMs();
}


std::pair<nAgent, nAgent> nAgent::crossOver(nAgent &partner){
    // select a cross over point
    unsigned int crossOverLocation = (int)floor(genUniRand(1, m_genome.getSize()));
        
    // a new baby is borned
    std::pair<nAgent, nAgent> children = std::make_pair(nAgent(*this, partner), 
                                                               nAgent(*this, partner));
    
    // form the genome of the baby by crossing over with the partner
    children.first.m_genome.setSize(this->m_genome.getSize());
    children.second.m_genome.setSize(partner.m_genome.getSize());
    
    for (size_t i = 0; i < this->m_genome.getSize(); i++) {
        children.first.m_genome.setGene((i < crossOverLocation) ? 
                                        this->m_genome.getGene((int)i) : partner.m_genome.getGene((int)i), 
                                        (int)i);
        children.second.m_genome.setGene((i < crossOverLocation) ? 
                                         partner.m_genome.getGene((int)i) : this->m_genome.getGene((int)i), 
                                         (int)i);
        
    }
    
    
    // further cross-over? (with probability 0.01)
    // TBD
    
    // build HMM units
    children.first.buildHMMs();
    children.second.buildHMMs();
                                                        
    return children;
}


nAgent nAgent::inheriteViaMutation(){
    
    // child is borned
    nAgent child(*this);
    
    // setup genome from the parent
    child.m_genome = this->m_genome; 
    
    // mutate genome
    child.mutate();
   
    // build HMM units
    child.buildHMMs();
   
    return child;
}

void nAgent::updateBrain(){

    // apply mask, if any
    applyMask(m_curState, m_maskedNode, m_maskValue);
    applyMask(m_prevState, m_maskedNode, m_maskValue);
 
    // a temp state
    unsigned long tempState = m_prevState;
    
    // run all HMM units on the current brain state to get the next state
    for (std::vector<nHMMUnit>::iterator it = m_hmms.begin(); it != m_hmms.end(); it++) {
        // apply the effect of "this" HMM unit
        it->fire(tempState, m_curState);
        // apply mask, if any
        applyMask(m_curState, m_maskedNode, m_maskValue);
        
        // go for next hmm
        tempState = m_curState;        
    }
    
    // update brain state history (if no inclusion of environmental update)
   // if (!includeEnvUpdate)
    m_stateHistory.push_back(std::make_pair(m_prevState, m_curState));
}



void nAgent::printBrainStateHistory(std::ostream& fout, bool saveBinary){
    
    // if state history is empty
    if (m_stateHistory.empty()) {
        std::cerr << "Error in nAgent: an empty brain encountered!\n"
        << "can not print brain history" << std::endl;
        exit(1);
    }
        
    for (std::vector<std::pair<unsigned long, unsigned long> >::iterator it = m_stateHistory.begin();
         it != m_stateHistory.end(); it++) {
        if (saveBinary)
            fout << "(" << it->first << ")\t" 
            << binary(maxNodes, it->first) << "\t"
            << "(" << it->second << ")\t" 
            << binary(maxNodes, it->second) << std::endl;
        else
            fout << it->first << "\t" << it->second << std::endl;
    }
}



std::vector<std::pair<unsigned long, unsigned long> > nAgent::getBrainScan(){
    // note down the current brain state
    unsigned long tempCurBrainState = this->getBrainState();
    
    // brain scan
    std::vector<std::pair<unsigned long, unsigned long> > brainScan;
    
    // how many times one input state is to be invoked
    int invokeRepeats = (deterministicHMM) ? 1 : 10;
    
    // run through each possible input state
    for (int i = 0; i < (1 << maxNodes); i++) {
        // repeat it for 50 times
        for (int j = 0; j < invokeRepeats; j++) {

            m_prevState = m_curState = (unsigned long)i;
            this->updateBrain();
            brainScan.push_back(std::make_pair(m_prevState, m_curState));
        }
    }
    
    // restore the present brain state
    m_curState = tempCurBrainState;
    
    return brainScan;

}


void nAgent::printBrainScan(std::ostream& fout, bool saveBinary){
    std::vector<std::pair<unsigned long, unsigned long> > bScan = this->getBrainScan();
    
    for (std::vector<std::pair<unsigned long, unsigned long> >::iterator it = bScan.begin();
         it != bScan.end(); it++) {
        if (saveBinary)
            fout << binary(maxNodes, it->first) << "\t" << binary(maxNodes, it->second) << std::endl;
        else
            fout << it->first << "\t" << it->second << std::endl;
    }
}


void nAgent::setMask(int nodeNumber, bool maskValue){
    m_maskedNode = nodeNumber;
    m_maskValue = maskValue;
}


void nAgent::updateFitness(double newFitness, bool useGeometric){
    
    // update fitness evaluation counter
    m_fitnessEvalCount++;
    
    // use geometric mean ?
    if (useGeometric) 
        m_fitness = pow(newFitness*pow(m_fitness, m_fitnessEvalCount - 1) , 1.0/m_fitnessEvalCount);
    else
        // arithmatic mean
        m_fitness = (newFitness + (m_fitnessEvalCount - 1)*m_fitness) / m_fitnessEvalCount;
}


void nAgent::resetFitness(){
    m_fitness = 0;
    m_fitnessEvalCount = 0;
}


double nAgent::costOfComputation() const{
    
    std::stringstream ipred;
    
    nAnalyzer ana1(m_id, m_genome, m_fitness, ipred);
    ana1.setRequirements("ipred");
    ana1.m_agent.m_stateHistory = m_stateHistory;
    
    return std::atof(ipred.str().c_str())/maxNodes;
}


void nAgent::updateLineage(int changeFactor, std::vector<unsigned int> accountedAncestors){

    // update record for each parent
    if (m_parents.size() != 0) {
        if (m_id == 0)
            std::cout << "id = " << m_id << " and parent size = " << m_parents.size() << std::endl; 
        for (std::vector<nAgent*>::iterator it = m_parents.begin(); 
             it != m_parents.end(); it++) {
            
            // check if this was already accounted
            bool accounted = false;
            for (std::vector<unsigned int>::iterator it1 = accountedAncestors.begin();
                 it1 != accountedAncestors.end(); it1++) {
                if ((*it)->m_id == *it1) {
                    accounted = true;
                }
            }
            
            // if not accounted already proceed
            if (!accounted) {
                // first make it accounted
                accountedAncestors.push_back((*it)->m_id);
                // increment the parentlineageSize
                (*it)->m_lineageSize += changeFactor;
                // propagate down the lineage
                (*it)->updateLineage(changeFactor, accountedAncestors);
            }
        }
    }    
}


void nAgent::removeFromLineage(){
    if (m_lineageSize != 0){
        std::cerr << "Error in nAgent: can not remove agent # "
        << m_id << "... it has non-zero lineage!"
        << std::endl;
    }    
    else if (m_alive){
        std::cerr << "Error in nAgent: can not remove agent # "
        << m_id << " ... it is still alive!"
        << std::endl;
    }
    else {
        updateLineage(-1);
    }
}


void nAgent::analyze(bool useBrainScan, std::ostream* fout){
    
    
    std::vector<std::pair<unsigned long, unsigned long> > origStateHistory = m_stateHistory;

    if (useBrainScan)
        m_stateHistory.clear();
    
    
    // if the state history is empty
    if (m_stateHistory.size() == 0) {
        std::cerr << "Warning in nAgent::analyze(): No maze data available." << std::endl; 
        std::cerr << "I will use brainScan for analysis!" << std::endl;
        getBrainScan();
    }
        
    // initialize modularity toolset
    ModularityToolset toolset;
    
    // transition table
    MT_TRANSITION_TABLE transTable;
    
    for (std::vector<std::pair<unsigned long, unsigned long> >::iterator it = m_stateHistory.begin();
         it != m_stateHistory.end(); it++){
        
        MT_STATE x0state = MT_STATE(maxNodes, it->first);
        transTable[x0state.to_ulong()].push_back(MT_STATE(maxNodes, it->second));
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
    *fout << m_id << "\t" << m_fitness << "\t" << "\t" << phiMC << "\t";
    
    // 
    for (size_t i=0; i < ei_max_P.size(); ++i) {
        *fout << ei_max_P[i];
        if (i+1 < ei_max_P.size())
            *fout <<",";
    }
    
    *fout << "\t";
    
    // main complex nodes
    for (size_t i=0; i < thisMC.size(); ++i) {
        *fout << thisMC[i];
        if (i+1 < thisMC.size())
            *fout <<",";
    }
    *fout << std::endl;
    
    if (useBrainScan)
        m_stateHistory = origStateHistory;
    
}
