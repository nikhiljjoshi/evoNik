//
//  nFood.hpp
//  evoNik
//
//  Created by Nikhil Joshi on 5/1/12.
//  Copyright (c) 2012 California Institute of Technology. All rights reserved.
//

#ifndef evoNik_nFood_hpp
#define evoNik_nFood_hpp

#include "utility.hpp"

class nFood{
public:
    unsigned int m_calories;
    position m_position;
    bool m_consumed;
    
    // constructor
    nFood():
    m_calories(0),
    m_position(){
        m_consumed = false;
        
    }
    
    nFood(position pos):
    m_position(pos){
        m_calories = 0;
        m_consumed = false;
    }
    
    nFood(unsigned int calories, position pos):
    m_calories(calories),
    m_position(pos){
        m_consumed = false;
        
    }
    
    // copy constructor
    nFood(const nFood& o):
    m_calories(o.m_calories),
    m_position(o.m_position),
    m_consumed(o.m_consumed){
    }
    
    // assignment operator
    nFood& operator = (const nFood& o){
        m_calories = o.m_calories;
        m_position = o.m_position;
        m_consumed = o.m_consumed;
        return *this;
    }
    
    
    // rank operator (for fitness ranking in population)
    bool operator < (const nFood& o) const{
        return (m_position.x < o.m_position.x);
    }

    
    // member functions
    // set position
    void setPosition(position pos)               { m_position = pos;  }
    // set x position
    void setX(unsigned int x)                    { m_position.x = x;  }
    // set y position
    void setY(unsigned int y)                    { m_position.y = y;  }
    // set caloriefic value
    void setCalories(unsigned int cal)           { m_calories = cal;  }
    // get position
    position getPosition(void)                   { return m_position; }
    // get Calories
    unsigned int getCalories(void)               { return m_calories; }
    // eat the food
    void consume(void)                           { m_consumed = true; }
    

};

#endif
