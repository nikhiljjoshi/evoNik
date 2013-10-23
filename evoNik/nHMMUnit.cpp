//
//  nHMM.cpp
//  evoNik
//
//  Created by Nikhil Joshi on 3/14/12.
//  Copyright (c) 2012 California Institute of Technology. All rights reserved.
//

#include "nHMMUnit.hpp"

void nHMMUnit::setup(nGenome genome, unsigned int start){
    // confirm start codon
    assert(genome.getGene(start) == startCode1);
    assert(genome.getGene(start+1) == startCode2);
    
    // read position (after start codon)
    int read = start + 2;
    
    // number of inputs 
    unsigned int numInputs = 1 + (unsigned int)(genome.getGene(read++) & (HMMFanIn - 1));
    // number of outputs
    unsigned int numOutputs = 1 + (unsigned int)(genome.getGene(read++) & (HMMFanOut - 1));
        
    // clear input and output vectors
    m_inputs.clear();
    m_outputs.clear();
    
    // fill in input nodes
    for (size_t i = 0; i < numInputs; i++){
        // next input
        unsigned int nextInput = genome.getGene(read + (int)i) % (maxNodes);
        // avoid repetition
        while (std::find(m_inputs.begin(), m_inputs.end(), nextInput) != m_inputs.end()){
            nextInput = (nextInput + 1) % maxNodes;
        }
        m_inputs.push_back(nextInput);
    }
    
    // set read position after input nodes list on genome
    read += HMMFanIn;
    
    // fill in output nodes
    for (size_t i = 0; i < numOutputs; i++){
        
        // next output
        unsigned int nextOutput = genome.getGene(read+(int)i) % (maxNodes);
                
        // avoid repetition
        while (std::find(m_outputs.begin(), m_outputs.end(), nextOutput) != m_outputs.end()){
            nextOutput = (nextOutput + 1) % (maxNodes);
        }        
        m_outputs.push_back(nextOutput);
    }
    
    // set read position after output nodes list on genome
    read += HMMFanOut;
    
    // setup transition probabilities
    // input states constitute rows
    m_hmm.resize(1 << numInputs);
    // sum of each row (for normalization)
    m_sums.resize(1 << numInputs);
    // output states are columns
    for (size_t i = 0; i < (size_t)(1 << numInputs); i++)
        m_hmm[i].resize(1 << numOutputs);
    // transition probabilities
    for (size_t row = 0; row < m_hmm.size(); row++) {
        // for each row set column entries (avoid zero as a probability)
        for (size_t col = 0; col < m_hmm[row].size(); col++){
            m_hmm[row][col] = (unsigned int) genome.getGene((int)(read + (1 << numOutputs)*row + col));
            
            m_sums[row] += m_hmm[row][col];
        }
        
        // if (rare event of) all entries in a row are identically zero
        if (m_sums[row] == 0) {
            m_hmm[row][0] = 1;
            m_sums[row] = 1;
        }
        
        // if deterministic, winner takes it all
        if (deterministicHMM) {
            size_t counter = std::max_element(m_hmm[row].begin(), m_hmm[row].end()) - m_hmm[row].begin();
            m_hmm[row].clear();
            m_hmm[row].resize(1 << numOutputs);
            m_hmm[row][counter] = 1;
            m_sums[row] = 1;
        }
    }
    
    
}

void nHMMUnit::fire(unsigned long &inState, unsigned long &outState){
    
    // input state corresponding to inputs of "this" unit
    int inputState = 0;
    
    // extract the state corresponding to inputs (which identifies the row)
    for (size_t i = 0; i < m_inputs.size(); i++)
        inputState = (inputState << 1) + ((inState >> m_inputs[i])&1);
 
    // generate a number (drop a niddle) between 0, sum[row]
    unsigned long dropper = (deterministicHMM)? 1 : (unsigned long)genUniRand(0, m_sums[inputState]);
    
    unsigned long total(m_hmm[inputState][0]), j(0);
    while (dropper > total)
        total += m_hmm[inputState][++j];
    
    outState = inState;
    
    for (unsigned int i=0; i < m_outputs.size(); i++) {
        (((j >> i)&1) == true) ? (outState |= 1 << m_outputs[i]) : ( outState &= ~(1 << m_outputs[i]));
    }
}

void nHMMUnit::printUnit(std::ostream& fout){

    size_t numInputs(m_inputs.size()), numOutputs(m_outputs.size());
    fout << "Inputs (" << numInputs << "): ";
    for (size_t i = 0; i < numInputs; i++)
        fout << (int)m_inputs[i] << "\t";
    fout << std::endl;
    
    fout << "Outputs (" << numOutputs << "): ";
    for (size_t i = 0; i < numOutputs; i++)
        fout << (int)m_outputs[(numOutputs-1) - i] << "\t";
    fout << std::endl;

    fout << "The probability table:\n" 
    << "(rows: input states | columns: output states)" 
    << std::endl;
    
    for (size_t row = 0; row < m_hmm.size(); row++){
        
        fout << "(" << row << ")" << "\t";
        for (size_t col = 0; col < m_hmm[row].size(); col++)
            fout << (double)m_hmm[row][col] / m_sums[row] << "\t";
        fout << std::endl;
    }
}
