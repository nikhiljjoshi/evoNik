//
//  constants.h
//  evoNik
//
//  Created by Nikhil Joshi on 3/13/12.
//  Copyright (c) 2012 California Institute of Technology. All rights reserved.
//

#ifndef evoNik_constants_h
#define evoNik_constants_h


//// Population parameters
// size
const unsigned int populationSize = 300;
// Elitism (the best candidate carryover)
const bool elitism = true;
// purge fraction (for biasing selection towards the fittest guys)
const double purgeFraction = 0.0;


//// Brain network parameters 
const unsigned int maxNodes = 12;


//// Genetics factors 
// genome length
const unsigned int genomeLength = 100;
// maximum number of chromosomes
const unsigned int maxNumHMMs = 10;
// if crossOver should be allowed
const bool crossOverOn = false;
// mutation rate
const double mutationRate = 0.5 / genomeLength;
// deletion rate
const double deletionRate = 0.05 / genomeLength;
// insertion rate
const double insertionRate = 0.05 / genomeLength;


//// Genome properties
// start codon : (42, 255-42)
// 42 is the ultimate answer to everything and universe from Hitchhiker's galaxy guide
const unsigned int startCode1 = 42;
const unsigned int startCode2 = 255 - 42;


//// HMM unit parameters
// deterministic or stochastic
const bool deterministicHMM = true;
// max input size
const unsigned int HMMFanIn = 4;               // be some power of 2 (i.e. 2^n)
// max output size
const unsigned int HMMFanOut = 4;              // be some power of 2 (i.e. 2^n)

//// Game parameters
// number of generations in a trial
const unsigned int maxGenerations = 10000;
// selection pressure up to (in percent of total generations)
const double selectionPressureUpToGeneration = 100.0;
// selectionPressure from (in percent of total generations)
const double selectionPressureFromGeneration = 0.0;
// fitness evaluation time
const unsigned int evaluationTime = 200;
// evaluation repetitions
const unsigned int evaluationRepetition = 15;
// use of geometric mean
const bool useGeometricMean = false;
// does gravity exist
const bool gravityPresent = false;
// is food required
const bool huntForFood = false;
// agent time deficit (in percent)
const double agentDeficit = 0.1;



//// Phi, etc analysis
// analysis interval
const unsigned int analysisInterval = 200;
// use brain scan instead of maze data
const bool useBrainScan = false;
// analyze including environmental update
const bool includeEnvUpdate = true;
// calculate over timesteps
const unsigned int calculateOverTimeDelays = 0;


//// Output messages
// cout messages/information
const bool suppressMessages = false;

#endif
