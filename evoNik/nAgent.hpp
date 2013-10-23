//
//  nAgent.hpp
//  evoNik
//
//  Created by Nikhil Joshi on 2/13/12.
//  Copyright (c) 2012 California Institute of Technology. All rights reserved.
//

#ifndef evoNik_nAgent_hpp
#define evoNik_nAgent_hpp

#include <vector>
#include <cmath>

#include "utility.hpp"
#include "nGenome.hpp"
#include "nHMMUnit.hpp"
#include "ModularityToolset/ModularityToolset.h"

// for computational costs
//#include "nAnalyzer.hpp"
class nAnalyzer;

class nAgent {
public:
    // master agent identification number
    static unsigned int masterID;
    
    /* evolution */
    // id of the agent
    unsigned int m_id;
    // parents of this agent
    std::vector<nAgent*> m_parents;
    // genome of this agent
    nGenome m_genome;
    // corresponding HMM units
    std::vector<nHMMUnit> m_hmms;    
    // alive or dead
    bool m_alive;
    // fitness
    double m_fitness;
    // fitness evaluation count
    unsigned int m_fitnessEvalCount;
    // size of lineage following "this" agent
    unsigned int m_lineageSize;
    // added to lineage
    bool m_addedToLineage;

    /* brain */
    // states
    unsigned long m_curState, m_prevState;
    // history
    std::vector<std::pair<unsigned long, unsigned long> > m_stateHistory;
    // masked node (for knockout analysis)
    int m_maskedNode;
    // masked to value
    bool m_maskValue;
    
    /* ergonomics */
    position m_position, m_prevPosition;
    // history
    std::vector<position> m_trajectory;
    
    // constructor
    nAgent():m_id(masterID++){
      //  m_parents.clear();
        m_addedToLineage = false;
        this->initialize();
    }
    
    // constructor with given id
    nAgent(unsigned int id)
    : m_id(id){
   //     m_parents.clear();
        m_addedToLineage = false;
        this->initialize();
    }
    
    // constructor with parents
    nAgent(std::vector<nAgent*> parents)
    : m_id(masterID++), m_parents(parents){
        m_addedToLineage = false;
        this->initialize();
    }
    
    // constructor with parents v2
    nAgent(nAgent &parent1, nAgent &parent2)
    : m_id(masterID++){
        m_parents.push_back(&parent1);
        m_parents.push_back(&parent2);
        m_addedToLineage = false;
        this->initialize();
    }
    
    // constructor with a single Mother
    nAgent(nAgent& singleMother)
    : m_id(masterID++){
        m_parents.push_back(&singleMother);
        m_addedToLineage = false;
        this->initialize();
    }
    
    // copy constructor
    nAgent(const nAgent &o)
    : m_id(o.m_id), m_parents(o.m_parents), m_genome(o.m_genome){ 
        m_addedToLineage = o.m_addedToLineage;
        this->initialize();
    }
    
    // assignment operator
    nAgent& operator =(const nAgent o){
        m_id = o.m_id;
        m_parents = o.m_parents;
        m_genome = o.m_genome;
        m_addedToLineage = o.m_addedToLineage;
        this->initialize();
        return *this;
    }
 
    // rank operator (for fitness ranking in population)
    bool operator < (const nAgent& o) const{
        return (m_fitness < o.m_fitness);
    }
    
    // destructor
    ~nAgent()
    { }
    
    // member functions
    // initialize 
    void initialize(void);
    // is it a valid agent?
    bool isValid(void);
    // setup a random genome
    void setupRandomGenome(unsigned int nucleotides = genomeLength);
    // setup the HMM units (from its genome)
    void buildHMMs(void);
    // load genome from a file
    void loadGenomeFromFile(std::fstream& genFile);
    // load genome with given id from a file
    void loadGenomeFromFile(std::fstream& genFile, unsigned int id);
    // set parents
    void setParents(nAgent &parent1, nAgent &parent2);
    // get parents (by their IDs)
    std::vector<nAgent*> getParents()                                                  { return m_parents; }
    // print parent by IDs
    void printParents(void);
    // update the lineage
    void updateLineage(int changeFactor, std::vector<unsigned int> accountedAncestors = std::vector<unsigned int>() );
    // inherite (via cross-over) from given parents
    std::pair<nAgent, nAgent> crossOver(nAgent &partner);
    // inherite (via mutation) from the given parent
    nAgent inheriteViaMutation(void);
    // mutate (with given mutation rate)
    void mutate(double rate = mutationRate);
    // apply deletion
    void applyDeletion(double rate = deletionRate);
    // apply insertion
    void applyInsertion(double rate = insertionRate);
    // retire the agent (it dies)
    void retire(void)                                                                  { m_alive = false;  }
    // declare as extinct (without any descendent)
    void removeFromLineage(void);
    // print genome of the agent
    void printGenome(std::ostream& fout = std::cout);
    // print associated HMM units
    void printHMMUnits(std::ostream& fout = std::cout);
    // reset brain
    void resetBrain(void)                                                              {  m_curState = 0; m_stateHistory.clear(); }
    // update brain state
    void updateBrain();
    // get the current brain state
    unsigned long getBrainState(void)                                                  { return m_curState; }
    // set brain state
    void setBrainState(unsigned long state)                                            { m_curState = state; }                      
    // get brain state history, so far
    std::vector<std::pair<unsigned long, unsigned long> > getBrainHistory(void)        { return m_stateHistory; }
    // print brain state history
    void printBrainStateHistory(std::ostream& fout = std::cout, bool saveBinary = false);
    // get EEG scan for the brain
    std::vector<std::pair<unsigned long, unsigned long> > getBrainScan(void);
    void printBrainScan(std::ostream& fout = std::cout, bool saveBinary = false);
    // set mask (for knockout)
    void setMask(int nodeNumber, bool maskValue);
    // calculate computational cost
    double costOfComputation(void) const;
    // update fitness
    void updateFitness(double newFitness, bool useGeometric = false);
    // reset fitness
    void resetFitness(void);
    // perform analysis
    void analyze(bool scanBrain = useBrainScan, std::ostream* fout = &std::cout);
    
};

#endif
