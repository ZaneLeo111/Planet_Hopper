

#include "Scene_Overworld.h"
#include "Scene_Menu.h"
#include "Scene_Play.h"
#include "Scene_Editor.h"
#include "Assets.h"
#include "GameEngine.h"
#include "Components.h"
#include "Action.h"
#include "Physics.h"

#include <iostream>
#include <fstream>

Scene_Overworld::Scene_Overworld(GameEngine* gameEngine)
    : Scene(gameEngine)
{
    init();
}

void Scene_Overworld::init()
{
    registerAction(sf::Keyboard::Escape, "QUIT");
    registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE");      // Toggle drawing (T)extures
    registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");    // Toggle drawing (C)ollision Boxes

    registerAction(sf::Keyboard::A, "LEFT");
    registerAction(sf::Keyboard::D, "RIGHT");
    registerAction(sf::Keyboard::Space, "SELECT");

    m_levelPaths.push_back("level1.txt");
    m_levelPaths.push_back("level2.txt");
    m_levelPaths.push_back("level3.txt");

    m_text.setFont(m_game->assets().getFont("Megaman"));

    m_game->assets().getSound("MusicTitle").stop();
    m_game->playSound("OverWorld");

    loadMap();
    std::cout << loadShaders() << "\n";
}

int Scene_Overworld::loadShaders()
{
    // Check if shaders are available
    if (!sf::Shader::isAvailable())
    {
        std::cerr << "Shader are not available" << std::endl;
        return -1;
    }

    // Load shaders
    if (!grey_shader.loadFromFile("images/new/shader_grey.frag", sf::Shader::Fragment))
    {
        std::cerr << "Error while loading grey shader" << std::endl;
        return -1;
    }
    if (!fade_shader.loadFromFile("images/new/shader_fade.frag", sf::Shader::Fragment))
    {
        std::cerr << "Error while loading fade shader" << std::endl;
        return -1;
    }
    if (!shake_shader.loadFromFile("images/new/shader_shake.frag", sf::Shader::Fragment))
    {
        std::cerr << "Error while loading shake shader" << std::endl;
        return -1;
    }
    if (!red_shader.loadFromFile("images/new/shader_red.frag", sf::Shader::Fragment))
    {
        std::cerr << "Error while loading red shader" << std::endl;
        return -1;
    }
}

void Scene_Overworld::loadMap()
{
    m_entityManager = EntityManager();

    planet1 = m_entityManager.addEntity("planet");
    planet1->addComponent<CAnimation>(m_game->assets().getAnimation("Plan1"), true);
    planet1->addComponent<CBoundingBox>(m_game->assets().getAnimation("Plan1").getSize());
    planet1->addComponent<CTransform>(Vec2(450, 360));
    planet1->addComponent<CLevelStatus>(true);

    planet2 = m_entityManager.addEntity("planet");
    planet2->addComponent<CAnimation>(m_game->assets().getAnimation("Plan2"), true);
    planet2->addComponent<CBoundingBox>(m_game->assets().getAnimation("Plan2").getSize());
    planet2->addComponent<CTransform>(Vec2(800, 360));
    planet2->addComponent<CLevelStatus>();

    planet3 = m_entityManager.addEntity("planet");
    planet3->addComponent<CAnimation>(m_game->assets().getAnimation("Plan3"), true);
    planet3->addComponent<CBoundingBox>(m_game->assets().getAnimation("Plan3").getSize());
    planet3->addComponent<CTransform>(Vec2(1150, 360));
    planet3->addComponent<CLevelStatus>();

    spawnPlayer();
}

void Scene_Overworld::spawnPlayer()
{
    m_player = m_entityManager.addEntity("player");

    m_player->addComponent<CTransform>(Vec2(100, 360));
    m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Stand"), true);
    m_player->addComponent<CBoundingBox>(Vec2(48, 48));
    m_player->addComponent<CInput>();
}

void Scene_Overworld::sMovement()
{

    CTransform& transform = m_player->getComponent<CTransform>();
    CInput& input = m_player->getComponent<CInput>();
    int offset = 40;
    if (input.right)
    {
        //transform.prevPos.x = transform.pos.x;
        if (planet1->getComponent<CLevelStatus>().unlocked 
            && transform.pos.x + offset < planet1->getComponent<CTransform>().pos.x)  
        { 
            transform.pos.x = planet1->getComponent<CTransform>().pos.x - offset; 
        }
        else if (planet2->getComponent<CLevelStatus>().unlocked 
            && transform.pos.x + offset < planet2->getComponent<CTransform>().pos.x)  
        { 
            transform.pos.x = planet2->getComponent<CTransform>().pos.x - offset; 
        }
        else if (planet3->getComponent<CLevelStatus>().unlocked 
            && transform.pos.x + offset < planet3->getComponent<CTransform>().pos.x)  
        { 
            transform.pos.x = planet3->getComponent<CTransform>().pos.x - offset; 
        }
        //std::cout << "pos: " << transform.pos.x << "\n";
        //std::cout << "prev pos: " << transform.prevPos.x << "\n";
        input.right = false;
    }
    else if (input.left)
    {
        //transform.pos.x = transform.prevPos.x;
        if      (transform.pos.x + offset == planet3->getComponent<CTransform>().pos.x) { transform.pos.x = planet2->getComponent<CTransform>().pos.x - offset; }
        else if (transform.pos.x + offset == planet2->getComponent<CTransform>().pos.x) { transform.pos.x = planet1->getComponent<CTransform>().pos.x - offset; }
        else if (transform.pos.x + offset == planet1->getComponent<CTransform>().pos.x) { transform.pos.x = transform.prevPos.x; }
        input.left = false;
    }
    
    //// horizontal movement
    //int xdir = (input.right - input.left);
    //transform.velocity.x = xdir * 350;
    //if (xdir != 0) transform.scale.x = xdir;

    //// vertical movement
    ////int ydir = (input.down - input.up);
    ////transform.velocity.y = ydir * 5;

    //// player speed limits
    //if (abs(transform.velocity.x) > 350)
    //{
    //    transform.velocity.x = transform.velocity.x > 0 ? 350 : -350;
    //}
    ////if (abs(transform.velocity.y) > 20)
    ////{
    ////    transform.velocity.y = transform.velocity.y > 0 ? 20 : -20;
    ////}

    //// updates prevPos and current pos
    //transform.prevPos = transform.pos;
    //transform.pos += transform.velocity;
}

void Scene_Overworld::sAnimation()
{

}

void Scene_Overworld::sCollision()
{
    shake = false;
    for (auto p : m_entityManager.getEntities("planet"))
    {
        if      (p->getComponent<CAnimation>().animation.getName() == "Plan1") { m_selectedMenuIndex = 0; }
        else if (p->getComponent<CAnimation>().animation.getName() == "Plan2") { m_selectedMenuIndex = 1; }
        else if (p->getComponent<CAnimation>().animation.getName() == "Plan3") { m_selectedMenuIndex = 2; }

        if (p->getComponent<CLevelStatus>().unlocked == true) { shader = &fade_shader; }
        if (p->getComponent<CLevelStatus>().unlocked == false) { shader = &red_shader; }

        Vec2 overlap = Physics::GetOverlap(m_player, p);
        if (overlap.x >= 0 && overlap.y >= 0)
        {
            p->getComponent<CAnimation>().animation.getSprite();
            shake = true;
            //shader = &shake_shader;
        }

        if (m_changeScene && overlap.x >= 0 && overlap.y >= 0)
        {
            m_game->changeScene("PLAY", std::make_shared<Scene_Play>(m_game, m_levelPaths[m_selectedMenuIndex]));
        }

        Vec2& pos = m_player->getComponent<CTransform>().pos;
        Vec2& r =   m_player->getComponent<CBoundingBox>().halfSize;
        if (pos.x - r.x <= 0) { pos.x = r.x; }
        if (pos.y - r.y <= 0) { pos.y = r.y; }
        if (pos.x + r.x >= m_game->window().getSize().x) { pos.x = m_game->window().getSize().x - r.x; }
        if (pos.y + r.y >= m_game->window().getSize().y) { pos.y = m_game->window().getSize().y - r.y; }

    }
}

void Scene_Overworld::sDoAction(const Action& action)
{
    if (action.type() == "START")
    {
        if (action.name() == "TOGGLE_TEXTURE") { m_drawTextures = !m_drawTextures; }
        else if (action.name() == "TOGGLE_COLLISION") { m_drawCollision = !m_drawCollision; }
        else if (action.name() == "QUIT") { onEnd(); }
        else if (action.name() == "SELECT") { m_changeScene = true; }

        else if (action.name() == "LEFT") { m_player->getComponent<CInput>().left = true; }
        else if (action.name() == "RIGHT") { m_player->getComponent<CInput>().right = true; }
    }

    else if (action.type() == "END")
    {
        if      (action.name() == "LEFT")   { m_player->getComponent<CInput>().left = false; }
        else if (action.name() == "RIGHT")  { m_player->getComponent<CInput>().right = false; }
        else if (action.name() == "SELECT") { m_changeScene = false; }
    }
}

void Scene_Overworld::update()
{
    m_entityManager.update();

    sMovement();
    sCollision();
    sAnimation();

    m_currentFrame++;
}

void Scene_Overworld::onEnd()
{
    m_hasEnded = true;
    m_game->assets().getSound("OverWorld").stop();
    m_game->playSound("MusicTitle");
    m_game->changeScene("MENU", nullptr, true);
}

void Scene_Overworld::sRender()
{
    m_game->window().clear(sf::Color(127, 127, 127));
    sf::Texture backgroundTexture;
    backgroundTexture.loadFromFile("images/new/blue.png");
    backgroundTexture.setSmooth(false);
    //background.setTexture(backgroundTexture);

    Vec2 TextureSize(backgroundTexture.getSize().x, backgroundTexture.getSize().y);  //Added to store texture size.
    Vec2 WindowSize(m_game->window().getSize().x, m_game->window().getSize().y);   //Added to store window size.

    float ScaleX = (float)WindowSize.x / TextureSize.x;
    float ScaleY = (float)WindowSize.y / TextureSize.y;     //Calculate scale.

    background.setTexture(backgroundTexture);
    background.setScale(ScaleX, ScaleY);      //Set scale.  
    m_game->window().draw(background);

    // Set shader parameters
    fade_shader.setUniform("time", time.getElapsedTime().asSeconds());
    shake_shader.setUniform("time", time.getElapsedTime().asSeconds());
    //red_shader.setUniform("time", time.getElapsedTime().asSeconds());
    //grey_shader.setUniform("time", time.getElapsedTime().asSeconds());
    // draw all Entity textures / animations
    if (m_drawTextures)
    {
        
        for (auto e : m_entityManager.getEntities())
        {
            auto& transform = e->getComponent<CTransform>();

            if (e->hasComponent<CAnimation>())
            {
                auto& animation = e->getComponent<CAnimation>().animation;
                animation.getSprite().setRotation(transform.angle);
                animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
                animation.getSprite().setScale(transform.scale.x, transform.scale.y);
                if (e->tag() == "planet")   
                {
                    if (e->getComponent<CLevelStatus>().unlocked == true)   { shader = &fade_shader; }
                    if (e->getComponent<CLevelStatus>().unlocked && shake)  { shader = &shake_shader; }
                    if (e->getComponent<CLevelStatus>().unlocked == false)  { shader = &red_shader; }
                    //std::cout << "getting here" << "\n";
                    m_game->window().draw(animation.getSprite(), shader); 
                }
                else                        { m_game->window().draw(animation.getSprite()); }
            }
        }
    }

    // draw all Entity collision bounding boxes with a rectangleshape
    if (m_drawCollision)
    {
        for (auto e : m_entityManager.getEntities())
        {
            if (e->hasComponent<CBoundingBox>())
            {
                auto& box = e->getComponent<CBoundingBox>();
                auto& transform = e->getComponent<CTransform>();
                sf::RectangleShape rect;
                rect.setSize(sf::Vector2f(box.size.x - 1, box.size.y - 1));
                rect.setOrigin(sf::Vector2f(box.halfSize.x, box.halfSize.y));
                rect.setPosition(transform.pos.x, transform.pos.y);
                rect.setFillColor(sf::Color(0, 0, 0, 0));
                rect.setOutlineColor(sf::Color(255, 255, 255, 255));
                rect.setOutlineThickness(1);
                m_game->window().draw(rect);
            }
        }
    }

    // draw the controls in the bottom-left
    m_text.setCharacterSize(20);
    m_text.setFillColor(sf::Color::White);
    m_text.setString("move left: a    move right: d   select: space   back: esc");
    m_text.setPosition(sf::Vector2f(10, 690));
    m_game->window().draw(m_text);
}