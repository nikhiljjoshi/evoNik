//
//  showGame.h
//  mazeSolver
//
//  Created by Nikhil Joshi on 12/1/12.
//  Copyright (c) 2012 Nikhil Joshi. All rights reserved.
//

#ifndef __mazeSolver__showGame__
#define __mazeSolver__showGame__

#include <iostream>
#include <string>
#include <sstream>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>
#include <boost/thread/barrier.hpp>

#include <SFML/Graphics.hpp>

#include "roundedRectangle.hpp"
#include "tileMap.hpp"
#include "player.hpp"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

// utility to save the screenshot
void snapIt(sf::RenderWindow& window, std::string fileName);

class game{
public:
    // tile and player image files
    fs::path m_tileSetFile,
    m_playerPosFile;
    // (this) run data files
    fs::path m_mazeDataFile,
    m_brainDataFile,
    m_trajectoryDataFile;
    // background music file
    fs::path m_musicFile;
    // tile of the game
    std::string m_title;
    
    // tile map
    tileMap m_tilemap;
    // player
    player m_player;
    
    // screen shots filePrefix
    std::string m_screenShotsPrefix;
    
    // parameters
    bool m_showBrain;                    // brain is to be shown
    bool m_detailedView;                 // detailed brain view
    bool m_showTrajectory;               // player trajectory to be shown in main window
    unsigned int m_playerSpeed;          // speed of player motion
    bool m_showMovie;                    // movie to be shown
    bool m_takeScreenShots;              // save window screen-shots
    
    
    // constructor
    game(int argc, const char* argv[]){
        
        // set parameters
        m_showBrain = false;
        m_detailedView = false;
        m_showTrajectory = false;
        m_playerSpeed = 100;
        m_showMovie = false;
        m_takeScreenShots = false;

        // check and configure
        if (configure(argc, argv)){
            setTileMap();
            setPlayer();
        }
        else {
            std::cerr << "Game configuration failed!" << std::endl;
            exit(1);
        }
    }
    

    // member functions
    // check validity
    bool configure(int argc, const char* argv[]);
    // set tile map
    void setTileMap(void);
    // get tile map
    tileMap& getTileMap(void)                                      {   return m_tilemap;  }
    // set player
    void setPlayer(void);
    // get the player
    player& getPlayer(void)                                        {   return m_player;   }
    // run the game
    void run(sf::RenderWindow& window);
    // render the moves
    void renderGame(sf::RenderWindow& window);
    // make the player take action
    //void nextMove(sf::RenderWindow& window, sf::FloatRect area, bool showTrajectory);
    // get maze data file names
    std::string getMazeFile(void)                                 {   return m_mazeDataFile.string();   }
    // get brain data file names
    std::string getBrainFile(void)                                {   return m_brainDataFile.string();   }
    // get trajectory data file names
    std::string getTrajectoryFile(void)                           {   return m_trajectoryDataFile.string();   }
    
     
};

#endif /* defined(__mazeSolver__showGame__) */
