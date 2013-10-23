//
//  nGenome.cpp
//  evoNik
//
//  Created by Nikhil Joshi on 3/15/12.
//  Copyright (c) 2012 California Institute of Technology. All rights reserved.
//

#include <iostream>

#include "nGenome.hpp"


unsigned int nGenome::getGene(unsigned int position){
    return m_genome[position % m_genome.size()];
}


bool nGenome::setGene(unsigned int g, unsigned int position){
    m_genome[position % m_genome.size()] = g;    
    return true;
}

bool nGenome::deleteGene(unsigned int position){
    m_genome.erase(m_genome.begin() + (position % m_genome.size()));
    return true;
}

bool nGenome::insertGene(unsigned int g, unsigned int position){
    m_genome.insert(m_genome.begin() + (position % m_genome.size()), g);
    return true;
}

void nGenome::printGenome(std::ostream& fout){
    for(std::vector<unsigned int>::iterator it = m_genome.begin(); 
        it != m_genome.end(); it++)
        fout << (int)*it << "\t";
    fout << std::endl;
}

