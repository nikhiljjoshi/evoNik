//
//  tileMap.cpp
//  mazeSolver
//
//  Created by Nikhil Joshi on 12/1/12.
//  Copyright (c) 2012 Nikhil Joshi. All rights reserved.
//

#include "tileMap.hpp"

void tileMap::loadTileMap(){
    
    // clear the (already existing) tile map
    m_tileMap.clear();
    
    // maze data file
    std::fstream mazeData;
    mazeData.open(m_tileMapFileName, std::ios::in);
    
    // read the maze data from file
    while (mazeData.good() && !mazeData.eof()) {
        // read one line at a time
        std::string line;
        getline(mazeData, line);
        // trim the line for trailing white spaces
        boost::trim(line);
        
        // split the line
        std::vector<std::string> splitLine;
        boost::split(splitLine, line, boost::is_any_of(" \t"));
        
        // if an empty line, don't read further
        if (splitLine.size() <= 1)
            break;
         
        // skip comments
        if (*(splitLine[0].c_str()) == '#')
            continue;
                    
        // the row is ready to be pushed to the map
        m_tileMap.push_back(splitLine);
    }
}


unsigned int tileMap::getWidth(){
    // if empty, load the map
    if (m_tileMap.empty())
        loadTileMap();
    
    return (unsigned int)m_tileMap[0].size();
    
}


unsigned int tileMap::getHeight(){
    // if empty, load the map
    if (m_tileMap.empty())
        loadTileMap();
    
    return (unsigned int)m_tileMap.size();
    
}

void tileMap::buildTiles(){
    
    // clear the (already existing) tile set
    m_tileSet.clear();
    
    // how many tiles (in the image)
    unsigned int numTilesX(m_tileSetImage.getSize().x/m_tileWidth),
    numTilesY(m_tileSetImage.getSize().y/m_tileHeight);
    
    // one tile rect
    sf::IntRect subRect;
    subRect.width = m_tileWidth;
    subRect.height = m_tileHeight;
    
    // separate tiles from the tileset file
    for (int y = 0; y < numTilesY; y++) {
        for (int x = 0; x < numTilesX; x++) {
            // Position the one-tile rectangle
            subRect.left = x*m_tileWidth;
            subRect.top = y*m_tileHeight;
            // Capture the tile
            sf::Sprite sp(m_tileSetImage, subRect);
            m_tileSet.push_back(sp);
            
        }
    }
}


void tileMap::setBackgroundTile(unsigned int bkgTileIndex){
    // set the background flag to true
    m_needBkgPaint = true;
    m_bkgTileIndex = bkgTileIndex;
}



void tileMap::printMap(std::ostream& fout){
    
    // if absent load tile map
    if (m_tileMap.empty())
        loadTileMap();
    
    // now print
    for (int y = 0; y < m_tileMap.size(); y++){
        for (int x = 0; x < m_tileMap[0].size(); x++)
            fout << m_tileMap[y][x] << "\t";
        fout << std::endl;
    }
}


void tileMap::setIndexingRule(std::string ruleString){
    
    // string vector to keep separated rules
    std::vector<std::string> rules;
    
    // first separate the tile index for "this" position
    boost::regex e("\\[(.*?)\\],\\s+(\\d+)");
    boost::smatch what;
    try {
        boost::regex_match(ruleString, what, e);
    } catch (std::exception& e){
        std::cerr << "Error: " << e.what() << std::endl;
        exit(1);
    }
    unsigned int index = atoi(what[2].str().c_str());
    
    // index trimmed rule string
    ruleString = what[1].str();
    
    // now separate various position -> map value triplets
    // e = boost::regex("(?:\\((\\d+),(\\d+)\\):\\s*(\\d+),*\\s*)+");
    boost::split(rules, ruleString, boost::is_any_of(";"), boost::token_compress_on);
    
    std::vector<boost::tuple<int, int, std::string> >tilemaps;
    
    e = boost::regex("\\s*\\(([+\\-]*\\d+),\\s*([+\\-]*\\d+)\\):\\s*([\\w\\d]+)");
    for (int i = 0; i < rules.size(); i++)
        if (boost::regex_match(rules[i], what, e))
            tilemaps.push_back(boost::make_tuple(atoi(what[1].str().c_str()),
                                                 atoi(what[2].str().c_str()),
                                                 what[3].str()));
    
    if (m_indexingRules.size() == 1) {
        std::cout << "Warning: More than one rule found!" << std::endl
        << "Rules are overridable! "
        << "Earlier rules are overridden in case of conflict!" << std::endl;
    }
    
    m_indexingRules.push_back(std::make_pair(tilemaps, index));
}


void tileMap::printIndexingRules(std::ostream& fout){
    
    fout << "The tiling is done according to the rules:" << std::endl;
    
    // iterate over each indexing rule
    for (std::vector<indexingRule>::iterator it = m_indexingRules.begin();
         it != m_indexingRules.end(); it++) {
        fout << "Rule no. " << it - m_indexingRules.begin() << ": [";
        // list each tiling condition
        for (tilingConditions::iterator it1 = (it->first).begin();
             it1 != (it->first).end(); it1++) {
            if (it1 != (it->first).begin())
                fout << "; ";
            fout << "(" << it1->get<0>() << ", "
            << it1->get<1>() << "): "
            << it1->get<2>();
        }
        // print corresponding index
        fout << "] => tile no. " << it->second << std::endl;
    }
    
}


unsigned int tileMap::whichTile(unsigned int posX, unsigned int posY){
    
    // iterate over each rule (latter rules override earlier ones)
    for (std::vector<indexingRule>::reverse_iterator it = m_indexingRules.rbegin();
         it != m_indexingRules.rend(); it++) {
        // this tile (could be applied
        bool thisTile = true;
        // check over every criterion
        for (tilingConditions::iterator it1 = (it->first).begin();
             it1 != (it->first).end() && thisTile; it1++){
            // tile positions being checked
            int currPosX(posX + it1->get<0>()),
            currPosY(posY + it1->get<1>());
            
            // bring the position inside maze, if not so
            while (currPosX < 0)
                currPosX = (currPosX + m_tileMap[0].size()) % (unsigned int)m_tileMap[0].size();
            while (currPosY < 0)
                currPosY = (currPosY + m_tileMap.size()) % (unsigned int)m_tileMap.size();
           
            if (m_tileMap[currPosY][currPosX] != it1->get<2>())
                thisTile = false;
        }
        // if all conditions satisfied, apply this tile
        if (thisTile)
            return it->second;
        
    }
    
    // no applicable tile was found
    std::cerr << "Error: No appropriate tile was found" << std::endl;
    
    // apply background tile
    if (m_needBkgPaint) {
        std::cerr << "I applied the background tile!" << std::endl;
        return m_bkgTileIndex;
    }
    else
        exit(1);
}

void tileMap::renderMap(sf::RenderWindow& window, sf::FloatRect area){
    
    
    // If unspecified, use the whole window as rendering area
    area.width = ( area.width) ? area.width : window.getSize().x;
    area.height = (area.height) ? area.height : window.getSize().y;

    
    // if empty, load tile map
    if (m_tileMap.empty())
        this->loadTileMap();
    
    // if not built earlier, build tiles
    if (m_tileSet.empty())
        this->buildTiles();
    
    // effective tile size
    // tile map scaled so that a whole width is always presented on the screen
    // double scaleX = ((double)area.width) / (m_tileWidth*m_tileMap[0].size());
    double scaleY = ((double)area.height / m_tileMap.size()) / m_tileHeight;
    
    // extent to which the tiling must be applied
    int numColumns = ((-1*m_offsetX) / (m_tileWidth*scaleY)) + 1 + area.width / (m_tileWidth*scaleY);
    
    // start tiling the window
    for (int i =0; i < m_tileMap.size(); i++)  // y co-ordinate
        for (int j = 0; j < numColumns; j++) {  // x co-ordinate
            // is background painting needed?
            if (m_needBkgPaint) {
                m_tileSet[m_bkgTileIndex].setScale(scaleY, scaleY);
                m_tileSet[m_bkgTileIndex].setPosition(m_offsetX + area.left + j*m_tileWidth*scaleY,
                                                      m_offsetY + area.top + i*m_tileHeight*scaleY);
                window.draw(m_tileSet[m_bkgTileIndex]);
            }
            
            // pick up the correct tile and scale
            m_tileSet[whichTile(j % m_tileMap[0].size(), i)].setScale(scaleY, scaleY);
            m_tileSet[whichTile(j % m_tileMap[0].size(), i)].setPosition(m_offsetX + area.left + j*m_tileWidth*scaleY,
                                                                         m_offsetY + area.top + i*m_tileHeight*scaleY);
            window.draw(m_tileSet[whichTile(j % m_tileMap[0].size(), i)]);
        }
}


