//
//  utility.hpp
//  evoNik
//
//  Created by Nikhil Joshi on 2/14/12.
//  Copyright (c) 2012 California Institute of Technology. All rights reserved.
//

#ifndef evoNik_utility_hpp
#define evoNik_utility_hpp

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>

#include <ctime>
#include <string>
#include <fstream>
#include <cmath>
#include <sstream>

#include <boost/program_options.hpp>
#include <boost/thread.hpp>
#include <boost/format.hpp>
#include <boost/bind.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/iterator/indirect_iterator.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/assign/std/vector.hpp>
#include <boost/random.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/copy.hpp>




#include "constants.hpp"
#include "nDijkstra.hpp"

namespace fs = boost::filesystem;
namespace pt = boost::posix_time;
namespace po = boost::program_options;
namespace io = boost::iostreams;


// binary variables
#include <boost/dynamic_bitset.hpp>
typedef boost::dynamic_bitset<unsigned long> binary;


// Position in 2D geometry
struct position{
    int x, y;
    
    position()
    { x = y = 0; }
    
    position(int p, int q){
        x = p;
        y = q;
    }
    
    ~position()
    { }
    
    // comparison operator
    bool operator == (const position& o){
        return (x == o.x && y == o.y);
    }
    
    // member functions
    void print(std::ostream& fout = std::cout){
        fout << "(" << x << ", " << y <<")" << std::endl;
    }
    
    int getX(void) { return x; }
    int getY(void) { return y; }
    
};


inline void init(void){
    srand((unsigned int)time(NULL));
    rand();

    if (suppressMessages){
        std::cout << "Warning: Message suppression is set to \"true\" " << std::endl;
        std::cout << "Warning: All messages will be suppressed henceforth!" << std::endl;
        std::cout << "Warning: This will not affect messaging Errors, if any" << std::endl;
    }
}


inline std::string dateTimeStamp(bool fractional =true) {
    std::string result;
    pt::ptime t(boost::posix_time::microsec_clock::universal_time());
    
    boost::gregorian::date::date_type date = t.date();
    pt::time_duration time_of_day = t.time_of_day();
    result = (boost::format("%1$d_%2$02d_%3$02d_%4$02d_%5$02d_%6$02d") % date.year() % date.month() % date.day()
              % time_of_day.hours()  % time_of_day.minutes() % time_of_day.seconds()).str();
    if (fractional) {
        result += "_";
        result += boost::lexical_cast<std::string>(time_of_day.fractional_seconds());
    }
    return result;
}


inline double genUniRand(double a, double b){       
    return a + (b-a)*((double)rand() / (double)RAND_MAX);
}


inline void applyMask(unsigned long& state, int bitPosition, bool zeroOrOne){
    // if mask value to 1
    if (zeroOrOne)
        state |= (1 << bitPosition );
    else // if value to 0
        state &= ~(1 << bitPosition);
}

inline void applyBit(unsigned long& state, int bitPosition, bool zeroOrOne){
    applyMask(state, bitPosition, zeroOrOne);
}

inline void applyBit(unsigned int& state, int bitPosition, bool zeroOrOne){
    // if mask value to 1
    if (zeroOrOne)
        state |= (1 << bitPosition );
    else // if value to 0
        state &= ~(1 << bitPosition);
}

inline int hammingDistance(position pos1, position pos2){
    
    return (int) (std::abs((long)pos1.x - pos2.x) + std::abs((long)pos1.y - pos2.y));
    
}

template<typename T>
inline std::string compressIt(T s){
   
    std::stringstream uncompressed, compressed;
    uncompressed << s;
    
    io::filtering_streambuf<io::output> o;
    o.push(io::gzip_compressor());
    o.push(uncompressed);
    io::copy(o, compressed);
    
    return compressed.str();
}

template<typename T>
inline std::string compressIt(std::vector<T> s){
    
    std::stringstream uncompressed, compressed;
    for (typename std::vector<T>::iterator it = s.begin();
         it != s.end(); it++)
        uncompressed << *it;
        
    io::filtering_streambuf<io::input> o;
    o.push(io::gzip_compressor());
    o.push(uncompressed);
    io::copy(o, compressed);
    
    return compressed.str();
}

#endif
