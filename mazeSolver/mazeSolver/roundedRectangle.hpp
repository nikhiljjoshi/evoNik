//
//  roundedRectangle.hpp
//  mazeSolver
//
//  Created by Nikhil Joshi on 12/31/12.
//  Copyright (c) 2012 Nikhil Joshi. All rights reserved.
//

#ifndef mazeSolver_roundedRectangle_hpp
#define mazeSolver_roundedRectangle_hpp

#include <cmath>
#include <vector>

#include <SFML/Graphics/Shape.hpp>

# define cornerPrecision 60

class SFML_GRAPHICS_API roundedRectangle : public sf::Shape {
public:
    // constructor
    explicit roundedRectangle(sf::Vector2f size, double cornerRadius = 0);
    
    
    // member functions
    void setPointCount(unsigned int count);
    virtual unsigned int getPointCount(void) const;
    void setPoint(unsigned int index, const sf::Vector2f& point);
    virtual sf::Vector2f getPoint(unsigned int index) const;
    
private:
    // size
    sf::Vector2f m_size;
    // corner radius
    double m_cornerRadius;
    // vertices
    std::vector<sf::Vector2f> m_points;
    
    // create data points
    void create(void);
  
    
};


#endif
