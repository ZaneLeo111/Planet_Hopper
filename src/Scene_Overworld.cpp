
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

    registerAction(sf::Keyboard::A, "LEFT");
    registerAction(sf::Keyboard::D, "RIGHT");
    registerAction(sf::Keyboard::Space, "SELECT");

    m_levelPaths.push_back("level1.txt");
    m_levelPaths.push_back("level2.txt");
    m_levelPaths.push_back("level3.txt");

    m_text.setFont(m_game->assets().getFont("ChunkFive"));
    
    m_game->assets().getMusic("MusicTitle").stop();

    m_game->playMusic("OverWorld");

    loadMap(m_game->progress);
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
    else
    {
        std::cout << "Shaders loaded successfully" << std::endl;
        return 0;
    }
}

void Scene_Overworld::loadMap(const int& levelAvailable)
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
    planet2->addComponent<CLevelStatus>(false);

    planet3 = m_entityManager.addEntity("planet");
    planet3->addComponent<CAnimation>(m_game->assets().getAnimation("Plan3"), true);
    planet3->addComponent<CBoundingBox>(m_game->assets().getAnimation("Plan3").getSize());
    planet3->addComponent<CTransform>(Vec2(1150, 360));
    planet3->addComponent<CLevelStatus>(false);

    if (levelAvailable == 2) { planet2->addComponent<CLevelStatus>(true); }
    if (levelAvailable == 3)
    { 
        planet2->addComponent<CLevelStatus>(true);
        planet3->addComponent<CLevelStatus>(true);
    }
    
    sf::VertexArray point(sf::Points, 1);
    for (int i = 0; i < 1280; i += 20)
    {
        point[0].position.x = i;
        point[0].position.y = 360;
        point[0].color = sf::Color::Red;
        points.push_back(point);
    }

    spawnPlayer();
}

void Scene_Overworld::spawnPlayer()
{
    m_player = m_entityManager.addEntity("player");

    m_player->addComponent<CTransform>(Vec2(30, 360));
    m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Spaceship"), true);
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
        transform.velocity.x = 10;
        direction = "right";
        
    }
    if (input.left)
    {
        transform.velocity.x = -10;
        direction = "left";
    }

    transform.prevPos = transform.pos;

    if (direction == "right")
    {
        if (currPlanet == 1 && !planet2->getComponent<CLevelStatus>().unlocked) { return; }
        else if (currPlanet == 2 && !planet3->getComponent<CLevelStatus>().unlocked) { return; }

        transform.scale.x = 1.0;
        if (direction != "none") { transform.pos.x += transform.velocity.x; }
        
        if (planet1->getComponent<CLevelStatus>().unlocked
            && transform.pos.x + offset == planet1->getComponent<CTransform>().pos.x)
        {
            direction = "none";
            currPlanet = 1;
        }
        else if (planet2->getComponent<CLevelStatus>().unlocked
            && transform.pos.x + offset == planet2->getComponent<CTransform>().pos.x)
        {
            direction = "none";
            currPlanet = 2;
        }
        else if (planet3->getComponent<CLevelStatus>().unlocked
            && transform.pos.x + offset == planet3->getComponent<CTransform>().pos.x)
        {
            direction = "none";
            currPlanet = 3;
        }
    }

    if (direction == "left")
    {
        transform.pos.x += transform.velocity.x;
        transform.scale.x = -1.0;
        if      (transform.pos.x + offset == planet3->getComponent<CTransform>().pos.x) { currPlanet = 3; direction = "none"; }
        else if (transform.pos.x + offset == planet2->getComponent<CTransform>().pos.x) { currPlanet = 2; direction = "none"; }
        else if (transform.pos.x + offset == planet1->getComponent<CTransform>().pos.x) { currPlanet = 1; direction = "none"; }
        else if (transform.pos.x + offset > planet3->getComponent<CTransform>().pos.x) { currPlanet = 3; }
        else { currPlanet = 0; }
    }

    if (transform.pos.x < 30)   { transform.pos.x = 30; }
    if (transform.pos.x > 1255) { transform.pos.x = 1255; }
}

void Scene_Overworld::sCollision()
{
    for (auto p : m_entityManager.getEntities("planet"))
    {
        if      (p->getComponent<CAnimation>().animation.getName() == "Plan1") { m_selectedMenuIndex = 0; }
        else if (p->getComponent<CAnimation>().animation.getName() == "Plan2") { m_selectedMenuIndex = 1; }
        else if (p->getComponent<CAnimation>().animation.getName() == "Plan3") { m_selectedMenuIndex = 2; }

        Vec2 overlap = Physics::GetOverlap(m_player, p);
        if (overlap.x >= 0 && overlap.y >= 0)
        {
            p->getComponent<CAnimation>().animation.getSprite();
            shake = p->getComponent<CAnimation>().animation.getName();
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
        if (action.name() == "QUIT") { onEnd(); }
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

    m_currentFrame++;
}

void Scene_Overworld::onEnd()
{
    m_hasEnded = true;
    m_game->assets().getMusic("OverWorld").stop();
    m_game->playMusic("MusicTitle");
    m_game->changeScene("MENU", nullptr, true);
}

void Scene_Overworld::sRender()
{
    m_game->window().setView(m_game->window().getDefaultView());
    m_game->window().clear(sf::Color(127, 127, 127));
    sf::Texture backgroundTexture;
    backgroundTexture.loadFromFile("images/new/stars.png");
    backgroundTexture.setSmooth(true);
    backgroundTexture.setRepeated(true);

    sf::Texture backgroundTexture2;
    backgroundTexture2.loadFromFile("images/new/stars.png");
    backgroundTexture2.setSmooth(true);
    backgroundTexture2.setRepeated(true);

    Vec2 TextureSize(backgroundTexture.getSize().x, backgroundTexture.getSize().y);  //Added to store texture size.
    Vec2 WindowSize(m_game->window().getSize().x, m_game->window().getSize().y);   //Added to store window size.

    float ScaleX = (float)WindowSize.x / TextureSize.x;
    float ScaleY = (float)WindowSize.y / TextureSize.y;     //Calculate scale.

    background.setTexture(backgroundTexture);
    background.setScale(ScaleX, ScaleY);      //Set scale.
    

    sf::Sprite background2;
    background2.setTexture(backgroundTexture2);
    background2.setScale(ScaleX, ScaleY);      //Set scale.  

    offset++;
    background.setPosition(0 + offset, 0);
    background2.setPosition(-1280 + offset, 0);

    if (offset >= 1280)
    {
        offset = 0;
        background.setPosition(0, 0);
        background2.setPosition(-1280, 0);

    }
    
    m_game->window().draw(background);
    m_game->window().draw(background2);

    // Set shader parameters
    fade_shader.setUniform("time", time.getElapsedTime().asSeconds());
    shake_shader.setUniform("time", time.getElapsedTime().asSeconds());

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
                    if (e->getComponent<CLevelStatus>().unlocked == false)  { shader = &red_shader; }
                    if (e->getComponent<CLevelStatus>().unlocked && shake == e->getComponent<CAnimation>().animation.getName()) 
                    {
                        shader = &shake_shader; 
                    }
                    m_game->window().draw(animation.getSprite(), shader); 
                }
                else                        { m_game->window().draw(animation.getSprite()); }
            }
        }
    }

    // draw the game name in the top middle of the screem
    m_text.setCharacterSize(55);
    m_text.setFillColor(sf::Color::White);
    m_text.setString("PLANET HOPPER");
    m_text.setPosition(sf::Vector2f(400, 50));
    m_game->window().draw(m_text);

    // draw the controls in the bottom-left
    m_text.setCharacterSize(35);
    m_text.setFillColor(sf::Color::White);
    m_text.setString("move left: a    move right: d   select: space   back: esc");
    m_text.setPosition(sf::Vector2f(10, 690));
    m_game->window().draw(m_text);

    // draw red dotted points
    for (auto& p : points)
    {
        m_game->window().draw(p);
    }
}