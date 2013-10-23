//
//  tileMap.hpp
//  mazeSolver
//
//  Created by Nikhil Joshi on 11/30/12.
//  Copyright (c) 2012 Nikhil Joshi. All rights reserved.
//

#ifndef mazeSolver_tileMap_hpp
#define mazeSolver_tileMap_hpp

#include <iostream>
#include <fstream>
#include <string>
#include <map>

#include <boost/algorithm/string.hpp>
#include <boost/any.hpp>
#include <boost/regex.hpp>
#include <boost/tuple/tuple.hpp>

#include <SFML/Graphics.hpp>

typedef std::vector<boost::tuple<int, int, std::string> > tilingConditions;
typedef std::pair<tilingConditions, unsigned int> indexingRule;

class tileMap{
public:
    // tile size
    unsigned int m_tileWidth, m_tileHeight;
    // tile level (map) data file
    const char* m_tileMapFileName;
    // texture data file
    sf::Texture m_tileSetImage;
    // 2D tile map
    std::vector<std::vector<std::string> >m_tileMap;
    // texture data (tileset)
    std::vector<sf::Sprite> m_tileSet;
    // list of rules for constructing tilemap from tile set
    std::vector<indexingRule> m_indexingRules;
    // background tile index (if any)
    bool m_needBkgPaint;
    unsigned int m_bkgTileIndex;
    
    // off set for rendering window
    int m_offsetX, m_offsetY;
    
    // constructor
    tileMap(const char* tileMapFile, const char* tileSetFile,
            unsigned int tileWidth = 32, unsigned int tileHeight = 32)
    : m_tileMapFileName(tileMapFile),
    m_tileWidth(tileWidth), m_tileHeight(tileHeight){
        // set the texture file
        m_tileSetImage.loadFromFile(tileSetFile);
        
        // default background tile
        m_needBkgPaint = false;
        
        // set offsets to default (0)
        m_offsetX = m_offsetY = 0;
        
    }
    
    
    // default constructor
    tileMap(){
        
    }
    
    
    // copy constructor
    tileMap(const tileMap& o) {
        m_tileMapFileName = o.m_tileMapFileName,
        m_tileWidth = o.m_tileWidth,
        m_tileHeight = o.m_tileHeight,
        m_tileSetImage = o.m_tileSetImage;
        m_needBkgPaint = o.m_needBkgPaint;
        m_offsetX = o.m_offsetX;
        m_offsetY = o.m_offsetY;
        }
    
    // assignment operator
    tileMap& operator = (const tileMap o){
        m_tileMapFileName = o.m_tileMapFileName,
        m_tileWidth = o.m_tileWidth,
        m_tileHeight = o.m_tileHeight,
        m_tileSetImage = o.m_tileSetImage;
        m_needBkgPaint = o.m_needBkgPaint;
        m_offsetX = o.m_offsetX;
        m_offsetY = o.m_offsetY;
        return *this;
    }
        
        
    // member functions
    // build the tile map
    void loadTileMap(void);
    // get width
    unsigned int getWidth(void);
    // get height
    unsigned int getHeight(void);
    // build the tiles
    void buildTiles(void);
    // set correspondance for which tile for what symbol in map
    // the rule is usually of the form: [comma-separated list of ((xPos, yPos), tileMapVal) ], tileIndexValue
    // all positions with respect to "this" (tile to be indexed) position
    void setIndexingRule(std::string ruleString);
    // print indexing rules
    void printIndexingRules(std::ostream& fout = std::cout);
    // which tile is appropriate (according to given rules) at the given position
    unsigned int whichTile(unsigned int posX, unsigned int posY);
    // set background tile
    void setBackgroundTile(unsigned int bkgTileIndex);
    // show map
    void printMap(std::ostream& fout = std::cout);
    // render the map on the window
    void renderMap(sf::RenderWindow& window, sf::FloatRect area = sf::FloatRect());
    // update offset
    void updateOffset(int dx, int dy)                                                    {   m_offsetX += dx; m_offsetY += dy;  }
        
};


#endif
