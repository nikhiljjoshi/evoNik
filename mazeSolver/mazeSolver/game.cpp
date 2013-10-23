//
//  showGame.cpp
//  mazeSolver
//
//  Created by Nikhil Joshi on 12/1/12.
//  Copyright (c) 2012 Nikhil Joshi. All rights reserved.
//

#include "game.hpp"

boost::mutex m;

// to save a snap of the moment
void snapIt(sf::RenderWindow& window, std::string fileName) {
    
    static unsigned int index = 0;
    
    std::stringstream buffer;
    buffer << std::setfill('0') << std::setw(5) << index++;
    std::string fname(fileName+"-"+buffer.str()+".png");
    window.capture().saveToFile(fname);
}


bool game::configure(int argc, const char* argv[]){
    
    std::cout << "Reading program options..." << std::endl;
    // program options descritor
    po::options_description allOpts("");
    // general
    po::options_description genOpt("General options");
    genOpt.add_options()
    ("help", "produce help message")
    ;
    // mandatory
    po::options_description manOpt("Mandatory options");
    manOpt.add_options()
    ("tilesetData", po::value<std::string>()->required(),
     "tile set image file (required)")
    ("playerPosData", po::value<std::string>()->required(),
     "image file containing player poses (required)")
    ("mazeData", po::value<std::string>()->required(),
     "maze floor plan file path (required)")
    ("brainData", po::value<std::string>()->required(),
     "brain activity file path (required)")
    ("trajectoryData", po::value<std::string>()->required(),
     "agent trajectory file (required)")
    ;
    // modifiables
    po::options_description modifiers("Modifiable options");
    modifiers.add_options()
    ("takeSnaps", po::value<std::string>()->implicit_value("gameShots"),
     "take screen shots after every display refresh")
    ("music", po::value<std::string>()->implicit_value("NOT_SPECIFIED.mp3"),
     "play the music specified by the file")
    ;
    
    // compile all options
    allOpts.add(genOpt).add(manOpt).add(modifiers);
    // parse command line
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, allOpts), vm);
    
    // create help message
    if (vm.count("help")) {
        std::cout << allOpts << std::endl;;
        return false;
    }
    
    std::cout << "Read all the program options! Now, checking..." << std::endl;
    
    // check program options
    try {
        po::notify(vm);
    }
    
    catch (std::exception& e){
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
    catch(...){
        std::cerr << "Unknown error!" << std::endl;
        return false;
    }
    
    // should screen shots be saved to image files?
    if (vm.count("takeSnaps")) {
        m_takeScreenShots = true;
        m_screenShotsPrefix = vm["takeSnaps"].as<std::string>();
    }
    std::cout << "Check complete! Screen shot mode is selected as " << m_takeScreenShots << std::endl;
    
    
    std::cout << "Now, reading maze data file..." << std::endl;
    
    // data files
    // tile set file
    m_tileSetFile = fs::current_path()/vm["tilesetData"].as<std::string>();
    
    if (!exists(m_tileSetFile)) {
        std::cerr << "Error: tile set image file " << m_tileSetFile.string() << " does not exist"
        <<std::endl;
        return false;
    }
    
    // player poses
    m_playerPosFile = fs::current_path()/vm["playerPosData"].as<std::string>();
    
    if (!exists(m_playerPosFile)) {
        std::cerr << "Error: player poses image file " << m_playerPosFile << " does not exist"
        <<std::endl;
        return false;
    }
    
    // maze floor plan
    m_mazeDataFile = fs::current_path()/vm["mazeData"].as<std::string>();
    
    if (!exists(m_mazeDataFile)) {
        std::cerr << "Error: Maze planfile " << m_mazeDataFile << " does not exist"
        << std::endl;
        return false;
    }
    
    // brain activity
    m_brainDataFile = fs::current_path()/vm["brainData"].as<std::string>();
    
    if (!exists(m_brainDataFile)) {
        std::cerr << "Error: brain file " << m_brainDataFile << " does not exist"
        << std::endl;
        return false;
    }
    // trajectory
    m_trajectoryDataFile = fs::current_path()/vm["trajectoryData"].as<std::string>();
    
    if (!exists(m_trajectoryDataFile)) {
        std::cerr << "Error: trajectory data file " << m_trajectoryDataFile << " does not exist"
        << std::endl;
        return false;
    }
    
    // music file, if specified
    if (vm.count("music")) {
        m_musicFile = fs::current_path()/vm["music"].as<std::string>();
        
        if (!exists(m_musicFile)) {
            std::cerr << "Error: music file " << m_musicFile.string() << " does not exist"
            << std::endl
            << "I will play mute!" << std::endl;
        }
        
    }
    
    return true;
}


void game::setTileMap(){
    
    std::cout << "Tilemap construction started ..." << std::endl;
    m_tilemap = tileMap(m_mazeDataFile.c_str(), m_tileSetFile.c_str());
    
}


void game::setPlayer(){
    std::cout << "Player is being brought to life ..." << std::endl;
    m_player = player(m_playerPosFile.c_str(),
                      m_trajectoryDataFile.c_str(),
                      m_brainDataFile.c_str());    
}


void game::run(sf::RenderWindow& window){
    
    std::cout << "Game run is initiated." << std::endl;
        
    // adjust player trajectory to the tile map
    m_player.attachToTileMap(m_tilemap);
    
    
    // deactivate window rendering in this tread
    window.setActive(false);
    
    // launch the rendering thread
    boost::thread renderingThread(&game::renderGame, this, boost::ref(window));
    
    // run game
    while (window.isOpen()) {
        // Collect event
        sf::Event event;
        while (window.pollEvent(event)){
            // if the window is closed, return to regular sequence
            if (event.type == sf::Event::Closed) {
                m.lock();
                window.close();
                m.unlock();
            }
            // otherwise perform a task of ...
            else if (event.type == sf::Event::KeyPressed){
                if (event.key.code == sf::Keyboard::S){
                    window.display();
                    window.capture().saveToFile("screenCapture.png");
                }
                if (event.key.code == sf::Keyboard::Right)
                    m_playerSpeed = std::max(0, (int)m_playerSpeed - 20);
                if (event.key.code == sf::Keyboard::Left)
                    m_playerSpeed = std::min(500, (int)m_playerSpeed + 20);
                if (event.key.code == sf::Keyboard::Up)
                    m_tilemap.updateOffset(0, +1);
                if (event.key.code == sf::Keyboard::Down)
                    m_tilemap.updateOffset(0, -1);
                if (event.key.code == sf::Keyboard::N) {
                    m.lock();
                    m_player.m_deltaT = +1;
                    m_player.updatePosition();
                    m.unlock();
                }
                if (event.key.code == sf::Keyboard::P) {
                    m.lock();
                    m_player.m_deltaT = -1;
                    m_player.updatePosition();
                    m.unlock();
                }
                if (event.key.code == sf::Keyboard::D){
                    std::cout << "DetailedView set to " << !m_detailedView << std::endl;
                    m_detailedView = !m_detailedView;
                }
                if (event.key.code == sf::Keyboard::B){
                    std::cout << "showBrain set to " << !m_showBrain << std::endl;
                    m_showBrain = !m_showBrain;
                }
                if (event.key.code == sf::Keyboard::T){
                    std::cout << "showTrajectory set to " << !m_showTrajectory << std::endl;
                    m_showTrajectory = !m_showTrajectory;
                }
                if (event.key.code == sf::Keyboard::M){
                    std::cout << "showMovie set to " << !m_showMovie << std::endl;
                    m_showMovie = !m_showMovie;
                }
                
            }
        }
        
        
        // if movie mode is on
        if (m_showMovie) {
            m.lock();
            m_player.m_deltaT = +1;
            m_player.updatePosition();
            m.unlock();
        }
    }
    renderingThread.join();
}


void game::renderGame(sf::RenderWindow& window) {
    
    // activate the window
    window.setActive(true);
    
    // display areas
    double stripAreaHeight(0.2), gameAreaWidth(0.6);
    
    sf::FloatRect stripArea(0, 0,
                            window.getSize().x*2,
                            stripAreaHeight*window.getSize().y);
    sf::FloatRect gameArea(0,
                           stripAreaHeight*window.getSize().y,
                           gameAreaWidth*window.getSize().x,
                           (1 - stripAreaHeight)*window.getSize().y);
    sf::FloatRect brainArea(gameAreaWidth*window.getSize().x, stripAreaHeight*window.getSize().y,
                            (1 - gameAreaWidth)*window.getSize().x, (1 - stripAreaHeight)*window.getSize().y);
    

    
    // start the clock
    sf::Clock clock;
    
    while (window.isOpen()) {
        
        m.lock();
        
        // populate player moves
        std::vector<sf::Sprite> playerMoves = m_player.populateMoves(window, stripArea);
        
        // animate
        for (int i = 0; i < playerMoves.size(); i++) {
            
            // wait for
            clock.restart();
            while (clock.getElapsedTime().asMilliseconds() < m_playerSpeed){
            }
            
            
            // clear the window
            window.clear();
            
            // strip/panorama rendering
            m_tilemap.renderMap(window, stripArea);
            window.draw(playerMoves[i]);
            m_player.renderTrajectory(window, stripArea, true);
            
            
            // game zoomed in view
            // Focusing around player (x-) position : player always stays at the center of the view
            double focusHeight = 12.0 * (stripAreaHeight / m_tilemap.m_tileMap.size());
            double focusWidth = (gameArea.width / gameArea.height)*focusHeight;
            double focusRectLeft = std::max((double)0,
                                            m_player.getPosition(window, stripArea).x - focusWidth * window.getSize().x/2);

            double focusRectTop =  std::min(std::max((double)0,
                                                     m_player.getPosition(window, stripArea).y - focusHeight * window.getSize().y/2),
                                            (stripAreaHeight - focusHeight)*window.getSize().y);
            
            sf::FloatRect focus(focusRectLeft,
                                focusRectTop,
                                focusWidth * window.getSize().x,
                                focusHeight*window.getSize().y);
            
            
            sf::FloatRect zoom(0.018 + gameArea.left / window.getSize().x,
                               0.02 + gameArea.top / window.getSize().y,
                               0.95*(gameArea.width / window.getSize().x),
                               0.95*(gameArea.height / window.getSize().y));
            
            sf::View zoomedView;
            zoomedView.setViewport(zoom);
            zoomedView.reset(focus);

            // draw rectangle around the focus area
            // rect around focus
            roundedRectangle focusRect(sf::Vector2f(focus.width, 0.97*focus.height), 20);
            focusRect.setPosition(focusRectLeft, focusRectTop);
            focusRect.setOutlineColor(sf::Color(255,0,0));
            focusRect.setFillColor(sf::Color::Transparent);
            focusRect.setOutlineThickness(2);
            window.draw(focusRect);

            // set the zoomed-in view
            window.setView(zoomedView);
            m_tilemap.renderMap(window, stripArea);
            window.draw(playerMoves[i]);
            if (m_showTrajectory)
                m_player.renderTrajectory(window, stripArea);
            window.setView(window.getDefaultView());
            
            // draw rectangle around the zoomed game view
            roundedRectangle gameRect1(sf::Vector2f(gameArea.width, gameArea.height), 70);
            gameRect1.setPosition(gameArea.left, gameArea.top);
            gameRect1.setOutlineColor(sf::Color::Green);
            gameRect1.setOutlineThickness(30);
            gameRect1.setFillColor(sf::Color::Transparent);
            window.draw(gameRect1);
            roundedRectangle gameRect2(sf::Vector2f(0.986*gameRect1.getGlobalBounds().width,
                                                    0.986*gameRect1.getGlobalBounds().height),
                                       65);
            gameRect2.setOrigin(gameRect2.getGlobalBounds().width/2, gameRect2.getGlobalBounds().height/2);
            gameRect2.setPosition(gameRect1.getGlobalBounds().left + gameRect1.getGlobalBounds().width/2,
                                  gameRect1.getGlobalBounds().top + gameRect1.getGlobalBounds().height/2);
            gameRect2.setOutlineColor(sf::Color::Black);
            gameRect2.setOutlineThickness(20);
            gameRect2.setFillColor(sf::Color::Transparent);
            window.draw(gameRect2);
            
            // player brain activity
            if (m_showBrain){
                m_player.showBrain(window, brainArea, m_detailedView);
                
                // border
                roundedRectangle brainBorder(sf::Vector2f(brainArea.width, brainArea.height), 50);
                brainBorder.setPosition(brainArea.left, brainArea.top);
                brainBorder.setOutlineColor(sf::Color::Green);
                brainBorder.setOutlineThickness(3);
                brainBorder.setFillColor(sf::Color::Transparent);
                window.draw(brainBorder);
                
            }
            
            window.display();
            // if screen shot is to be taken
            if (m_takeScreenShots)
                snapIt(window, m_screenShotsPrefix);
            
        }
        m_player.m_deltaT = 0;
        m.unlock();
    }
}