                                                 
#include "Scene.h"
#include "GameEngine.h"
#include <cmath>

Scene::Scene()
{

}
                                                     
Scene::Scene(GameEngine * gameEngine)
    : m_game(gameEngine)
{ 
    
}

void Scene::setPaused(bool paused)
{
    m_paused = paused;
}

void Scene::setOptionMenu(bool open)
{
    m_optionMenuOpen = open;
    if (m_optionMenuOpen == false)
    {
        setPaused(false);
    }
}

size_t Scene::width() const
{
    return m_game->window().getSize().x;
}
                                                     
size_t Scene::height() const
{
    return m_game->window().getSize().y;
}

size_t Scene::currentFrame() const
{
    return m_currentFrame;
}
                                                     
bool Scene::hasEnded() const
{
    return m_hasEnded;
}

const ActionMap& Scene::getActionMap() const
{
    return m_actionMap;
}
                                                     
void Scene::registerAction(int inputKey, const std::string& actionName)
{
    m_actionMap[inputKey] = actionName;
}

void Scene::unregisterAction(int inputKey)
{
    // if the key is in the map, remove it
    if(m_actionMap.find(inputKey) != m_actionMap.end()) 
    {
        m_actionMap.erase(inputKey);
    }
}

void Scene::doAction(const Action& action)
{
    // ignore null actions
    if (action.name() == "NONE") { return; }

    sDoAction(action);
}

void Scene::simulate(const size_t frames)
{
    for (size_t i = 0; i < frames; i++)
    {
        update();
    }
}

void Scene::drawLine(const Vec2& p1, const Vec2& p2)
{
    sf::Vertex line[] = { sf::Vector2f(p1.x, p1.y), sf::Vector2f(p2.x, p2.y) };
    m_game->window().draw(line, 2, sf::Lines);
}

