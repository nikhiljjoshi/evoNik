//
//  run.hpp
//  evoNik
//
//  Created by Nikhil Joshi on 4/20/12.
//  Copyright (c) 2012 California Institute of Technology. All rights reserved.
//

#ifndef evoNik_run_hpp
#define evoNik_run_hpp

#include <string>

#include "utility.hpp"
#include "nPopulation.hpp"


class nRun{
public:
    // run name
    std::string m_runName;
    // this run id
    unsigned int m_id;
    // working directory
    fs::path m_thisRunDirectory;
    // run data storage directory
    fs::path m_dataDirectory;
    // LOD, knockout, analysis data and evolution progress files
    std::fstream m_lodFile, m_knockoutFile, m_analysisFile, m_progressFile, m_parameterFile;
    
    
    // constructor with id (for multiple runs)
    nRun(std::string runName, unsigned int id = 0)
    :m_runName(runName), m_id(id){
        this->init();
    
    }
    
    //destructor
    ~nRun(){
        
    }
    
    // member functions
    // initialize the run
    void init(void);
    // start processing
    void go(void);
    // finish LOD for the "best" guy
    void dumpRemainingLODandKnockout(unsigned int genID, nAgent& a, nGame& g);
    // close the run
    void close(void);
    
};


#endif
