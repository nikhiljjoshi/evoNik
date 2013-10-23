//
//  hmm.h
//  mazeSolver
//
//  Created by Nikhil Joshi on 12/8/12.
//  Copyright (c) 2012 Nikhil Joshi. All rights reserved.
//

#ifndef __mazeSolver__hmm__
#define __mazeSolver__hmm__

#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <cmath>

#include <boost/dynamic_bitset.hpp>

#include <SFML/Graphics.hpp>

#include "table.hpp"

class hmm{
public:
    // id
    unsigned int m_id;
    // inputs
    std::vector<unsigned int> m_inputs;
    // outputs
    std::vector<unsigned int> m_outputs;
    // transition probabilities (unnormalized)
    std::vector<std::vector<unsigned int> > m_transTable;
    
    // fonts
    sf::Font m_font;
    // color (in which HMM is displayed)
    sf::Color m_color;
    
    
    // constructor
    hmm(unsigned int id,
        std::vector<unsigned int> inputs,
        std::vector<unsigned int> outputs,
        std::vector<std::vector<unsigned int> > transTable,
        std::vector<double> leftTop = std::vector<double>())
    :m_id(id), m_inputs(inputs), m_outputs(outputs), m_transTable(transTable){
        
        // set fonts
        m_font.loadFromFile("/Library/Fonts/Arial.ttf");
        
        // color
        m_color = sf::Color(230, 170, 0);
    }
    
    
    // copy constructor
    hmm(const hmm& o){
        m_id = o.m_id;
        m_inputs = o.m_inputs;
        m_outputs = o.m_outputs;
        m_transTable = o.m_transTable;
        m_font = o.m_font;
        m_color = o.m_color;
    }
    
    // assignment operator
    hmm& operator = (const hmm o){
        m_id = o.m_id;
        m_inputs = o.m_inputs;
        m_outputs = o.m_outputs;
        m_transTable = o.m_transTable;
        m_font = o.m_font;
        m_color = o.m_color;
    
        return *this;
    }
    
    
    // rank operator (for sorting)
    bool operator < (const hmm& o) const{
        return (m_id < o.m_id);
    }

    
    // member functions
//    // render the unit on a given window and return end corners
//    sf::FloatRect renderUnit(sf::RenderWindow& window,
//                             std::vector<double> leftTop = std::vector<double>(),
//                             bool withActivity = true,
//                             unsigned int inState = std::pow(2, 12.0),
//                             unsigned int outState = std::pow(2, 12.0));
    
    // render the unit on given window and return end corners
    sf::FloatRect renderUnit(sf::RenderTarget& tex,
                             std::vector<double> leftTop = std::vector<double>(),
                             bool withActivity = true,
                             unsigned int inState = std::pow(2, 12.0),
                             unsigned int outState = std::pow(2, 12.0));
    
};


#endif /* defined(__mazeSolver__hmm__) */
