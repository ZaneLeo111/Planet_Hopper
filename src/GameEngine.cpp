                                                    
#include "GameEngine.h"
#include "Assets.h"
#include "Scene_Play.h"
#include "Scene_Editor.h"
#include "Scene_Menu.h"
#include "Scene_Overworld.h"

#include <iostream>

GameEngine::GameEngine(const std::string & path)
{
    init(path);
}

void GameEngine::init(const std::string & path)
{
    m_assets.loadFromFile(path);
    diff = "NORMAL";

    m_window.create(sf::VideoMode(1280, 768), "Planet Hopper");
    m_window.setFramerateLimit(60);
                                                     
    changeScene("MENU", std::make_shared<Scene_Menu>(this));
}

std::shared_ptr<Scene> GameEngine::currentScene()
{
    return m_sceneMap[m_currentScene];
}
                                                     
bool GameEngine::isRunning()
{ 
    return m_running && m_window.isOpen();
}

sf::RenderWindow & GameEngine::window()
{
    return m_window;
}

void GameEngine::run()
{
    while (isRunning())
    {
        update();
    }
}

void GameEngine::sUserInput()
{
    sf::Event event;
    while (m_window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            quit();
        }
                                                     
        if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::X)
            {
                std::cout << "screenshot saved to " << "test.png" << std::endl;
                sf::Texture texture;
                texture.create(m_window.getSize().x, m_window.getSize().y);
                texture.update(m_window);
                if (texture.copyToImage().saveToFile("test.png"))
                {
                    std::cout << "screenshot saved to " << "test.png" << std::endl;
                }
            }
        }
                                                     
        if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
        {
            // if the current scene does not have an action associated with this key, skip the event
            if (currentScene()->getActionMap().find(event.key.code) == currentScene()->getActionMap().end()) { continue; }
           
            // determine start or end action by whether it was key pres or release
            const std::string actionType = (event.type == sf::Event::KeyPressed) ? "START" : "END";
                        
            // look up the action and send the action to the scene
            currentScene()->doAction(Action(currentScene()->getActionMap().at(event.key.code), actionType));
        }

        auto mousePos = sf::Mouse::getPosition(m_window);
        Vec2 mpos(mousePos.x, mousePos.y);

        if (event.type == sf::Event::MouseButtonPressed)
        {
            switch (event.mouseButton.button)
            {
            case sf::Mouse::Left: { currentScene()->doAction(Action("LEFT_CLICK", "START", mpos)); break; }
            case sf::Mouse::Middle: { currentScene()->doAction(Action("MIDDLE_CLICK", "START", mpos)); break; }
            case sf::Mouse::Right: { currentScene()->doAction(Action("RIGHT_CLICK", "START", mpos)); break; }
            default: break;
            }

        }

        if (event.type == sf::Event::MouseButtonReleased)
        {
            switch (event.mouseButton.button)
            {
            case sf::Mouse::Left: { currentScene()->doAction(Action("LEFT_CLICK", "END", mpos)); break; }
            case sf::Mouse::Middle: { currentScene()->doAction(Action("MIDDLE_CLICK", "END", mpos)); break; }
            case sf::Mouse::Right: { currentScene()->doAction(Action("RIGHT_CLICK", "END", mpos)); break; }
            default: break;
            }
        }

        if (event.type == sf::Event::MouseMoved)
        {
            currentScene()->doAction(Action("MOUSE_MOVE", "START", Vec2(event.mouseMove.x, event.mouseMove.y)));
        }
    }
}

void GameEngine::changeScene(const std::string& sceneName, std::shared_ptr<Scene> scene, bool endCurrentScene)
{
    if (scene)
    {
        m_sceneMap[sceneName] = scene;
    }
    else
    {
        if (m_sceneMap.find(sceneName) == m_sceneMap.end())
        {
            std::cerr << "Warning: Scene does not exist: " << sceneName << std::endl;
            return;
        }
    }

    if (endCurrentScene)
    {
        m_sceneMap.erase(m_sceneMap.find(m_currentScene));
    }
                                                     
    m_currentScene = sceneName;
}

bool GameEngine::hasScene(const std::string sceneName)
{
    if (m_sceneMap.find(sceneName) == m_sceneMap.end())
    {
        return false;
    }
    return true;
}

std::shared_ptr<Scene> GameEngine::getScene(const std::string &sceneName)
{
    
    if (m_sceneMap.find(sceneName) == m_sceneMap.end())
    {
        std::cerr << "Warning: Scene does not exist: " << sceneName << std::endl;
    }

    return m_sceneMap.find(sceneName)->second;
}

void GameEngine::update()
{
    if (!isRunning()) { return; }
    
    if (m_sceneMap.empty()) { return; }
    
    sUserInput();
    currentScene()->simulate(m_simulationSpeed);
    currentScene()->sRender();
    m_window.display();
}

void GameEngine::quit()
{
    m_running = false;
}

Assets& GameEngine::assets()
{
    return m_assets;
}

void GameEngine::playSound(const std::string &soundName)
{
    m_assets.getSound(soundName).play();
}

void GameEngine::playMusic(const std::string &musicName)
{
    m_assets.getMusic(musicName).play();
}

std::string GameEngine::getDiff()
{
    return diff;
}

void GameEngine::setDiff(std::string d)
{
    diff = d;
    if (d == "EASY")
    {
        takenScaler = 0.5;
        bulletScaler = 2.0;
    }

    if (d == "NORMAL")
    {
        takenScaler = 1.0;
        bulletScaler = 1.0;
    }

    if (d == "HARD")
    {
        takenScaler = 2.0;
        bulletScaler = 0.5;
    }
}

std::string GameEngine::getShootKey()
{
    return shootKey;
}

void GameEngine::setShootKey(std::string s)
{
    std::cout << "set shoot key get called" << std::endl;
    shootKey = s;
    
    if (s == "J")
    {
       gameControls.shoot = sf::Keyboard::J;
    }
    else if (s == "K")
    {
       gameControls.shoot = sf::Keyboard::K;
    }
    else if (s == "SPACE")
    {
       gameControls.shoot = sf::Keyboard::Space;
    }
}

std::string GameEngine::getMoveKey()
{
    return moveKey;
}

void GameEngine::setMoveKey(std::string s)
{
    moveKey = s;
    
    if (s == "WASD")
    {
       gameControls.up = sf::Keyboard::W;
       gameControls.gravity = sf::Keyboard::S;
       gameControls.left = sf::Keyboard::A;
       gameControls.right = sf::Keyboard::D;
    }
    else if (s == "ARROW")
    {
       gameControls.up = sf::Keyboard::Up;
       gameControls.gravity = sf::Keyboard::Down;
       gameControls.left = sf::Keyboard::Left;
       gameControls.right = sf::Keyboard::Right;
    }
}

