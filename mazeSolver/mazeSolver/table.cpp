//
//  table.cpp
//  mazeSolver
//
//  Created by Nikhil Joshi on 12/11/12.
//  Copyright (c) 2012 Nikhil Joshi. All rights reserved.
//

#include "table.hpp"


void cell::setText(std::string text){
    m_content.setString(text);

    // resize the bounding box accordingly
    resizeBoundingBox(sf::Vector2f(m_content.getGlobalBounds().width*1.1,
                                   m_content.getGlobalBounds().height*1.1));
}


void cell::setTextAngle(double angle){
    m_content.setRotation(angle);
    
    // resize the bounding box accordingly
    resizeBoundingBox(sf::Vector2f(m_content.getGlobalBounds().width*1.1,
                                   m_content.getGlobalBounds().height*1.1));

}

void cell::setTextColor(sf::Color color){
    m_content.setColor(color);

    // resize the bounding box accordingly
    resizeBoundingBox(sf::Vector2f(m_content.getGlobalBounds().width*1.1,
                                   m_content.getGlobalBounds().height*1.1));
}


void cell::setTextSize(double size){
    m_content.setCharacterSize(size);
    
    // resize the bounding box accordingly
    resizeBoundingBox(sf::Vector2f(m_content.getGlobalBounds().width*1.1,
                                   m_content.getGlobalBounds().height*1.1));
}

void cell::setTextStyle(sf::Text::Style textStyle){
    m_content.setStyle(textStyle);
    
    // resize the bounding box accordingly
    resizeBoundingBox(sf::Vector2f(m_content.getGlobalBounds().width*1.1,
                                   m_content.getGlobalBounds().height*1.1));
}



void cell::resizeBoundingBox(sf::Vector2f size){    
    m_boundingBox.setSize(size);
}


//sf::FloatRect cell::render(sf::RenderWindow &window, sf::Vector2f topLeft){
// 
//    sf::RenderTexture tempTex;
//    
//    sf::FloatRect returnRect = render(tempTex, topLeft);
//    
//    sf::Sprite tempSprite(tempTex.getTexture());
//    window.draw(tempSprite);
//    
//    return returnRect;
//}



sf::FloatRect cell::render(sf::RenderTarget &target, sf::Vector2f topLeft){
    
    // draw the bounding box first
    m_boundingBox.setPosition(topLeft);
    target.draw(m_boundingBox);
    
    // set origins of text to the center of the bounding box
    m_content.setOrigin(m_content.getLocalBounds().width/2, m_content.getLocalBounds().height/2*1.4);
    m_content.setPosition(m_boundingBox.getGlobalBounds().left + m_boundingBox.getGlobalBounds().width/2,
                          m_boundingBox.getGlobalBounds().top + m_boundingBox.getGlobalBounds().height/2);
    target.draw(m_content);
    
    // return the rect
    return m_boundingBox.getGlobalBounds();
}


void table::addCell(unsigned int row, unsigned int col,
                    std::string content, double textSize, sf::Color textColor, double textAngle,
                    sf::Color bkgColor, sf::Color borderColor, double borderThickness){
    
    if (row >= m_cells.size()) {
        std::cerr << "Error: row index exceeds table size" << std::endl;
        exit(1);
    }
    else if (col >= m_cells[row].size()) {
        std::cerr << "Error: column index exceeds allowed table limits" << std::endl;
        exit(1);
    }
    
    m_cells[row][col] = cell(content, textSize, textColor, textAngle);
    m_cells[row][col].setBackgroundColor(bkgColor);
    m_cells[row][col].setBorderColor(borderColor);
    m_cells[row][col].setBorderThickness(borderThickness);
        
}



//void table::render(sf::RenderWindow& window, sf::Vector2f position){
//    
//    if (window.isOpen()) {
//        
//        sf::RenderTexture tempTex;
//        tempTex.create(window.getSize().x, window.getSize().y);
//        
//        render(tempTex, position);
//        
//        sf::Sprite tempSprite(tempTex.getTexture());
//        window.draw(tempSprite);
//    }
//}



void table::render(sf::RenderTarget& target, sf::Vector2f position){
    
    // first determine maximum width for each row
    std::vector<double> width, height;
    
    height.resize(m_cells.size());
    width.resize(m_cells[0].size());
    
    // iterate over each row
    for (int i = 0; i < m_cells.size(); i++) {
        // iterate over each column
        for (int j = 0; j < m_cells[i].size(); j++) {
            // if height for this row is smaller than the next entry, adjust
            if (height[i] < m_cells[i][j].m_boundingBox.getSize().y)
                height[i] = m_cells[i][j].m_boundingBox.getSize().y;
            
            // update width for each column if the cell in THAT column is wider
            if (width[j] < m_cells[i][j].m_boundingBox.getSize().x)
                width[j] = m_cells[i][j].m_boundingBox.getSize().x;
        }
    }
    
    // resize the rows and columns to accommodate largest cells
    for (int i = 0; i < m_cells.size(); i++)
        for (int j = 0; j < m_cells[i].size(); j++)
            m_cells[i][j].resizeBoundingBox(sf::Vector2f(width[j], height[i]));
    
    // start rendering
    double tableWidth(0.0), tableHeight(0.0);
    for (int i = 0; i < width.size(); i++)
        tableWidth += width[i];
    for (int i = 0; i < height.size(); i++)
        tableHeight += height[i];
    

    // an enclosing bounding box
    m_outline.setSize(sf::Vector2f(tableWidth,
                                   tableHeight));
    m_outline.setPosition(position);
    m_outline.setOutlineColor(m_boundaryColor);
    m_outline.setOutlineThickness(m_boundaryThickness);
    m_outline.setFillColor(m_fillColor);
    target.draw(m_outline);
    
    
    
    // start rendering
    double renderAtY(position.y);
    for (int i = 0; i < m_cells.size(); i++) {  // each row
        double renderAtX(position.x);
        for (int j = 0; j < m_cells[i].size(); j++) {
            m_cells[i][j].render(target, sf::Vector2f(renderAtX, renderAtY));
            renderAtX += width[j];
        }
        renderAtY += height[i];
    }
    

}