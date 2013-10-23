//
//  genome.hpp
//  evoNik
//
//  Created by Nikhil Joshi on 3/15/12.
//  Copyright (c) 2012 California Institute of Technology. All rights reserved.
//

#ifndef evoNik_nGenome_hpp
#define evoNik_nGenome_hpp

#include <vector>

#include "utility.hpp"

class nGenome{
public:
    std::vector<unsigned int> m_genome;
    
    
    // default constructor
    nGenome(){
    }

    // copy constructor
    nGenome( const nGenome& o): m_genome(o.m_genome){
    }
    
    // assignment operator
    nGenome& operator = (const nGenome& o){
        m_genome = o.m_genome;
        return *this;
    }
    
    // destructor
    ~nGenome(){
        m_genome.clear();
    }
    
    
    // member functions
    // set size (resize) of the genome
    void setSize(size_t s)               { m_genome.resize(s); }
    // get the size of the genome
    size_t getSize(void)                 { return m_genome.size(); }
    // get the value (gene) at the location
    unsigned int getGene(unsigned int);
    // set the value (gene) at the location
    bool setGene(unsigned int, unsigned int);
    // delete the gene at the location
    bool deleteGene(unsigned int);
    // insert a gene at the location
    bool insertGene(unsigned int, unsigned int);
    // print out genome
    void printGenome(std::ostream& fout = std::cout);

    
};


#endif
