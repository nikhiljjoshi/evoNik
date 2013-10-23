//
//  player.cpp
//  mazeSolver
//
//  Created by Nikhil Joshi on 12/3/12.
//  Copyright (c) 2012 Nikhil Joshi. All rights reserved.
//

#include "player.hpp"


void player::loadTrajectory(){
    
    // clear the (already existing) tile map
    m_positions.clear();
    
    // trajectory data file
    std::fstream trajectoryData;
    trajectoryData.open(m_trajectoryFileName, std::ios::in);
    
    // read the maze data from file
    while (trajectoryData.good() && !trajectoryData.eof()) {
        // read one line at a time
        std::string line;
        getline(trajectoryData, line);
        
        // trim the line for trailing white spaces
        boost::trim(line);
        // split the line
        std::vector<std::string> splitLine;
        boost::split(splitLine, line, boost::is_any_of(" \t"));
        
        // if the line is not appropriate or is a command
        if (splitLine.size() <= 1 || *(splitLine[0].c_str()) == '#')
            continue;
        
        // otherwise
        m_positions.push_back(std::make_pair(atoi(splitLine[3].c_str()),
                                             atoi(splitLine[4].c_str())));
    }
    
    // close the file
    trajectoryData.close();
}

void player::printTrajectory(std::ostream& fout){
    
    // if trajectory is empty, load it
    if (m_positions.empty()) {
        loadTrajectory();
    }
    
    fout << "Printing trajectory:" << std::endl;
    
    for (std::vector<std::pair<unsigned int, unsigned int> >::iterator it = m_positions.begin();
         it != m_positions.end(); it++) {
        fout << it - m_positions.begin() << "\t"     // time-step
        << "(" << it->first                         // x-coordinate
        << ", " << it->second << ")" << std::endl;         // y-coordinate
    }
}


void player::attachToTileMap(tileMap& t){
    
    // set the tilemap
    m_tilemap = &t;
    
    // if the tilemap is empty load it
    if (m_tilemap->m_tileMap.empty())
        m_tilemap->loadTileMap();
    
    // adjust the trajectory for the given tileMap
    unsigned int width(m_tilemap->getWidth()), height(m_tilemap->getHeight());
    
    // iterate over the trajectory
    for (std::vector<std::pair<unsigned int, unsigned int> >::iterator it = m_positions.begin();
         it != m_positions.end(); it++) {
        // adjust the x-position
        it->first %= width;
        // adjust the y-position
        it->second %= height;
    }
}


void player::resetTrajectory(){
    // clear the current trajectory
    m_positions.clear();
    
    // re-load the original trajectory
    loadTrajectory();
}


void player::renderTrajectory(sf::RenderWindow &window, sf::FloatRect area, bool showOnlyDots){
    
    // if area is not specified, use the whole window
    area.width = ( area.width) ? area.width : window.getSize().x;
    area.height = (area.height) ? area.height : window.getSize().y;
    
    // if not already, load brain data
    if (m_sensors.empty())
        loadBrainData();
    
    // effective tile size
    // tile map scaled so that a whole width is always presented on the screen
    // double scaleX = ((double)window.getSize().x) / (m_tileWidth*m_tileMap[0].size());
    double scaleY = (static_cast<double>(area.height) / m_tilemap->m_tileMap.size()) / m_tilemap->m_tileHeight;
    
    
    // trace over the trajectory so far and place sensor-stamps along it
    for (int i = 0; i < m_timeIndex; i++) {
        
        sf::Sprite thisTile;
        if (showOnlyDots)
            thisTile.setTexture(m_redDotImage);
        else
            thisTile = m_tilemap->m_tileSet[((m_nextBrainState[i] & 4032) >> 6) + 16];
        
        thisTile.setScale(scaleY, scaleY);
        thisTile.setPosition(m_tilemap->m_offsetX + area.left + m_positions[i].first*m_tilemap->m_tileWidth*scaleY,
                             m_tilemap->m_offsetY + area.top + m_positions[i].second*m_tilemap->m_tileHeight*scaleY);
        window.draw(thisTile);
    }
}


void player::setTileSize(unsigned int width, unsigned int height){
    m_tileWidth = width;
    m_tileHeight = height;
}


void player::buildPoses(void){
    
    // clear the (already existing) poses
    m_poses.clear();
    
    // how many poses (in the image)
    unsigned int numPosesX(m_playerPosImage.getSize().x/m_tileWidth),
    numPosesY(m_playerPosImage.getSize().y/m_tileHeight);
    
    // one tile rect
    sf::IntRect subRect;
    subRect.width = m_tileWidth;
    subRect.height = m_tileHeight;
    
    // separate tiles from the tileset file
    for (int y = 0; y < numPosesY; y++) {
        for (int x = 0; x < numPosesX; x++) {
            // Position the one-tile rectangle
            subRect.left = x*m_tileWidth;
            subRect.top = y*m_tileHeight;
            // Capture the tile
            sf::Sprite sp(m_playerPosImage, subRect);
            m_poses.push_back(sp);
            
        }
    }
}


void player::showPoses(sf::RenderWindow &window){
    
    // if empty, build poses
    if (m_poses.empty())
        buildPoses();
    
    // number of poses in one row
    unsigned int numColumns = floor(window.getSize().x / m_tileWidth) - 2;
    
    for (int i = 0; i < m_poses.size(); i++){
        m_poses[i].setPosition((i % numColumns)*m_tileWidth, floor(i / numColumns)*m_tileHeight);
        m_poses[i].setScale(1.0, 1.0);
        window.draw(m_poses[i]);
    }
}


std::vector<sf::Sprite> player::populateMoves(sf::RenderWindow &window,
                                              sf::FloatRect area,
                                              bool showTrajectory){
    
    // if area is not specified, use the whole window
    area.width = ( area.width) ? area.width : window.getSize().x;
    area.height = (area.height) ? area.height : window.getSize().y;
    
    // if empty, load trajectory
    if (m_positions.empty())
        this->loadTrajectory();
    
    // if not built earlier, build poses
    if (m_poses.empty())
        this->buildPoses();
    
    // check if attached with a tilemap
    if (m_tilemap == NULL) {
        std::cerr << "Error: no tilemap attached to the player!" << std::endl
        << "Insert attachToTileMap(tilemap t) method of player class and re-run." << std::endl;
        exit(1);
    }
    
    // scales to be used for placing and moving the player inside the maze
    
    // player sprite must be scaled to (match the tiling of the map)
    double scaleY = ((double)area.height / m_tilemap->m_tileMap.size()) / m_tileHeight;
    
    // translational units with which the player is moved
    // these many tiles must be placed vertically in the window =>
    double effTileHeight = area.height / m_tilemap->m_tileMap.size();
    // to preserve the aspect ratio, scale the width accordingly
    double effTileWidth = ( (m_tilemap->m_tileWidth /
                             m_tilemap->m_tileHeight ) *
                           (area.height /
                            m_tilemap->m_tileMap.size()) );
    
    
    // animation
    
    // movements stack
    std::vector<unsigned int> movements;
    std::vector<sf::Sprite> moveSprites;
    
    
    // if the player tries to move out of trajectory bounds (e.g. t < 0 or t > t_Total)
    if (m_timeIndex < 0 || m_timeIndex >= m_positions.size()) {

        // bring the player inside the trajectory
        m_timeIndex = (m_timeIndex < 0) ? 0 : (int)m_positions.size() - 1;
               
        // pick up the winning movement
        switch (m_curPose) {
            case 11:
                m_curPose = 18;
                break;
            default:
                m_curPose = 11;
                break;
        }
        m_poses[m_curPose].setScale(scaleY, scaleY);
        m_poses[m_curPose].setPosition(m_tilemap->m_offsetX + area.left + m_positions[m_timeIndex].first*effTileWidth,
                                       m_tilemap->m_offsetY + area.top + m_positions[m_timeIndex].second*effTileHeight);
        
        
        moveSprites.push_back(m_poses[m_curPose]);
    }
        
    else {
        
        // which direction the player is going?
        int dx(m_positions[m_timeIndex].first - m_positions[m_timeIndex - m_deltaT].first),
        dy(m_positions[m_timeIndex].second - m_positions[m_timeIndex - m_deltaT].second);
        
        // confirm a valid move
        if (dx*dy != 0) {
            std::cerr << "Error: Not a valid move!" << std::endl
            << "The player possibly moved diagonally at time-step " << m_timeIndex << ". Check the trajectory file!"
            << std::endl;
            exit(1);
        }
        
        // if dx and dy are both zero: not moving anywhere
        if (m_deltaT == 0)
            movements.push_back(m_curPose);
        
        // if going to the front
        else if (dx == 1) {
            if (m_curPose == 4) {// if was facing backwards
                movements.push_back(3);
                movements.push_back(5);
            }
            movements.push_back(9);
            movements.push_back(8);
            movements.push_back(9);
            movements.push_back(5);
        }
        
        // if going back
        else if (dx == -1){
            if (m_curPose == 5) {    // turn back if facing front
                movements.push_back(2);
                movements.push_back(4);
            }
            movements.push_back(0);
            movements.push_back(1);
            movements.push_back(0);
            movements.push_back(4);
        }
        
        // if climbing down or up
        else if (dy == +1 || dy == -1){
            movements.push_back(m_curPose);
            if (m_curPose == 5) {   // if moving forward
                movements.push_back(35);
                movements.push_back(17);
                movements.push_back(17);
                movements.push_back(35);
                movements.push_back(29);
            }
            else if (m_curPose == 4){  // if moving backwords
                movements.push_back(34);
                movements.push_back(12);
                movements.push_back(12);
                movements.push_back(34);
                movements.push_back(20);
            }
            movements.push_back(m_curPose);
        }
    
        // populate movements to be animated
        for (int i = 0; i < movements.size(); i++) {
            
            // pick up the correct tile and scale
            m_curPose = movements[i];
            m_poses[m_curPose].setScale(scaleY, scaleY);
            m_poses[m_curPose].setPosition(m_tilemap->m_offsetX + area.left + m_positions[m_timeIndex].first*effTileWidth
                                           - dx * effTileWidth * ((double)(movements.size() - 1 - i) / movements.size()),
                                           m_tilemap->m_offsetY + area.top + m_positions[m_timeIndex].second*effTileHeight
                                           - dy * effTileHeight * ((double)(movements.size() - 1 - i) / movements.size()));
            
            
            moveSprites.push_back(m_poses[m_curPose]);
        }
    }
    
    return moveSprites;
    
}


void player::resetBrain(){
    
    m_prevBrainState.clear();
    m_nextBrainState.clear();
    m_sensors.clear();
    m_uio.clear();
    m_hmmList.clear();
}


void player::loadBrainData(){
    
    // clear previous data
    resetBrain();
    
    // brain data file
    std::fstream brainData;
    brainData.open(m_playerBrainDataFile, std::ios::in);
    
    // read the maze data from file
    while (brainData.good() && !brainData.eof()) {
        // read one line at a time
        std::string line;
        getline(brainData, line);
        
        // if line is empty
        if (line.empty())
            continue;
        
        // inventory of split strings
        std::vector<std::string> splitStrings;
        
        // trim the line for trailing white spaces
        boost::trim(line);
        
        // if the line is a comment (or info. line)
        if (*(line.c_str()) == '#') {
            
            // extract the HMM unit
            boost::regex e("^#u (\\d+); ([\\d,]+); ([-\\d,]+); ([\\d\\.,]+)?; ([\\[\\],\\d]+);");
            boost::smatch what;
            if (boost::regex_match(line, what, e)) {
                // id
                unsigned int id = std::atoi(what[1].str().c_str());
                // if the unit already added, skip repetition
                bool repeat = false;
                for (std::vector<hmm>::iterator it = m_hmmList.begin();
                     it != m_hmmList.end(); it++)
                    if (id == it->m_id)
                        repeat = true;
                
                if (repeat)
                    continue;
                
                // inputs
                std::vector<unsigned int> inputs;
                std::string inputString = what[2].str();
                boost::split(splitStrings, inputString, boost::is_any_of(","));
                for (std::vector<std::string>::iterator it = splitStrings.begin();
                     it != splitStrings.end(); it++)
                    inputs.push_back(std::atoi((*it).c_str()));
                
                // outputs
                splitStrings.clear();
                std::vector<unsigned int> outputs;
                std::string outputString = what[3].str();
                boost::split(splitStrings, outputString, boost::is_any_of(","));
                for (std::vector<std::string>::iterator it = splitStrings.begin();
                     it != splitStrings.end(); it++)
                    outputs.push_back(std::atoi(it->c_str()));
                
                
                // transition probabilities
                // separate the rows
                std::vector<std::vector<unsigned int> > transProbs;
                std::vector<std::string> row;
                std::string transString = what[5].str();
                boost::split(row, transString, boost::is_any_of("[]"), boost::token_compress_on);
                for (std::vector<std::string>::iterator it = row.begin();
                     it != row.end(); it++){
                    
                    // separate the entries
                    splitStrings.clear();
                    boost::split(splitStrings, *it, boost::is_any_of(","));
                    // if not a trivial comma and has correct number of entries
                    if (*it != "," &&
                        splitStrings.size() == std::pow(2, (double)outputs.size()) ){
                        std::vector<unsigned int> rowEntries;
                        for (std::vector<std::string>:: iterator it = splitStrings.begin();
                             it != splitStrings.end(); it++)
                            rowEntries.push_back((unsigned int) ( (std::atof(it->c_str()) / 256.0) * 100 ) );
                        // one row is ready, push it to the transTable
                        transProbs.push_back(rowEntries);
                    }
                    
                }
                
                // create HMM unit now and push it to the player HMM list
                m_hmmList.push_back(hmm(id, inputs, outputs, transProbs));
                
            }
            
            // arrange the HMMs in ascending order of their id's
            std::sort(m_hmmList.begin(), m_hmmList.end());
            
        }
        
        // otherwise, the line is a per-time-step brain activity
        else{
            // clear temp. inventory
            splitStrings.clear();
            // split the line
            boost::split(splitStrings, line, boost::is_any_of("\t "));
            m_prevBrainState.push_back(std::atoi(splitStrings[1].c_str()));
            m_nextBrainState.push_back(std::atoi(splitStrings[2].c_str()));
            
            std::vector<std::string> uioString;
            boost::split(uioString, splitStrings[8], boost::is_any_of(":,"));
            std::vector<int> uios;
            for (std::vector<std::string>::iterator it = uioString.begin();
                 it != uioString.end(); it++) {
                uios.push_back(std::atoi(it->c_str()));
            }
            m_uio.push_back(uios);
        }
        
    }
}


void player::showBrain(sf::RenderWindow &window, sf::FloatRect area, bool detailedView){
    
    // proceed only if the window is open
    if (!window.isOpen())
        return;
    
    // if area is not specified, use the whole window
    area.width = ( area.width) ? area.width : window.getSize().x;
    area.height = (area.height) ? area.height : window.getSize().y;
    
    // if empty, load the brain data
    if (m_hmmList.empty())
        loadBrainData();
    
    // a texture on which the brain is temporarily rendered
    if (!m_brainCanvasCreated){
        if(!m_brainCanvas.create(std::max(800, (int)( (double)800*(area.width / area.height)) ),
                                 std::max(800, (int)( (double)800*(area.height / area.width)) ) )) {
            std::cerr << "Error: Texture creation failed in showBrain method" << std::endl;
            exit(1);
        }
        else
            m_brainCanvasCreated = true;
    }
    
    m_brainCanvas.clear();
    
    // position of HMM unit top left corner
    sf::FloatRect currentUnitRect;
    std::vector<double> maxExpanse(2);
    
    // show HMM units
    for (std::vector<hmm>::iterator it = m_hmmList.begin();
         it != m_hmmList.end(); it++){
        // Left top corner for the next unit
        std::vector<double> leftTop;
        
        // if the current unit left enough space, draw on the same line
        if (currentUnitRect.left + currentUnitRect.width < m_brainCanvas.getSize().x - 155) {
            leftTop.push_back(currentUnitRect.left + currentUnitRect.width);
            leftTop.push_back(currentUnitRect.top);
        }
        else { // draw on the next line
            leftTop.push_back(30);
            leftTop.push_back(currentUnitRect.top + currentUnitRect.height + 50);
        }
        
        // draw the unit
        currentUnitRect = it->renderUnit(m_brainCanvas, leftTop,
                                         detailedView,
                                         m_prevBrainState[m_timeIndex],
                                         m_nextBrainState[m_timeIndex]);
        
        if (maxExpanse[0] < currentUnitRect.left + currentUnitRect.width)
            maxExpanse[0] = currentUnitRect.left + currentUnitRect.width;
        if (maxExpanse[1] < currentUnitRect.top + currentUnitRect.height)
            maxExpanse[1] = currentUnitRect.top + currentUnitRect.height;
    }
    
    m_brainCanvas.setSmooth(true);
    m_brainCanvas.display();
    
    sf::Sprite tempSprite(m_brainCanvas.getTexture(), sf::IntRect(0,0, maxExpanse[0], maxExpanse[1]));
    
    // scale factor
    double scalingFactor = std::min((double)area.width/tempSprite.getGlobalBounds().width, (double)area.height/tempSprite.getGlobalBounds().height);
    
    tempSprite.setOrigin((double)tempSprite.getGlobalBounds().width/2, (double)tempSprite.getGlobalBounds().height/2);
    tempSprite.setPosition(sf::Vector2f(area.left + area.width/2, area.top + area.height/2));
    tempSprite.setScale(scalingFactor, scalingFactor);
    window.draw(tempSprite);
    
    
    // time stamp
    sf::Text timeStamp;
    timeStamp.setCharacterSize(20);
    timeStamp.setColor(sf::Color::White);
    timeStamp.setFont(m_hmmList.front().m_font);
    timeStamp.setStyle(sf::Text::Regular);
    timeStamp.setOrigin((double)timeStamp.getLocalBounds().width/2, (double)timeStamp.getLocalBounds().height/2);
    timeStamp.setPosition(area.left + 20, area.top + 20);
    std::stringstream buffer;
    buffer << m_timeIndex;
    timeStamp.setString("Brain at T = "+buffer.str());
    window.draw(timeStamp);
    
}


sf::Vector2f player::getPosition(sf::RenderWindow &window, sf::FloatRect area){
    
    // if area is not specified, use the whole window
    area.width = ( area.width) ? area.width : window.getSize().x;
    area.height = (area.height) ? area.height : window.getSize().y;
    
    // if empty, load trajectory
    if (m_positions.empty())
        this->loadTrajectory();
    
    // if not built earlier, build poses
    if (m_poses.empty())
        this->buildPoses();
    
    // check if attached with a tilemap
    if (m_tilemap == NULL) {
        std::cerr << "Error: no tilemap attached to the player!" << std::endl
        << "Insert attachToTileMap(tilemap t) method of player class and re-run." << std::endl;
        exit(1);
    }
    
    // scales to be used for placing and moving the player inside the maze
    // translational units with which the player is moved
    // these many tiles vertically
    double effTileHeight = area.height / m_tilemap->m_tileMap.size();
    // to preserve the aspect ratio, scale the width accordingly
    double effTileWidth = ( (m_tilemap->m_tileWidth /
                             m_tilemap->m_tileHeight ) *
                           (area.height /
                            m_tilemap->m_tileMap.size()) );
    
    
    return sf::Vector2f (m_tilemap->m_offsetX + area.left + m_positions[m_timeIndex].first*effTileWidth,
                         m_tilemap->m_offsetY + area.top + m_positions[m_timeIndex].second*effTileHeight);
    
    
}