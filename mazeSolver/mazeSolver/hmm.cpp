//
//  hmm.cpp
//  mazeSolver
//
//  Created by Nikhil Joshi on 12/8/12.
//  Copyright (c) 2012 Nikhil Joshi. All rights reserved.
//

#include "hmm.hpp"

// rendering on a Window

//sf::FloatRect hmm::renderUnit(sf::RenderWindow &window,
//                              std::vector<double> leftTop,
//                              bool detailedView,
//                              unsigned int inState,
//                              unsigned int outState){
//    // create a temp. texture
//    sf::RenderTexture tex;
//    tex.create(window.getSize().x, window.getSize().y);
//    
//    
//    sf::FloatRect returnVector = renderUnit(tex,
//                                            leftTop,
//                                            detailedView,
//                                            inState,
//                                            outState);
//    
//    sf::Sprite tempSprite(tex.getTexture());
//    window.draw(tempSprite);
//    
//    return returnVector;
//}



// rendering on a texture
sf::FloatRect hmm::renderUnit(sf::RenderTarget &tex,
                              std::vector<double> leftTop,
                              bool detailedView,
                              unsigned int inState,
                              unsigned int outState){
    
    // if new positions not given, use default
    leftTop[0] = (leftTop[0] < 30) ? 30 : leftTop[0];
    leftTop[1] = (leftTop[1] < 50) ? 50 : leftTop[1];
    
    // color convention
    sf::Color onState = sf::Color::Green;
    sf::Color onGoingTransition = sf::Color::Red;
    
    
    // buffer
    std::stringstream buffer;
    
    
    // print header on the window
    buffer << m_id;
    sf::Text header;
    header.setString("HMM unit # " + buffer.str());
    header.setFont(m_font);
    header.setColor(m_color);
    header.setCharacterSize(20);
    header.setStyle(sf::Text::Regular);
    header.setPosition(leftTop[0] + 40, leftTop[1]);
    tex.draw(header);
    
    
    // table
    table t(std::pow(2, (double)m_inputs.size()) + 4,     // rows are input states
            std::pow(2, (double)m_outputs.size()) + 4);   // columns are output states
    // +1 is for the unit number
    
    // top and left margin
    buffer.str("");
    buffer << m_id;
    t.addCell(0, 0, "U"+buffer.str(), 10, sf::Color(0,0,0,0));
    t.m_cells[0][0].setTextStyle(sf::Text::Style::Bold);
    
    // bottom and right margin
    buffer.str("");
    buffer << m_id;
    t.addCell((int)t.m_cells.size() - 1, (int)t.m_cells[0].size() -1, "U "+buffer.str(), 10, sf::Color(0,0,0,0));
    t.m_cells[(int)t.m_cells.size() - 1][(int)t.m_cells[0].size() -1].setTextStyle(sf::Text::Style::Bold);
    
    
    // input states
    // extract the inputState corresponding to "this" HMM unit inputs
    unsigned int inputState(0);
    for (std::vector<unsigned int>::reverse_iterator it = m_inputs.rbegin();
         it != m_inputs.rend(); it++)
        inputState = (inputState << 1) + ((inState >> *it)&1);
    
    for (int i = 0; i < std::pow(2, (double)m_inputs.size()); i++) {
        buffer.str("");
        buffer << boost::dynamic_bitset<unsigned int>(m_inputs.size(), i);
        t.addCell(i+2, 1, buffer.str());
        t.m_cells[i+2][1].setTextColor(m_color);
        // is this state the 'currently active' one?
        if (i == inputState) {
            t.m_cells[i+2][1].setTextColor(onState);
            t.m_cells[i+2][1].setTextStyle(sf::Text::Style::Bold);
        }
    }
    
    // output states
    // extract the inputState corresponding to "this" HMM unit inputs
    unsigned int outputState(0);
    for (std::vector<unsigned int>::reverse_iterator it = m_outputs.rbegin();
         it != m_outputs.rend(); it++)
        outputState = (outputState << 1) + ((outState >> *it)&1);
    
    for (int i = 0; i < std::pow(2, (double)m_outputs.size()); i++) {
        buffer.str("");
        buffer << boost::dynamic_bitset<unsigned int>(m_outputs.size(), i);
        t.addCell(1, i+2, buffer.str());
        t.m_cells[1][i+2].setTextColor(m_color);
        t.m_cells[1][i+2].setTextAngle(-30);
        // is this state the 'currently active' one?
        if (i == outputState) {
            t.m_cells[1][i+2].setTextColor(onState);
            t.m_cells[1][i+2].setTextStyle(sf::Text::Style::Bold);
        }
    }
    
    // transition probabilities
    for (int i = 0; i < m_transTable.size(); i++) {
        for (int j = 0; j < m_transTable[i].size(); j++) {
            buffer.str("");
            buffer << std::setfill('0') << std::setw(2) <<  m_transTable[i][j];
            t.addCell(2 + i, 2 + j, buffer.str());
            t.m_cells[2+i][2+j].setTextColor(sf::Color::Black);
            // if this is the 'current transition'
            if (i == inputState && j == outputState) {
                t.m_cells[i+2][j+2].setBackgroundColor(onGoingTransition);
                t.m_cells[i+2][j+2].setTextColor(sf::Color::Yellow);
            }
            else {
                t.m_cells[i+2][j+2].setBackgroundColor((double)m_transTable[i][j] / 100.0);
                if (m_transTable[i][j] < 40)
                    t.m_cells[i+2][j+2].setTextColor(sf::Color::White);
            }
        }
    }
    
    
    // if no detailed activity is to be shown
    if (!detailedView) {
        for (int i = 0; i < t.m_cells.size(); i++) {
            for (int j = 0; j < t.m_cells[i].size(); j++) {
                t.m_cells[i][j].setTextColor(sf::Color::Transparent);
                t.m_cells[i][j].setBackgroundColor(sf::Color::Transparent);
            }
        }
    }
    
    // render the table
    t.setBoundaryColor(m_color);
    t.setBoundaryThickness(2);
    
    t.render(tex, sf::Vector2f(leftTop[0] + 40, leftTop[1] + 40));
    
    // draw inputs
    for (int i = 0; i < m_inputs.size(); i++) {
        // input line
        sf::RectangleShape line(sf::Vector2f(30, 3));
        line.setPosition(t.m_outline.getGlobalBounds().left - 30, t.m_outline.getGlobalBounds().top + 15 + i*40);
        // if this input was active (set to 1)
        if (inState & (1 << m_inputs[(m_inputs.size() - 1) - i]))
            line.setFillColor(sf::Color::Green);
        else
            line.setFillColor(m_color);
        tex.draw(line);
        
        // input name
        buffer.str("");
        buffer << m_inputs[(m_inputs.size() - 1) - i];
        header.setString(buffer.str());
        header.setOrigin(header.getGlobalBounds().width, 0);   // for right justified text
        header.setPosition(t.m_outline.getGlobalBounds().left - 40, t.m_outline.getGlobalBounds().top + 5 + i*40);
        tex.draw(header);
    }
    
    
    // draw outputs
    for (int i = 0; i < m_outputs.size(); i++) {
        // output line
        sf::RectangleShape line(sf::Vector2f(30, 3));
        line.setPosition(t.m_outline.getGlobalBounds().left + t.m_outline.getGlobalBounds().width,
                         t.m_outline.getGlobalBounds().top + 15 + i*40);
        // if this bit was active (set to 1)
        if (outState & (1 << m_outputs[(m_outputs.size() - 1) - i]))
            line.setFillColor(sf::Color::Green);
        else
            line.setFillColor(m_color);
        tex.draw(line);
        
        // output name
        buffer.str("");
        buffer << m_outputs[(m_outputs.size() - 1) - i];
        header.setString(buffer.str());
        header.setOrigin(0, 0);
        header.setPosition(t.m_outline.getGlobalBounds().left + t.m_outline.getGlobalBounds().width + 40,
                           t.m_outline.getGlobalBounds().top + 5 + i*40);
        tex.draw(header);
        
    }
    
    
    // returning corners (for the HMM unit printing)
    return sf::FloatRect(leftTop[0], leftTop[1],
                         t.m_outline.getGlobalBounds().width + 150,
                         t.m_outline.getGlobalBounds().height + 100);
    
    
}