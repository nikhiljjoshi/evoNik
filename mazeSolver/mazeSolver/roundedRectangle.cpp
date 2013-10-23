//
//  roundedRectangle.cpp
//  mazeSolver
//
//  Created by Nikhil Joshi on 12/31/12.
//  Copyright (c) 2012 Nikhil Joshi. All rights reserved.
//

#include "roundedRectangle.hpp"


roundedRectangle::roundedRectangle(sf::Vector2f size, double cornerRadius)
: m_size(size), m_cornerRadius(cornerRadius) {
    
    // corner arc
    std::vector<std::pair<double, double>> arc;
    
    for (int i = 0; i <= cornerPrecision; i++) {
        
        // sample x, and y for a circle x^2 + y^2 = r^2
        double x = m_cornerRadius * ((double)i / cornerPrecision);
        
        arc.push_back( std::make_pair(x,
                                      std::sqrt(m_cornerRadius*m_cornerRadius - x*x)) );
        
    }
    
    // populate left top corner
    for (auto it = arc.begin(); it != arc.end(); it++)
        // add to the array of points
        m_points.push_back(sf::Vector2f(m_cornerRadius - it->first ,
                                        m_cornerRadius - it->second));
    
    // populate left bottom
    for (auto it = arc.rbegin(); it != arc.rend(); it++)
        m_points.push_back(sf::Vector2f(m_cornerRadius - it->first,
                                        m_size.y - (m_cornerRadius - it->second)) );

    
    // populate right bottom
    for (auto it = arc.begin(); it != arc.end(); it++)
        m_points.push_back(sf::Vector2f(m_size.x - (m_cornerRadius - it->first),
                                        m_size.y - (m_cornerRadius - it->second)) );
    
    
    // populate right top
    for (auto it = arc.rbegin(); it != arc.rend(); it++)
        m_points.push_back(sf::Vector2f(m_size.x - (m_cornerRadius - it->first),
                                        m_cornerRadius - it->second));
 
    // update the shape
    update();
    
}


void roundedRectangle::setPointCount(unsigned int count){
    m_points.resize(count);
    update();
}


unsigned int roundedRectangle::getPointCount() const{
    return static_cast<unsigned int>(m_points.size());
}

void roundedRectangle::setPoint(unsigned int index, const sf::Vector2f &point){
    m_points.push_back(point);
    update();
}


sf::Vector2f roundedRectangle::getPoint(unsigned int index) const{
    return m_points[index];
}