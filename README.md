# evoNik

Evolve small brain-like networks which can perform interesting tasks

## Synopsis

This is an implementation of an evolutionary algorithm to evolve small-sized, 
brain-like, binary networks that evolve to exhibit a desired functionality. The 
code, as is, will run an evolution using a 12-node or vertex network to solve 
a two dimensional maze with an entry (exit) door on the left (right) and a 
number of obstructing walls in between with one door each. The agent has five 
sensors three at the front and two on each side, through which it can sense a 
wall in an immediate maze-tile. At each door (and only while in the door) of 
a wall the agent is informed of the direction of the next door. Agent can move 
front or sideway using two motion actuators. The remaining four nodes are 
available to the agent to perform any internal computation, including memory.

The agent has to evolve correct wiring/connections among these binary units to 
solve the maze in shortest time, i.e. without digressing from the shortest 
path connecting entry and exit doors.

## References

More details at www.tinyurl.com/nikhiljjoshi/fun.html (smartness is complex)

Expected outcomes are similar to 

http://www.youtube.com/watch?v=ZQhC8Dxjz88 ,or

http://www.youtube.com/watch?v=C7UFgx0GTjU
 

## Packages

1. evoNik - runs the evolution for configuration set in constants.hpp

2. mazeSolver - builds animation for an instance of maze solving for a 
given maze, and a given agent


## Dependancies

1. boost c++ library functionality

2. additionary mazeSolver animation requires sfml c++ library



## Usage:
./evonik [EXPERIMENT NAME STRING] [RUN NUMBER STRING]

./mazeSolver --help   (for all options)


## Parameters

in constants.hpp file


## TO DO
1. Move class variables from public to private and add get/setters
1. Pass parameters runtime using boost::program_options in evoNik
