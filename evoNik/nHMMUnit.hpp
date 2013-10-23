//
//  file     : nHmm.hpp
//  project  : evoNik
//
//  Created by Nikhil Joshi on 3/14/12.
//  Copyright (c) 2012 California Institute of Technology. All rights reserved.
//
//  Short Description :
//  A class handling the Hidden Markov Model (HMM) units.
//

#ifndef evoNik_nHmmUnit_hpp
#define evoNik_nHmmUnit_hpp

#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <algorithm>

#include "utility.hpp"
#include "nGenome.hpp"

class nHMMUnit {
public:    
    // a 2-D table for transitions:
    std::vector<std::vector<unsigned int> > m_hmm;
    // sum of each row
    std::vector<unsigned int> m_sums;
    // inputs
    std::vector<unsigned int> m_inputs;
    // outputs
    std::vector<unsigned int> m_outputs;
    
    
    // constructor
    nHMMUnit(nGenome g, unsigned int position){
        this->setup(g, position);
    }
    
    // Default constructor
    nHMMUnit(){
    }
    // destructor
    ~nHMMUnit(){
        m_hmm.clear();
        m_sums.clear();
        m_inputs.clear();
        m_outputs.clear();
    }
    
    // member functions
    // setup the HMM using information encoded in a genome
    void setup(nGenome genome, unsigned int start);
    // fire an HMM to generate output from input states
    void fire(unsigned long &inState, unsigned long &outState);
    // print the HMM
    void printUnit(std::ostream& fout = std::cout);
    
};


#endif
