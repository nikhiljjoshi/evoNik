//
//  nPopulation.cpp
//  evoNik
//
//  Created by Nikhil Joshi on 4/12/12.
//  Copyright (c) 2012 California Institute of Technology. All rights reserved.
//

#include <iostream>

#include "nPopulation.hpp"

unsigned int nPopulation::generationID = 0;

void nPopulation::rank(void){
    
    if (m_id > static_cast<unsigned int>((selectionPressureFromGeneration/100.0)*maxGenerations) &&
        m_id < static_cast<unsigned int>((selectionPressureUpToGeneration/100.0)*maxGenerations)) {
      
        // ascending order in fitness
        std::sort(m_members.begin(), m_members.end(), compareFitnesses());
        // reverse: best fit guy gets index 0 and so on
        std::reverse(m_members.begin(), m_members.end());
    }
    // set as ranked
    m_ranked = true;
}

void nPopulation::printPopulation(std::ostream& fout){
    fout << "# Generation no. " << m_id << std::endl;
    
    fout << "# Member IDs are: ";
    for (std::vector<nAgent*>::iterator it = m_members.begin(); 
         it != m_members.end(); it++)
        fout << (*it)->m_id << "(" << (*it)->m_fitness << ") ";
    fout << std::endl;
}

double nPopulation::getAverageFitness(){
    double ave(0.0);
    
    for (std::vector<nAgent*>::iterator it = m_members.begin();
         it != m_members.end(); it++)
        ave += (*it)->m_fitness;
    
    return (m_members.size()==0) ? 0 : ave / m_members.size();
    
}


double nPopulation::getMaxFitness(){
    // rank the generation
    rank();
    
    //return the maximum fitness
    return m_members.front()->m_fitness;
}

double nPopulation::getMinFitness(){
    // rank the generation
    rank();
    
    // return the minimum fitness
    return m_members.back()->m_fitness;
}

nAgent* nPopulation::getElite(){
    rank();
    return m_members[0];
}


void nPopulation::populate(unsigned int size){

    rank();
    
    // if there was "a" master agent
    if (m_members.size()!= 0) {
        while (m_members.size() < size) {
            nAgent* temp = new nAgent(m_members[0]->inheriteViaMutation());
            m_members.push_back(temp);
        }
    } else {
        while (m_members.size() < size) {
            nAgent* temp = new nAgent;
            temp->setupRandomGenome();
            m_members.push_back(temp);
        }
    }

    // rank the reorganized population
    rank();
}


void nPopulation::addAgent(nAgent& a){
    m_members.push_back(&a);
    m_ranked = false;
    rank();
}


void nPopulation::removeAgent(nAgent& a){
    
    for (std::vector<nAgent*>::iterator it = m_members.begin();
         it != m_members.end(); it++)
        if( (*it)->m_id == a.m_id ){
            a.updateLineage(-1);
            delete *it; 
            m_members.erase(it);
            it--;
        }
}


void nPopulation::manageSize(){
    
    // get the population ranked first
    rank();
    
    // if the size is bigger
    if (m_members.size() > populationSize)
        std::for_each(boost::make_indirect_iterator(m_members.begin() + populationSize), 
                      boost::make_indirect_iterator(m_members.end()), 
                      boost::bind(&nPopulation::removeAgent, boost::ref(*this), _1));
    // if the size is less 
    else if(m_members.size() < populationSize)
        populate();
    
    // rank after reshaping
    rank();
}

    
void nPopulation::evaluate(nGame& game){
    
    m_game = &game;

    // check the evaluation game is set correctly
    if (!m_game->isValid()){
        std::cerr << "Error in nPopulation: can not evaluate generation. invalid game!"
        << std::endl;
        exit(1);
    }
    
    // evaluate fitness for each individual
    for (std::vector<nAgent*>::iterator it = m_members.begin();
         it != m_members.end(); it++) {
        m_game->updatePlayer(*(*it));
        // evaluate a number of times to reduce evaluation error
        for (unsigned int i = 0; i < evaluationRepetition; i++)
            m_game->execute();
    }
    
    // rank the population
    rank();
}


nPopulation nPopulation::reproduce(){
    
    // rank this population
    rank();
    
    // make an empty population
    nPopulation newPop;
    
    // if elitism is allowed (and selection pressure is to be applied)
    if (m_id > static_cast<unsigned int>((selectionPressureFromGeneration/100.0)*maxGenerations) &&
        m_id < static_cast<unsigned int>((selectionPressureUpToGeneration/100.0)*maxGenerations) &&
        elitism){

        // a new child from the elite mother
        nAgent* eliteChild = new nAgent(*m_members[0]);
        // elite child will have the exact genome of its mother
        eliteChild->m_genome = m_members[0]->m_genome;
        // build HMM units
        eliteChild->buildHMMs();
        // insert in the new population
        newPop.addAgent(*eliteChild);
    }

    
    // Roulette selection range
    double fitnessSummed(0.0);
    for (std::vector<nAgent*>::iterator it = m_members.begin(); 
         it != m_members.end(); it++) {
        // consider only those agents with fitness more than
        // a fratction of the max fitness in the generation
        if ((*it)->m_fitness >= purgeFraction*m_members[0]->m_fitness)
            fitnessSummed += (*it)->m_fitness;
    }

    while (newPop.m_members.size() < populationSize) {
        
        std::vector<int> parent;
        
        // select two parents (by Roulette selection)
        for (int i=0; i < 2; i++) {
            double total(m_members[0]->m_fitness);
            unsigned int j(0);
        
            // if selection pressure is present
            if (m_id > static_cast<unsigned int>((selectionPressureFromGeneration/100.0)*maxGenerations) &&
                m_id < static_cast<unsigned int>((selectionPressureUpToGeneration/100.0)*maxGenerations) ) {
                
                // generate a number (drop a niddle) between 0, sum[row]
                double dropper = genUniRand(0, fitnessSummed);
                
                while (dropper > total){
                    j++;
                    total += m_members[j]->m_fitness;
                }
            }
            
            else // no selection occurs, just select two parent randomly
                j = static_cast<unsigned int>(genUniRand(0, populationSize));
            
            parent.push_back(j);
        }
        
        // if crossOver is allowed 
        if (crossOverOn) {
            
            // cross over parent genomes to make two kids
            
            std::pair<nAgent, nAgent> kids = m_members[parent[0]]->crossOver(*m_members[parent[1]]);
            
            nAgent* child1 = new nAgent(kids.first);
            nAgent* child2 = new nAgent(kids.second);
        
            child1->mutate(); 
            child2->mutate();
            
            child1->applyDeletion();
            child2->applyDeletion();
            
            child1->applyInsertion();
            child2->applyInsertion();
           
            child1->buildHMMs();
            child2->buildHMMs();
            
            newPop.addAgent(*child1);
            newPop.addAgent(*child2);
        }
        // if not by crossOver, generate via mutation from both the parents
        else{
            nAgent* child1 = new nAgent(m_members[parent[0]]->inheriteViaMutation());
            nAgent* child2 = new nAgent(m_members[parent[1]]->inheriteViaMutation());
            
            child1->applyDeletion();
            child2->applyDeletion();
            
            child1->applyInsertion();
            child2->applyInsertion();
            
            child1->buildHMMs();
            child2->buildHMMs();
            
            newPop.addAgent(*child1);
            newPop.addAgent(*child2);
        }
    }
        
    // retire the old population
    for (std::vector<nAgent*>::iterator it = m_members.begin();
         it != m_members.end(); it++) {
        (*it)->retire();
    }
    
    // bring the new population in size
    newPop.manageSize();
    
    // clean the "impotent" lineage
    this->cleanLineage();
    
    // set parent population
    newPop.m_parentPopulation = this;
    
    // set child LOD output stream to parent's
    newPop.m_lodOutput = this->m_lodOutput;
    
    // set child analysis output stream to parent's
    newPop.m_analysisOutput = this->m_analysisOutput;
    
    return newPop;
}


void nPopulation::cleanLineage(){
            
    // remove those "dead" agents, which do not have any progeny left
    for (std::vector<nAgent*>::iterator it = m_members.begin();
         it != m_members.end(); it++)
        if ( (!(*it)->m_alive) && ((*it)->m_lineageSize == 0) ){
            removeAgent(*(*it));
            it--;
        }
    
    // propagate this down the evolution
    if (m_parentPopulation != NULL){
        m_parentPopulation->cleanLineage();
        
        // if the previous (cleaned up to now) generation 
        // has only one member left
        if(m_parentPopulation->m_members.size() == 1){
        
            // remove pointers from "this" generation to that "single" parent
            for (std::vector<nAgent*>::iterator it = m_members.begin();
                 it != m_members.end(); it++)
                (*it)->m_parents.clear();
            
            // store the "single" parent to the LOD file
            *m_lodOutput << "# Gen no. " << m_parentPopulation->m_id 
            << " (agent fitness = " << m_parentPopulation->m_members.back()->m_fitness 
            << "):" << std::endl;
            m_parentPopulation->m_members.back()->printGenome(*m_lodOutput);
            
            // perform knockout analysis for the "single" parent
            nAgent* origPlayer = m_game->m_player;
            m_parentPopulation->m_members.back()->m_alive = true;
            m_game->updatePlayer(*m_parentPopulation->m_members.back());
            *m_game->m_knockoutOutput << "# Gen no. " << m_parentPopulation->m_id << ":" << std::endl;
            m_game->profilePlayerKnockout();
            m_parentPopulation->m_members.back()->m_alive = false;
            m_game->updatePlayer(*origPlayer);
            
            // perform Phi, etc analysis after an interval
            if (m_parentPopulation->m_id % analysisInterval == 0){
                *m_analysisOutput << m_parentPopulation->m_id << "\t";
                nAnalyzer analyzer(m_parentPopulation->m_members.back()->m_id,
                                   m_parentPopulation->m_members.back()->m_genome, 
                                   m_parentPopulation->m_members.back()->m_fitness,
                                   *m_analysisOutput);
                analyzer.setRequirements("all");
                analyzer.run();
                //                m_parentPopulation->m_members.back()->analyze(false, m_analysisOutput);
                // boost::thread analysisThread(&nAnalyzer::analyze, &analyzer, false);
               // analysisThread.join();
            }
            
            // remove the "single" parent from the generation
            m_parentPopulation->removeAgent(*m_parentPopulation->m_members.back());
            // "this" generation does not point to any parent population counter 
            m_parentPopulation = NULL;
        }
    }
}

void nPopulation::setLODoutputStream(std::ostream& output){ 
    // this LOD output stream
    m_lodOutput = &output;
    
    // propagate down the parent
    if (m_parentPopulation != NULL) 
        m_parentPopulation->m_lodOutput = m_lodOutput;
}

void nPopulation::setAnalysisOutputStream(std::ostream& output){ 
    // this analysis output stream
    m_analysisOutput = &output;
    
    // propagate down the parent
    if (m_parentPopulation != NULL) 
        m_parentPopulation->setAnalysisOutputStream(*m_analysisOutput);
}
