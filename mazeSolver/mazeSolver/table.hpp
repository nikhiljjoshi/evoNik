//
//  table.hpp
//  mazeSolver
//
//  Created by Nikhil Joshi on 12/11/12.
//  Copyright (c) 2012 Nikhil Joshi. All rights reserved.
//

#ifndef mazeSolver_table_hpp
#define mazeSolver_table_hpp

#include <iostream>
#include <string>
#include <cmath>

#include <SFML/Graphics.hpp>

class cell {
public:
    //content
    sf::Text m_content;
    // bounding box
    sf::RectangleShape m_boundingBox;
        
    // constructor
    cell(std::string content, double textSize = 10,
         sf::Color textColor = sf::Color::Blue,
         double textAngle = 0,
         sf::Text::Style textStyle = sf::Text::Style::Regular){
        // content
        setText(content);
        // text size
        setTextSize(textSize);
        // text color
        setTextColor(textColor);
        // text angle
        setTextAngle(textAngle);
        // text style
        setTextStyle(textStyle);
                
        // default background color (nill)
        m_boundingBox.setFillColor(sf::Color(0,0,0,0));
        // default border color
        m_boundingBox.setOutlineColor(sf::Color::Black);
        // default border thickness
        m_boundingBox.setOutlineThickness(2);
    }

    // default constructor
    cell(){
        m_boundingBox.setFillColor(sf::Color(0,0,0,0));
        m_boundingBox.setOutlineThickness(0);
        
    }
    
    // copy constructor
    cell(const cell& o){
        m_content = o.m_content;
        m_boundingBox = o.m_boundingBox;
    }
    
    // assignment operator
    cell& operator = (const cell o){
        m_content = o.m_content;
        m_boundingBox = o.m_boundingBox;
        
        return *this;
    }
    

    // member function
    void setText(std::string text);
    void setTextAngle(double angle);
    void setTextColor(sf::Color color);
    void setTextSize(double size);
    void setTextStyle(sf::Text::Style textStyle);
    
    void setBorderColor(sf::Color color)                              {     m_boundingBox.setOutlineColor(color);   }
    void setBackgroundColor(sf::Color color)                          {     m_boundingBox.setFillColor(color);      }
    void setBackgroundColor(double grayLevel)                         {     m_boundingBox.setFillColor(sf::Color(std::floor(255*grayLevel),
                                                                                                                 std::floor(255*grayLevel),
                                                                                                                 std::floor(255*grayLevel), 150));  }
    void setBorderThickness(double thickness)                         {     m_boundingBox.setOutlineThickness(thickness);   }
    
    
    // resize the bounding box
    void resizeBoundingBox(sf::Vector2f size);
    
    // render the cell
//    sf::FloatRect render(sf::RenderWindow& window, sf::Vector2f position);
    sf::FloatRect render(sf::RenderTarget& target, sf::Vector2f position);

};


class table {
public:
    // cells
    std::vector<std::vector<cell> > m_cells;
    // overall background
    sf::Color m_fillColor;
    // outline
    sf::RectangleShape m_outline;
    // outline thickness
    double m_boundaryThickness;
    // outline color
    sf::Color m_boundaryColor;
    
    // constructor
    table(unsigned int numRow, unsigned int numColumn){
        m_cells.resize(numRow);
        for (int i = 0; i < numRow; i++)
            m_cells[i].resize(numColumn);
        
        // defaults
        m_fillColor = sf::Color(0,0,0,0);
        m_boundaryThickness = 0;
        m_boundaryColor = sf::Color(0,0,0,0);
        
    }
    
    // member functions
    void setBackgroundColor(sf::Color color)                            { m_fillColor = color;  }
    void setBoundaryThickness(double thickness)                         { m_boundaryThickness = thickness; }
    void setBoundaryColor(sf::Color color)                              { m_boundaryColor = color; }
    
    void addCell(unsigned int row, unsigned int col,
                 std::string content, double textSize = 10, sf::Color textColor = sf::Color::Blue, double textAngle = 0,
                 sf::Color bkgColor = sf::Color(0,0,0,0), sf::Color borderColor = sf::Color(0,0,0,0), double borderThickness = 0);
    
//    void render(sf::RenderWindow& window, sf::Vector2f position);
    void render(sf::RenderTarget& target, sf::Vector2f position);
};

#endif
