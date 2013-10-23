//
//  player.h
//  mazeSolver
//
//  Created by Nikhil Joshi on 12/3/12.
//  Copyright (c) 2012 Nikhil Joshi. All rights reserved.
//

#ifndef __mazeSolver__player__
#define __mazeSolver__player__

#include <iostream>
#include <fstream>
#include <cmath>

#include <boost/algorithm/string.hpp>
#include <boost/thread/barrier.hpp>

#include <SFML/Graphics.hpp>

#include "tileMap.hpp"
#include "hmm.hpp"

class player{
public:
    // Player poses
    // texture data file
    sf::Texture m_playerPosImage;
    // texture data (pos set)
    std::vector<sf::Sprite> m_poses;
    // tile size (size of the tile containing one player pose)
    unsigned int m_tileWidth, m_tileHeight;
    // current pose
    unsigned int m_curPose;
    
    // player trajectory list (as a function of increasing time)
    // trajecyory file
    const char* m_trajectoryFileName;
    // 2D trajectory data (in the maze)
    std::vector<std::pair<unsigned int, unsigned int> >m_positions;
    // a red dot rendition along the traversed path
    sf::Texture m_redDotImage;
    
    // trajectory parameters
    int m_timeIndex;
    int m_deltaT;
    
    // brain data
    const char* m_playerBrainDataFile;
    // data
    std::vector<unsigned int> m_prevBrainState, m_nextBrainState, m_sensors;
    std::vector<std::vector<int> > m_uio;
    std::vector<hmm> m_hmmList;
    
    // tilemap on which player lives
    tileMap* m_tilemap;
    
    // constructor
    player(const char* playerPosesFile, const char* playerTrajectoryFile,
           const char* playerBrainDataFile,
           unsigned int tileWidth = 39.5, unsigned int tileHeight = 39.5)
    : m_trajectoryFileName(playerTrajectoryFile),
    m_playerBrainDataFile(playerBrainDataFile),
    m_tileWidth(tileWidth),
    m_tileHeight(tileHeight){
        
        // import player poses
        m_playerPosImage.loadFromFile(playerPosesFile);
        m_playerPosImage.setSmooth(true);
        
        // set initial pose
        m_curPose = 5; // facing front
        
        // initial position index
        m_timeIndex = 0;
        m_deltaT = 0;
                
        // tile map
        m_tilemap = NULL;
        
        // brain texture needs creation
        m_brainCanvasCreated = false;
        
        
        // red dots along trajectory
        m_redDotImage.loadFromFile("/Users/nikhil/Desktop/work-temp/mazeSolver/work/images/redDotBig.png");
    }
    
    // default constructor
    player(){

    }
    
    // copy constructor
    player(const player& o){
        m_trajectoryFileName = o.m_trajectoryFileName;
        m_playerPosImage = o.m_playerPosImage;
        m_playerBrainDataFile = o.m_playerBrainDataFile;
        m_tileWidth = o.m_tileWidth;
        m_tileHeight = o.m_tileHeight;
        m_tilemap = o.m_tilemap;
        m_curPose = o.m_curPose;
        m_timeIndex = 0;
        m_deltaT = 0;
        
        m_brainCanvasCreated = false;
        
        m_redDotImage = o.m_redDotImage;
    }
    
    // assignment operator
    player operator = (const player o){
        m_trajectoryFileName = o.m_trajectoryFileName;
        m_playerPosImage = o.m_playerPosImage;
        m_playerBrainDataFile = o.m_playerBrainDataFile;
        m_tileWidth = o.m_tileWidth;
        m_tileHeight = o.m_tileHeight;
        m_tilemap = o.m_tilemap;
        m_curPose = o.m_curPose;
        m_timeIndex = 0;
        m_deltaT = 0;
        
        m_brainCanvasCreated = false;
        
        m_redDotImage = o.m_redDotImage;
        
        return *this;
    }
    
    // member functions
    // import player trajectory
    void loadTrajectory(void);
    // print trajectory
    void printTrajectory(std::ostream& fout = std::cout);
    // attach to a given tilemap
    void attachToTileMap(tileMap& t);
    // reset trajectory
    void resetTrajectory(void);
    // render trajectory
    void renderTrajectory(sf::RenderWindow& window, sf::FloatRect area = sf::FloatRect(), bool showOnlyDots = false);
    // set tile size
    void setTileSize(unsigned int width, unsigned int height);
    // build poses library
    void buildPoses(void);
    // show poses
    void showPoses(sf::RenderWindow& window);
    // advance player
    void updatePosition(void)                                   {     m_timeIndex += m_deltaT;   }
    // get the absolute position
    sf::Vector2f getPosition(sf::RenderWindow& window, sf::FloatRect area = sf::FloatRect());
    // populate player moves for the window area
    std::vector<sf::Sprite> populateMoves(sf::RenderWindow& window,
                                          sf::FloatRect area = sf::FloatRect(),
                                          bool showTrajectory = true);
    // clear all the brain
    void resetBrain(void);
    // load brain data
    void loadBrainData(void);
    // show brain
    void showBrain(sf::RenderWindow& window,
                   sf::FloatRect area = sf::FloatRect(),
                   bool detailedView = false);
    
private:
    // for brain rendering
    sf::RenderTexture m_brainCanvas;
    bool m_brainCanvasCreated;
    
};


#endif /* defined(__mazeSolver__player__) */
