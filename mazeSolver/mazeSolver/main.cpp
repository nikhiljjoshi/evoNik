//
//  main.cpp
//  mazeSolver
//
//  Created by Nikhil Joshi on 11/30/12.
//  Copyright (c) 2012 Nikhil Joshi. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <SFML/Graphics.hpp>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "table.hpp"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

#include "game.hpp"

int main(int argc, const char* argv[]) {
    
    sf::RenderWindow window(sf::VideoMode(1600,900), "Test");
    
//    sf::Font font;
//    font.loadFromFile("/Library/Fonts/Arial.ttf");
//    
//    cell c1("test", 20,
//            sf::Color::Blue,
//            0,
//            sf::Text::Style::Regular);
//        
//    while (window.isOpen()) {
//        sf::Event e;
//        while (window.pollEvent(e)) {
//            if (e.type == sf::Event::Closed) {
//                window.close();
//            }
//        }
//        
//        window.clear();
//        c1.render(window, sf::Vector2f(300,200));
//        window.display();
//        
//    }
    

    game g1(argc, argv);
    
    g1.getTileMap().setIndexingRule("[(0,0):0], 3");
    g1.getTileMap().setIndexingRule("[(0,0):1], 2");
    g1.getTileMap().setIndexingRule("[(0,0):2], 1");
    g1.getTileMap().setIndexingRule("[(0, 0):0; (0,-1):1; (0,1):1], 8");
    g1.getTileMap().setBackgroundTile(3);
    
    g1.run(window);
    
    return 0;
}
