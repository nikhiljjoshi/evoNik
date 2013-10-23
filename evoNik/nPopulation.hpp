//
//  nPopulation.hpp
//  evoNik
//
//  Created by Nikhil Joshi on 4/12/12.
//  Copyright (c) 2012 California Institute of Technology. All rights reserved.
//

#ifndef evoNik_nPopulation_hpp
#define evoNik_nPopulation_hpp

#include "constants.hpp"
#include "nAgent.hpp"
#include "nGame.hpp"
#include "nAnalyzer.hpp"

class nPopulation{
public:
    // master id 
    static unsigned int generationID;
    
    // id for this generation
    unsigned int m_id;
    // parent population
    nPopulation* m_parentPopulation;
    // members of population
    std::vector<nAgent*> m_members;
    // whether ranked (according to ascending fitness)
    bool m_ranked;
    // game for evaluation and knockout profiling of the population
    nGame* m_game;
     
    // LOD data output stream
    std::ostream* m_lodOutput;
    // analysis data output stream
    std::ostream* m_analysisOutput;
    
    // constructor for empty population
    nPopulation(): 
    m_id(generationID++), 
    m_lodOutput(&std::cout),
    m_analysisOutput(&std::cout){
        m_parentPopulation = NULL;
        m_ranked = false;
    }

    // constructor with a "master" agent
    nPopulation(nAgent& a):
    m_id(generationID++), 
    m_lodOutput(&std::cout),
    m_analysisOutput(&std::cout){
        m_parentPopulation = NULL;
        m_members.push_back(&a);
        populate();  
    }
    
    // constructor with a list of agents
    nPopulation(std::vector<nAgent*> members): 
    m_id(generationID++),
    m_members(members),
    m_lodOutput(&std::cout),
    m_analysisOutput(&std::cout){
        m_parentPopulation = NULL;
        m_ranked = false;
    }
    
    // copy constructor
    nPopulation(const nPopulation &o):
    m_id(o.m_id), 
    m_parentPopulation(o.m_parentPopulation),
    m_members(o.m_members),
    m_ranked(o.m_ranked),
    m_lodOutput(o.m_lodOutput),
    m_analysisOutput(o.m_analysisOutput){         
    
    }

    // assignment operator
    nPopulation& operator = (const nPopulation o){
        m_id = o.m_id;
        m_parentPopulation = o.m_parentPopulation;
        m_members = o.m_members;
        m_ranked = o.m_ranked;
        m_lodOutput = o.m_lodOutput;
        m_analysisOutput = o.m_analysisOutput;
        return *this;
    }
    
    // destructor
    ~nPopulation(){
        m_members.clear();
    }
    
    // member functions
    // get generation id
    unsigned int getGenerationID(void)                   { return m_id; }
    // set parent population
    void setParentPopulation(nPopulation& pop)           { m_parentPopulation = &pop; }
    // populate
    void populate(unsigned int size = populationSize);
    // add a given agent in the population
    void addAgent(nAgent& a);
    // remove a given agent from the population
    void removeAgent(nAgent& a);
    // bring size to the limit set
    void manageSize(void);
    // generate the next generation
    nPopulation reproduce(void);
    // retire
    bool retire(void);
    // evaluate
    void evaluate(nGame& game);
    // rank the individuals according to fitnesses
    void rank(void);
    // get the individual with highest fitness
    nAgent* getElite(void);
    // get average fitness of the generation
    double getAverageFitness(void);
    // get maximum (elite) fitness
    double getMaxFitness(void);
    // get minimum fitness
    double getMinFitness(void);
    // print population (agent id's)
    void printPopulation(std::ostream& fout = std::cout);
    // clean the lineage
    void cleanLineage();
    // set lod data stream
    void setLODoutputStream(std::ostream& output); 
    // set analysis data stream
    void setAnalysisOutputStream(std::ostream& output);
    
    
private:
    // for ranking fitnesses (by pointers to agents)
    struct compareFitnesses{
        bool operator ()(const nAgent* a1, const nAgent* a2){
            return *a1 < *a2;
        }
    };
    
};

#endif
