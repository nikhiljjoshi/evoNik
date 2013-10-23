//
//  main.cpp
//  evoNik
//
//  Created by Nikhil Joshi on 2/13/12.
//  Copyright (c) 2012 California Institute of Technology. All rights reserved.
//

#include "utility.hpp"
#include "nRun.hpp"
#include "nAnalyzer.hpp"

int main (int argc, char* argv[]){
    
    if (argc < 3) {
        std::cerr << "Error in main: Specify experiment name and run number" << std::endl;
        std::cerr << "usage ./evoNik [EXP_NAME_STRING] [RUN_INDEX]" <<std::endl;
        std::cerr << "e.g. ./evoNik test 0" << std::endl;
        exit(0);
    }
    
    init();
    
    nRun testRun(argv[1], atoi(argv[2]));
    testRun.go();
    testRun.close();
    
    return 0;
}

