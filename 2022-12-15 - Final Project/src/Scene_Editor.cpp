///\/\/\\\/\\\\\////\///\\//\\/\//\/\/\\\/\\\\\////\///\\//\\/\/
//
//  Assignment       COMP4300 - final
//  Professor:       David Churchill
//  Year / Term:     2022-09
//  File Name:       Scene_Editor.cpp
// 
//  Student Name:    Nathan French
//  Student User:    ncfrench
//  Student Email:   ncfrench@mun.ca
//  Student ID:      201943859
//  Group Member(s): Nathan
//
///\/\/\\\/\\\\\////\///\\//\\/\//\/\/\\\/\\\\\////\///\\//\\/\/

#include "Scene_Editor.h"
#include "Scene_Menu.h"
#include "Physics.h"
#include "Assets.h"
#include "GameEngine.h"
#include "Components.h"
#include "Action.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <math.h>
#include <cmath>

Scene_Editor::Scene_Editor(GameEngine* gameEngine)
    : Scene(gameEngine)
{
    init();
}

void Scene_Editor::init()
{
    registerAction(sf::Keyboard::Escape, "QUIT");
    registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE");      // Toggle drawing (T)extures
    registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");    // Toggle drawing (C)ollision Boxes
    registerAction(sf::Keyboard::G, "TOGGLE_GRID");         // Toggle drawing (G)rid
    registerAction(sf::Keyboard::F, "TOGGLE_CAMERA");       // Toggle drawing (F)amera
    registerAction(sf::Keyboard::Delete, "DELETE_MODE");    // Toggle (Delete) mode

    registerAction(sf::Keyboard::Num8, "MUSIC");
    registerAction(sf::Keyboard::Num9, "BACKGROUND");
    registerAction(sf::Keyboard::Num0, "DARK");
    registerAction(sf::Keyboard::Num1, "ENTITY_MENU");
    registerAction(sf::Keyboard::Num2, "SAVE/LOAD_MENU");

    registerAction(sf::Keyboard::W, "UP");
    registerAction(sf::Keyboard::S, "DOWN");
    registerAction(sf::Keyboard::A, "LEFT");
    registerAction(sf::Keyboard::D, "RIGHT");

    m_gridText.setCharacterSize(12);
    m_gridText.setFont(m_game->assets().getFont("Tech"));

    m_controlText.setCharacterSize(24);
    m_controlText.setFont(m_game->assets().getFont("Tech"));

    m_selectionText.setCharacterSize(48);
    m_selectionText.setFont(m_game->assets().getFont("Tech"));

    m_buttonText.setCharacterSize(24);
    m_buttonText.setFont(m_game->assets().getFont("Tech"));

    fillAssetList();
    loadBlankLevel();
}

Vec2 Scene_Editor::windowToWorld(const Vec2& window) const
{
    auto view = m_game->window().getView();

    float wx = view.getCenter().x - (m_game->window().getSize().x / 2);
    float wy = view.getCenter().y - (m_game->window().getSize().y / 2);

    return { window.x + wx, window.y + wy };
}

Vec2 Scene_Editor::gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity)
{
    Vec2 size = (entity->hasComponent<CBoundingBox>() ? 
        entity->getComponent<CBoundingBox>().size : 
        entity->getComponent<CAnimation>().animation.getSize());

    return Vec2(
        (gridX * m_gridSize.x) + size.x / 2,
        (m_BOUNDARYPOS.y - gridY) * m_gridSize.y - (size.y / 2)
    );
}

Vec2 Scene_Editor::midPixelToGrid(std::shared_ptr<Entity> entity)
{
    Vec2 size = (entity->hasComponent<CBoundingBox>() ?
        entity->getComponent<CBoundingBox>().size :
        entity->getComponent<CAnimation>().animation.getSize());

    Vec2 pos = entity->getComponent<CTransform>().pos;

    return Vec2(
        (pos.x - (size.x / 2)) / m_gridSize.x,
        m_BOUNDARYPOS.y - ((pos.y + (size.y / 2)) / m_gridSize.y)
    );
}

// makes sure entity is alligned to grid
// won't place if a certain entity is overlaping another or if out of bounds
bool Scene_Editor::snapToGrid(std::shared_ptr<Entity> entity)
{
    Vec2& ePos = entity->getComponent<CTransform>().pos;
    Vec2 snap = gridToMidPixel(floor(ePos.x / m_gridSize.x), (m_BOUNDARYPOS.y - 1) - floor(ePos.y / m_gridSize.y), entity);
    ePos = snap;
    for (auto& e : m_entityManager.getEntities())
    {
        if (e == entity || e->tag() == "dec" || e->tag() == "camera") continue;
        Vec2 o = Physics::GetOverlap(entity, e);
        if (o.x > 0 && o.y > 0)
        {
            return false;
        }
    }
    

    Vec2 absolutePos = { m_BOUNDARYPOS.x * m_gridSize.x , m_BOUNDARYPOS.y * m_gridSize.y };
    Vec2 absoluteNeg = { m_BOUNDARYNEG.x * m_gridSize.x , m_BOUNDARYNEG.y * m_gridSize.y };

    // out of bounds
    if (snap.x < absoluteNeg.x || snap.x > absolutePos.x ||
        snap.y < absoluteNeg.y || snap.y > absolutePos.y)
    {
        return false;
    }

    return true;
}
/*

// dev plan for level editor here

rotate entity?

modify mode / modifiy entity by middle click, camera locks onto entity, middle click entity to disengage,
list if values to change appears next to entity: 
health, speed, ai (cycle),gravity,jump,maxspeed?,damage, etc all will be increment cycle or increment
extra mode if ai is patrol, when right clicking on enemy, enter patrol mode, 
click on empty spaces to create points, then finish by clicking on enemy
list of entities that can be changed: player, enemy, hazard, moving platform

*/

void Scene_Editor::fillAssetList()
{
    std::vector<std::string> BLACK_LIST =
    {
        "Inventory", "Sky", "Stars", "Hill", "Land", "Craters", "SkyObj", "Rock", "Space"
    };

    std::string ASSET_FILE = "assets.txt";

    std::ifstream fin(ASSET_FILE);
    std::string temp;

    while (fin >> temp)
    {
        if (temp == "Animation")
        {
            fin >> temp;
            if (std::find(BLACK_LIST.begin(), BLACK_LIST.end(), temp) != BLACK_LIST.end())
            {
                fin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }
            else
            {
                m_aniAssets.push_back(temp);
            }
            fin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        else fin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

// remove floating point annoyance in txt
std::string Scene_Editor::formatFloat(float f)
{
    // round to 2 decimals
    f = std::ceil(f * 100.0) / 100.0;

    // convert to string
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << f;
    std::string str = ss.str();

    // remove any trailing 0's
    str = str.substr(0, str.find_last_not_of('0') + 1);
    if (str.find('.') == str.size() - 1)
    {
        str = str.substr(0, str.size() - 1);
    }
    return(str);
}

void Scene_Editor::loadBlankLevel()
{
    m_entityManager = EntityManager();

    m_game->assets().getSound("MusicTitle").stop();

    LevelConfig& lc = m_levelConfig;
    lc.MUSIC = "Play";
    lc.DARK = false;
    lc.BACKGROUND = "None";
    lc.NAME = "level1.txt";

    PlayerConfig& pc = m_playerConfig;
    pc.X = 0; pc.Y = 0; pc.CX = 46; pc.CY = 70; pc.SPEED = 5; pc.JUMP = -20; pc.MAXSPEED = 20; pc.GRAVITY = 0.75; pc.WEAPON = "Buster";

    spawnPlayer();
    spawnCamera();
    m_game->playSound(m_levelConfig.MUSIC);
}

void Scene_Editor::loadLevel(const std::string& filename)
{
    // reset the entity manager every time we load a level
    m_entityManager = EntityManager();

    m_levelConfig.MUSIC = "Play";
    m_levelConfig.DARK = false;
    m_levelConfig.BACKGROUND = "None";
    m_levelConfig.NAME = filename.substr(13);
    std::ifstream fin(filename);
    std::string temp;

    while (fin >> temp)
    {
        if (temp == "Lighting")
        {
            fin >> temp;
            if (temp == "Night")
            {
                m_levelConfig.DARK = true;
            }
        }
        else if (temp == "BackgroundType")
        {
            fin >> m_levelConfig.BACKGROUND;
        }
        else if (temp == "Music")
        {
            fin >> m_levelConfig.MUSIC;
        }

        else if (temp == "Dec" || temp == "Tile" || temp == "Hazard")
        {
            // tile and decs
            std::string type = temp;
            std::string texture;
            float x, y;
            fin >> texture >> x >> y;

            auto tile = m_entityManager.addEntity(type == "Dec" ? "dec" : "tile");
            tile->addComponent<CAnimation>(m_game->assets().getAnimation(texture), true);
            if (type == "Tile")
            {
                int m; int v;
                fin >> m >> v;
                tile->addComponent<CBoundingBox>(tile->getComponent<CAnimation>().animation.getSize(),m,v);
            }
            tile->addComponent<CTransform>(gridToMidPixel(x, y, tile));

            tile->addComponent<CDraggable>();
        }

        // items, player, and npc
        else if (temp == "Item")
        {
            std::string tileName;
            float x, y;
            fin >> tileName >> x >> y;
            Vec2 tSize = m_game->assets().getAnimation(tileName).getSize();

            auto item = m_entityManager.addEntity("item");

            item->addComponent<CAnimation>(m_game->assets().getAnimation(tileName), true);
            item->addComponent<CTransform>(gridToMidPixel(x, y, item));
            item->addComponent<CBoundingBox>(m_game->assets().getAnimation(tileName).getSize());
            item->addComponent<CDraggable>();
        }
        else if (temp == "Npc")
        {

        }
        else if (temp == "Player")
        {
            PlayerConfig& pc = m_playerConfig;
            fin >> pc.X >> pc.Y >> pc.CX >> pc.CY >> pc.SPEED
                >> pc.JUMP >> pc.MAXSPEED >> pc.GRAVITY >> pc.WEAPON;
        }
        else
        {
            fin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
    fin.close();
    spawnPlayer();
    spawnCamera();

    m_game->playSound(m_levelConfig.MUSIC);
}

void Scene_Editor::saveLevel()
{
    LevelConfig& lc = m_levelConfig;

    std::ofstream fout("customLevels/" + lc.NAME);
    std::string saveLine = "";

    // Player
    PlayerConfig& pc = m_playerConfig;
    Vec2 grid = midPixelToGrid(m_player);

    pc.X = grid.x; pc.Y = grid.y;
    pc.CX = m_player->getComponent<CBoundingBox>().size.x; pc.CY = m_player->getComponent<CBoundingBox>().size.y;
    pc.GRAVITY = m_player->getComponent<CGravity>().gravity;

    // I made the player pos int because they like to scoot around when saving/loading
    saveLine = "Player " + std::to_string((int)pc.X) + " " + std::to_string((int)pc.Y) +
        " " + formatFloat(pc.CX) + " " + formatFloat(pc.CY) + " " +
        formatFloat(pc.SPEED) + " " + formatFloat(pc.JUMP) + " " + formatFloat(pc.MAXSPEED) +
        " " + formatFloat(pc.GRAVITY) + " " + pc.WEAPON;

    fout << saveLine << std::endl;

    // World Properties
    // Lighting
    saveLine = "Lighting " + std::string(lc.DARK ? "Night" : "Day");
    fout << saveLine << std::endl;

    // Music
    saveLine = "Music " + lc.MUSIC;
    fout << saveLine << std::endl;

    // Background
    saveLine = "BackgroundType " + lc.BACKGROUND;
    fout << saveLine << std::endl;

    if (lc.BACKGROUND == m_levelAssetList["Background"][0])
    {
        saveLine = "Background Sky 0 1.25 1.5 640 384" + std::string("\n") +
            "Background Stars 1 1.25 1.65 640 384 0.95" + std::string("\n") +
            "Background SkyObj 1 1 1 640 200 0.85" + std::string("\n") +
            "Background Hill 1 1.25 1 640 574 0.50" + std::string("\n") +
            "Background Rock 1 1 1 640 574 0.10" + std::string("\n") +
            "Background Land 0 1.25 1 640 717" + std::string("\n") +
            "Background Craters 1 1 1 640 717 0.01";
        fout << saveLine << std::endl;
    }
    else if (lc.BACKGROUND == m_levelAssetList["Background"][1])
    {
        saveLine = "Background Sky 0 1.25 1.5 640 384" + std::string("\n") +
            "Background Stars 1 1.25 1.65 640 384 0.95" + std::string("\n") +
            "Background SkyObj 1 1 1 640 200 0.85" + std::string("\n") +
            "Background Hill 1 1.25 1 640 574 0.50" + std::string("\n") +
            "Background Rock 1 1 1 640 574 0.10" + std::string("\n") +
            "Background Land 0 1.25 1 640 717" + std::string("\n") +
            "Background Craters 1 1 1 640 717 0.01";
        fout << saveLine << std::endl;
    }
    else if (lc.BACKGROUND == m_levelAssetList["Background"][2])
    {
        saveLine = "Background Sky 0 1.25 1.5 640 384" + std::string("\n") +
            "Background Stars 1 1.25 1.65 640 384 0.95" + std::string("\n") +
            "Background SkyObj 1 1 1 640 200 0.85" + std::string("\n") +
            "Background Hill 1 1.25 1 640 574 0.50" + std::string("\n") +
            "Background Rock 1 1 1 640 574 0.10" + std::string("\n") +
            "Background Land 0 1.25 1 640 717" + std::string("\n") +
            "Background Craters 1 1 1 640 717 0.01";
        fout << saveLine << std::endl;
    }

    // level content
    for (auto& e : m_entityManager.getEntities())
    {
        if (!e->hasComponent<CTransform>() || !e->hasComponent<CAnimation>() || 
             e->tag() == "camera" || e->tag() == "button" || e->tag() == "player") continue;

        CTransform& t = e->getComponent<CTransform>();
        CAnimation& a = e->getComponent<CAnimation>();
        
        char u[1];
        u[0] = e->tag()[0];
        u[0] = toupper(u[0]);
        std::string tag = u[0] + e->tag().substr(1);
        
        grid = midPixelToGrid(e);

        saveLine = tag + " " + a.animation.getName() + " " + formatFloat(grid.x) + " " + formatFloat(grid.y);

        if (e->tag() == "tile")
        {
            saveLine = saveLine + " " + std::to_string(e->getComponent<CBoundingBox>().blockMove) +
                                  " " + std::to_string(e->getComponent<CBoundingBox>().blockVision);
        }
        else if (e->tag() == "dec")
        {

        }
        else if (e->tag() == "item")
        {

        }
        else if (e->tag() == "npc")
        {

        }
        else if (e->tag() == "hazard")
        {

        }
        fout << saveLine << std::endl;
    }
    fout.close();
}

void Scene_Editor::spawnPlayer()
{
    PlayerConfig& pc = m_playerConfig;

    // here is a sample player entity which you can use to construct other entities
    auto player = m_entityManager.addEntity("player");
    player->addComponent<CAnimation>(m_game->assets().getAnimation("Stand"), true);

    Vec2 spawnPos = gridToMidPixel(pc.X, pc.Y, player);
    player->addComponent<CTransform>(spawnPos);
    player->addComponent<CBoundingBox>(Vec2(pc.CX, pc.CY));
    player->addComponent<CDraggable>();
    m_player = player;
}

void Scene_Editor::spawnCamera()
{
    PlayerConfig& pc = m_playerConfig;

    m_camera = m_entityManager.addEntity("camera");
    m_camera->addComponent<CAnimation>(m_game->assets().getAnimation(m_CAMERA_AVATAR), true);

    Vec2 spawnPos = gridToMidPixel(0, 0, m_camera);
    m_camera->addComponent<CTransform>(spawnPos);
    m_camera->addComponent<CBoundingBox>(m_BOUND_BOX);

    m_camera->addComponent<CInput>();
    m_camera->addComponent<CState>("move");
}

bool Scene_Editor::pasteEntity(std::shared_ptr<Entity> e)
{
    auto entity = m_entityManager.addEntity(e->tag());
    entity->addComponent<CTransform>() = e->getComponent<CTransform>();
    entity->addComponent<CDraggable>();

    if (e->hasComponent<CBoundingBox>()) entity->addComponent<CBoundingBox>() = e->getComponent<CBoundingBox>();
    if (e->hasComponent<CAnimation>()) entity->addComponent<CAnimation>() = e->getComponent<CAnimation>();
    if (e->hasComponent<CGravity>()) entity->addComponent<CGravity>() = e->getComponent<CGravity>();

    entity->getComponent<CTransform>().pos = windowToWorld(m_mPos);
    if (snapToGrid(entity))
    {
        return(true);
    }
    else
    {
        entity->destroy();
        return(false);
    }
}

void Scene_Editor::showEntityPage(int page)
{
    clearMenu();
    Vec2& camPos = m_camera->getComponent<CTransform>().pos;

    Vec2 startCorner = { camPos.x - (width() / 2), camPos.y - (height() / 2) };
    Vec2 endCorner = { startCorner.x + width(), startCorner.y + height() };
    int x = startCorner.x + m_gridSize.x;
    int y = startCorner.y + m_gridSize.y*3;

    for (int i = (112*page); i < (112*page)+113 && i < m_aniAssets.size(); i++)
    {
        auto b = m_entityManager.addEntity("button");
        b->addComponent<CTransform>(Vec2(x, y));
        b->addComponent<CAnimation>(m_game->assets().getAnimation(m_aniAssets[i]), true);
        b->addComponent<CBoundingBox>(Vec2( m_gridSize.x, m_gridSize.y ));

        Vec2 aniSize = b->getComponent<CAnimation>().animation.getSize();
        b->getComponent<CTransform>().scale = Vec2{ m_gridSize.x / aniSize.x, m_gridSize.y / aniSize.y };

        x += m_gridSize.x + 10;
        if (x > endCorner.x - m_gridSize.x) { x = startCorner.x + m_gridSize.x; y += (m_gridSize.y + 10); }
    }

}

void Scene_Editor::clearMenu()
{
    for (auto e : m_entityManager.getEntities("button"))
    {
        e->destroy();
    }
}

void Scene_Editor::showSLMenu()
{
    Vec2& camPos = m_camera->getComponent<CTransform>().pos;

    for (int i = 0; i < 2; i++)
    {
        auto b = m_entityManager.addEntity("button");
        b->addComponent<CTransform>(Vec2(camPos.x + (m_gridSize.x * 2.5 * (i == 0 ? -1 : 1)), camPos.y));
        b->addComponent<CAnimation>(m_game->assets().getAnimation("Plat"), true);
        b->addComponent<CBoundingBox>(Vec2(m_gridSize.x * 2, m_gridSize.y));

        Vec2 aniSize = b->getComponent<CAnimation>().animation.getSize();
        b->getComponent<CTransform>().scale = Vec2{ (m_gridSize.x * 2) / aniSize.x, m_gridSize.y / aniSize.y };

        b->addComponent<CButton>(i == 0 ? "save" : "load");
    }
}

std::shared_ptr<Entity> Scene_Editor::createEntity(std::string animation)
{
    auto e = m_entityManager.addEntity(m_entityTypes[m_menuSelection]);

    e->addComponent<CTransform>(windowToWorld(m_mPos));
    e->addComponent<CAnimation>(m_game->assets().getAnimation(animation), true);
    e->addComponent<CDraggable>();

    if (e->tag() == "tile")
    {
        e->addComponent<CBoundingBox>(e->getComponent<CAnimation>().animation.getSize(),1,1);
    }
    else if (e->tag() == "item")
    {
        e->addComponent<CBoundingBox>(e->getComponent<CAnimation>().animation.getSize());
    }

    return e;
}

void Scene_Editor::update()
{
    m_entityManager.update();

    sState();
    if (m_camera->getComponent<CState>().state != "entity" && m_camera->getComponent<CState>().state != "sl") sMovement();
    sCollision();
    sAnimation();
    sRender();

    m_currentFrame++;
}

void Scene_Editor::sState()
{

    CState& state = m_camera->getComponent<CState>();
    CInput& input = m_camera->getComponent<CInput>();

    // can drag if in move state
    if (state.state == "move")
    {
        if (input.click1 && !m_place)
        {
            for (auto e : m_entityManager.getEntities())
            {
                if (e->hasComponent<CDraggable>() && Physics::IsInside(windowToWorld(m_mPos), e))
                {
                    if (!e->getComponent<CDraggable>().dragging)
                    {
                        e->getComponent<CDraggable>().dragging = true;
                        m_selected = e;
                        state.state = "drag";
                        m_drop = false;
                        break;
                    }
                }
            }
        }
        else if (input.click2 && m_selected != NULL && m_selected->tag() != "player")
        {
            pasteEntity(m_selected);
        }
    }
    // can place and change to move during drag
    else if (state.state == "drag")
    {
        if (input.click1 && m_drop)
        {
            if (snapToGrid(m_selected))
            {
                m_selected->getComponent<CDraggable>().dragging = false;
                state.state = "move";
                m_place = true;
            }
        }
    }
    // deletes any non important entities in cursor pos
    else if (state.state == "delete" && input.click2)
    {
        for (auto e : m_entityManager.getEntities())
        {
            if (e->hasComponent<CDraggable>() && Physics::IsInside(windowToWorld(m_mPos), e) && e->tag() != "player")
            {
                if (e == m_selected) m_selected = NULL;
                e->destroy();
            }
        }
    }
    // entity menu buttons, makes a new entity if button is clicked
    else if (state.state == "entity" && input.click1)
    {
        bool clicked = false;
        for (auto& e : m_entityManager.getEntities("button"))
        {
            if (Physics::IsInside(windowToWorld(m_mPos), e))
            {
                m_selected = createEntity(e->getComponent<CAnimation>().animation.getName());
                m_selected->getComponent<CDraggable>().dragging = true;
                clicked = true;
                break;
            }
        }
        if (clicked)
        {
            m_menuSelection = 0;
            clearMenu();
            state.state = "drag";
            m_drop = false;
        }
    }
    else if (state.state == "sl" && input.click1)
    {
        std::string choice = "";
        for (auto& e : m_entityManager.getEntities("button"))
        {
            if (Physics::IsInside(windowToWorld(m_mPos), e))
            {
                choice = e->getComponent<CButton>().value;
                break;
            }
        }
        if (choice != "")
        {
            if (choice == "save")
            {
                clearMenu();
                m_levelConfig.NAME = "level" + std::to_string(m_menuSelection + 1) + ".txt";
                saveLevel();
                state.state = "move";
                m_menuSelection = 0;
            }
            else if (choice == "load")
            {
                std::string fileName = ("customLevels/level" + std::to_string(m_menuSelection + 1) + ".txt");
                std::ifstream f(fileName.c_str());
                if (f.good() == true)
                {
                    clearMenu();
                    m_game->assets().getSound(m_levelConfig.MUSIC).stop();
                    loadLevel(fileName);
                    m_menuSelection = 0;
                }
            }
        }
    }
}

void Scene_Editor::sMovement()
{
    CTransform& transform = m_camera->getComponent<CTransform>();
    CInput& input = m_camera->getComponent<CInput>();

    // camera movement \\

    // horizontal movement
    Vec2 dir = Vec2((input.right - input.left), (input.down - input.up));
    transform.velocity = dir * m_CAMERA_SPEED;

    // updates prevPos and current pos
    transform.prevPos = transform.pos;
    transform.pos += transform.velocity;

    // Dragging \\

    // if entity is being dragged, move with mouse
    if (m_selected != NULL && m_selected->getComponent<CDraggable>().dragging)
    {
        m_selected->getComponent<CTransform>().pos = windowToWorld(m_mPos);
    }

}

void Scene_Editor::sCollision()
{

    // boundary collison
    CTransform& transform = m_camera->getComponent<CTransform>();
    CBoundingBox& box = m_camera->getComponent<CBoundingBox>();

    Vec2 absolutePos = { m_BOUNDARYPOS.x * m_gridSize.x , m_BOUNDARYPOS.y * m_gridSize.y };
    Vec2 absoluteNeg = { m_BOUNDARYNEG.x * m_gridSize.x , m_BOUNDARYNEG.y * m_gridSize.y };

    // when the camera travels too far 
    if (transform.pos.x < absoluteNeg.x) transform.pos.x = absolutePos.x - box.halfSize.x;
    if (transform.pos.x > absolutePos.x) transform.pos.x = absoluteNeg.x + box.halfSize.x;

    // when the camerea falls into the sky
    if (transform.pos.y < absoluteNeg.y) transform.pos.y = absolutePos.y - box.halfSize.y;
    if (transform.pos.y > absolutePos.y) transform.pos.y = absoluteNeg.y + box.halfSize.y;
}

void Scene_Editor::sDoAction(const Action& action)
{
    if (action.type() == "START")
    {
        if (action.name() == "TOGGLE_TEXTURE") { m_drawTextures = !m_drawTextures; }
        else if (action.name() == "TOGGLE_COLLISION") { m_drawCollision = !m_drawCollision; }
        else if (action.name() == "TOGGLE_GRID") { m_drawGrid = (m_drawGrid + 1) % 3; }
        else if (action.name() == "TOGGLE_CAMERA") { m_drawCamera = !m_drawCamera; }
        else if (action.name() == "QUIT") { onEnd(); }

        else if (action.name() == "UP") 
        { 
            m_camera->getComponent<CInput>().up = true; 
            if (m_camera->getComponent<CState>().state == "entity")
            {
                m_pageSelection--;
                if (m_pageSelection < 0) m_pageSelection = floor((m_aniAssets.size() - 1) / 112);
                showEntityPage(m_pageSelection);
            }
        }
        else if (action.name() == "DOWN") 
        { 
            m_camera->getComponent<CInput>().down = true;
            if (m_camera->getComponent<CState>().state == "entity")
            {
                m_pageSelection++;
                if (m_pageSelection > (int)floor((m_aniAssets.size() - 1) / 112)) m_pageSelection = 0;
                showEntityPage(m_pageSelection);
            }
        }
        else if (action.name() == "LEFT") 
        { 
            m_camera->getComponent<CInput>().left = true; 
            if (m_camera->getComponent<CState>().state == "entity")
            {
                m_menuSelection--;
                if (m_menuSelection < 0) m_menuSelection = (m_entityTypes.size() - 1);
            }
            else if (m_camera->getComponent<CState>().state == "sl")
            {
                m_menuSelection--;
                if (m_menuSelection < 0) m_menuSelection = (m_saveLimit - 1);
            }
        }
        else if (action.name() == "RIGHT") 
        { 
            m_camera->getComponent<CInput>().right = true;
            if (m_camera->getComponent<CState>().state == "entity")
            {
                m_menuSelection = (m_menuSelection + 1) % m_entityTypes.size();
            }
            else if (m_camera->getComponent<CState>().state == "sl")
            {
                m_menuSelection = (m_menuSelection + 1) % (m_saveLimit);
            }
        }

        else if (action.name() == "LEFT_CLICK") { m_camera->getComponent<CInput>().click1 = true; }
        else if (action.name() == "RIGHT_CLICK") { m_camera->getComponent<CInput>().click2 = true; }
        else if (action.name() == "MIDDLE_CLICK") { m_camera->getComponent<CInput>().click3 = true; }
        else if (action.name() == "MOUSE_MOVE") { m_mPos = action.pos(); }

        else if (action.name() == "DELETE_MODE")
        {
            if (m_camera->getComponent<CState>().state == "move")
            {
                m_camera->getComponent<CState>().state = "delete";
            }
            else if (m_camera->getComponent<CState>().state == "delete")
            {
                m_camera->getComponent<CState>().state = "move";
            }
            else if (m_camera->getComponent<CState>().state == "drag")
            {
                m_camera->getComponent<CState>().state = "delete";
                m_selected->destroy();
                m_selected = NULL;
                m_drop = true;
                m_place = false;
            }
        }

        else if (action.name() == "MUSIC")
        {
            m_game->assets().getSound(m_levelConfig.MUSIC).stop();
            for (int i = 0; i < m_levelAssetList["Music"].size(); i++)
            {
                if (m_levelAssetList["Music"][i] == m_levelConfig.MUSIC)
                {
                    m_levelConfig.MUSIC = m_levelAssetList["Music"][(i + 1) % m_levelAssetList["Music"].size()];
                    break;
                }
            }
            m_game->playSound(m_levelConfig.MUSIC);
        }
        else if (action.name() == "BACKGROUND")
        {
            for (int i = 0; i < m_levelAssetList["Background"].size(); i++)
            {
                if (m_levelAssetList["Background"][i] == m_levelConfig.BACKGROUND)
                {
                    m_levelConfig.BACKGROUND = m_levelAssetList["Background"][(i + 1) % m_levelAssetList["Background"].size()];
                    break;
                }
            }
        }
        else if (action.name() == "DARK")
        {
            m_levelConfig.DARK = !m_levelConfig.DARK;
            saveLevel();
        }

        else if (action.name() == "ENTITY_MENU")
        {
            if (m_camera->getComponent<CState>().state == "entity")
            {
                m_menuSelection = 0;
                m_camera->getComponent<CState>().state = "move";
                clearMenu();
            }
            else if (m_camera->getComponent<CState>().state == "move")
            {
                m_pageSelection = 0;
                m_camera->getComponent<CState>().state = "entity";
                showEntityPage(0);
            }
        }
        else if (action.name() == "SAVE/LOAD_MENU")
        {
            if (m_camera->getComponent<CState>().state == "sl")
            {
                m_menuSelection = 0;
                m_camera->getComponent<CState>().state = "move";
                clearMenu();
            }
            else if (m_camera->getComponent<CState>().state == "move")
            {
                m_menuSelection = 0;
                m_camera->getComponent<CState>().state = "sl";
                showSLMenu();
            }
        }

    }
    else if (action.type() == "END")
    {
        if (action.name() == "UP") { m_camera->getComponent<CInput>().up = false; }
        if (action.name() == "DOWN") { m_camera->getComponent<CInput>().down = false; }
        if (action.name() == "LEFT") { m_camera->getComponent<CInput>().left = false; }
        if (action.name() == "RIGHT") { m_camera->getComponent<CInput>().right = false; }

        else if (action.name() == "LEFT_CLICK") 
        {
            m_camera->getComponent<CInput>().click1 = false; 
            m_drop = true;
            m_place = false;
        }
        else if (action.name() == "RIGHT_CLICK") 
        { 
            m_camera->getComponent<CInput>().click2 = false;
            m_texture = true;
        }
        else if (action.name() == "MIDDLE_CLICK") { m_camera->getComponent<CInput>().click3 = false; }

    }
}

void Scene_Editor::sAnimation()
{

    // update animation frame for all entities
    for (auto& e : m_entityManager.getEntities())
    {
        if (e->hasComponent<CAnimation>())
        {
            CAnimation& eAni = e->getComponent<CAnimation>();
            eAni.animation.update();
            if (!eAni.repeat && eAni.animation.hasEnded()) e->destroy();
        }
    }
}

void Scene_Editor::onEnd()
{
    m_hasEnded = true;
    m_game->assets().getSound(m_levelConfig.MUSIC).stop();
    m_game->playSound("MusicTitle");
    m_game->changeScene("MENU", std::shared_ptr<Scene_Menu>(), true);
}

void Scene_Editor::sRender()
{
    std::string s = m_camera->getComponent<CState>().state;

    // determines colour of background
    std::tuple<int,int,int> rgb;
    if (m_levelConfig.BACKGROUND == "Background1") rgb = { 60, 123, 232 };
    else if (m_levelConfig.BACKGROUND == "Background2") rgb = { 158, 132, 179 };
    else if (m_levelConfig.BACKGROUND == "Background3") rgb = { 150, 51, 64 };
    else rgb = { 148, 148, 148 };
    if (m_levelConfig.DARK)
    {
        std::get<0>(rgb) -= 50;
        std::get<1>(rgb) -= 50;
        std::get<2>(rgb) -= 50;
    }

    m_game->window().clear(sf::Color(std::get<0>(rgb), std::get<1>(rgb), std::get<2>(rgb)));

    // set the viewport of the window to be centered on the player if it's far enough right
    auto& pPos = m_camera->getComponent<CTransform>().pos;

    sf::View view = m_game->window().getView();
    view.setCenter(pPos.x, pPos.y);
    m_game->window().setView(view);

    Vec2 upperLeftCorner = Vec2((m_game->window().getView().getCenter().x - width() / 2),
        (m_game->window().getView().getCenter().y - height() / 2));
    // normal editor drawing
    if (s != "entity" && s != "sl")
    {
        // draw all Entity textures / animations
        if (m_drawTextures)
        {
            for (auto e : m_entityManager.getEntities())
            {
                if (e->tag() == "camera" && !m_drawCamera) continue;

                auto& transform = e->getComponent<CTransform>();

                if (e->hasComponent<CAnimation>())
                {
                    auto& animation = e->getComponent<CAnimation>().animation;
                    animation.getSprite().setRotation(transform.angle);
                    animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
                    animation.getSprite().setScale(transform.scale.x, transform.scale.y);
                    m_game->window().draw(animation.getSprite());
                }
            }
        }

        // guide text
        m_controlText.setString("Toggle: Texture = T | Collision Boxes = C | Camera = F | Grid = G (" + std::to_string(m_drawGrid) + ")"
            + "\n" + (s == "delete" ? "DELETE MODE ON (Right click to delete | DEL to toggle off)" :
                     (s == "move" ? "Menus: Entity = 1 | Save/Load = 2\nDelete Mode (DEL) | Paste Entity (Mouse2) "
                     + (m_selected != NULL ? "Entity: " + m_selected->getComponent<CAnimation>().animation.getName() : "") : 
                         "Place Entity (Mouse1) | Delete Mode (DEL)")));

        m_controlText.setPosition(upperLeftCorner.x, upperLeftCorner.y);
        m_game->window().draw(m_controlText);

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

        // draw the grid so that students can easily debug
        if (m_drawGrid > 0)
        {
            Vec2 absolutePos = { m_BOUNDARYPOS.x * m_gridSize.x , m_BOUNDARYPOS.y * m_gridSize.y };
            Vec2 absoluteNeg = { m_BOUNDARYNEG.x * m_gridSize.x , m_BOUNDARYNEG.y * m_gridSize.y };

            // define boundaries for lines
            float leftX = absoluteNeg.x;
            float rightX = absolutePos.x;
            float upY = absoluteNeg.y;
            float downY = absolutePos.y;

            float halfWidth = width() / 2.0f;
            float halfHeight = height() / 2.0f;

            float nextGridX = leftX - ((int)leftX % (int)m_gridSize.x);

            for (float x = nextGridX; x < rightX + 1; x += m_gridSize.x)
            {
                drawLine(Vec2(x, upY), Vec2(x, downY));
            }

            for (float y = upY; y < downY + 1; y += m_gridSize.y)
            {
                drawLine(Vec2(leftX, y), Vec2(rightX, y));

                if (y == downY) continue;
                for (float x = nextGridX; x < rightX; x += m_gridSize.x)
                {
                    if (x > pPos.x - halfWidth && x < pPos.x + halfWidth && y > pPos.y - halfHeight && y < pPos.y + halfHeight &&
                        m_drawGrid == 2)
                    {
                        std::string xCell = std::to_string((int)x / (int)m_gridSize.x);
                        std::string yCell = std::to_string(((int)m_BOUNDARYPOS.y - 1) - ((int)y / (int)m_gridSize.y));
                        m_gridText.setString("(" + xCell + "," + yCell + ")");
                        m_gridText.setPosition(x + 3, y + 2);
                        m_game->window().draw(m_gridText);
                    }
                }
            }
        }
    }
    // menu drawing
    else
    {
        float textWidth;
        if (s == "entity")
        {
            // menu drawing
            m_controlText.setString("A/D to cycle type | W/S to cycle page | 1 to return\nPage: " + std::to_string(m_pageSelection) + "/"
                + std::to_string((int)floor((m_aniAssets.size() - 1) / 112)));

            m_controlText.setPosition(upperLeftCorner.x, upperLeftCorner.y);
            m_game->window().draw(m_controlText);

            m_selectionText.setString("< " + m_entityTypes[m_menuSelection] + " >");

            textWidth = m_selectionText.getString().getSize() * m_selectionText.getCharacterSize();
            m_selectionText.setPosition(upperLeftCorner.x + (width() / 2) - (textWidth / 2), upperLeftCorner.y + m_gridSize.y);
            m_game->window().draw(m_selectionText);
        }
        else if (s == "sl")
        {
            m_controlText.setString("A/D to cycle saves | 2 to return");
            m_controlText.setPosition(upperLeftCorner.x, upperLeftCorner.y);
            m_game->window().draw(m_controlText);

            m_selectionText.setString("< level" + std::to_string(m_menuSelection + 1) + " >");

            textWidth = m_selectionText.getString().getSize() * m_selectionText.getCharacterSize();
            m_selectionText.setPosition(upperLeftCorner.x + (width() / 2) - (textWidth / 2),upperLeftCorner.y + m_gridSize.y);
            m_game->window().draw(m_selectionText);
        }

        for (auto e : m_entityManager.getEntities("button"))
        {
            auto& transform = e->getComponent<CTransform>();

            if (e->hasComponent<CAnimation>())
            {
                auto& animation = e->getComponent<CAnimation>().animation;
                animation.getSprite().setRotation(transform.angle);
                animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
                animation.getSprite().setScale(transform.scale.x, transform.scale.y);
                m_game->window().draw(animation.getSprite());
            }
            if (e->hasComponent<CButton>())
            {
                m_buttonText.setString(e->getComponent<CButton>().value);

                textWidth = m_buttonText.getString().getSize() * m_buttonText.getCharacterSize();
                m_buttonText.setPosition(transform.pos.x - (textWidth / 2), transform.pos.y);
                m_game->window().draw(m_buttonText);
            }
        }
    }
    // other controls
    m_controlText.setString("Music (8): " + m_levelConfig.MUSIC +
        " | Background (9): " + m_levelConfig.BACKGROUND + " | Dark (0): " + std::to_string(m_levelConfig.DARK)
    + " | Name: " + m_levelConfig.NAME);
    m_controlText.setPosition(upperLeftCorner.x, upperLeftCorner.y + height() - m_controlText.getCharacterSize() - 5);
    m_game->window().draw(m_controlText);
}
