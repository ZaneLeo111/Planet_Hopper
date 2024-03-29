
#pragma once

#include "Scene.h"
#include <map>
#include <memory>

#include "EntityManager.h"

class Scene_Editor : public Scene
{
    struct PlayerConfig
    {
        float X, Y, CX, CY, SPEED, MAXSPEED, JUMP, GRAVITY;
        std::string WEAPON;
    };

    struct LevelConfig
    {
        bool DARK;
        std::string MUSIC, BACKGROUND, NAME;
    };


protected:

    std::shared_ptr<Entity> m_player;
    std::shared_ptr<Entity> m_camera;
    PlayerConfig            m_playerConfig;
    LevelConfig             m_levelConfig;

    // drawing
    bool                    m_drawTextures = true;
    bool                    m_drawCollision = false;
    int                     m_drawGrid = 2;
    bool                    m_drawCamera = true;
    bool                    m_help = true;

    // grid
    const Vec2              m_gridSize = { 64, 64 };
    const Vec2              m_BOUNDARYNEG = { 0, 0 };
    const Vec2              m_BOUNDARYPOS = { 200, 30 };

    // text
    sf::Text                m_gridText;
    sf::Text                m_controlText;
    sf::Text                m_selectionText;
    sf::Text                m_buttonText;
    sf::Text                m_modText;

    // camera
    int                     m_CAMERA_SPEED = 5;
    std::string             m_CAMERA_AVATAR = "Bomb";
    Vec2                    m_BOUND_BOX = Vec2(6, 6);
    bool                    m_fast = false;

    // mouse
    Vec2                    m_mPos = { 0, 0 };
    bool                    m_drop = false;
    bool                    m_place = false;
    std::shared_ptr<Entity> m_selected = NULL;

    // patrol point
    bool                    m_patrol = true;

    // mute for music
    bool                    m_mute = false;

    // debounce for modifiy function
    bool                    m_modIncrease = true;
    bool                    m_modDecrease = true;

    // modifiable types by tag
    std::map<std::string, std::vector<std::string>> m_modTypes
    {
        {"tile", {"Block vision", "Block move", "Damage", "Speed"}},
        {"npc", {"Health", "Damage", "Jump", "Gravity", "Speed", "AI"}},
        {"player", {"Jump", "Gravity", "Speed"}}
    };

    // max and min of mod values
    std::map<std::string, std::vector<int>> m_minMax
    {
        {"Health", {1, 20}},
        {"Damage", {0, 20}},
        {"Jump",   {-20, 0}},
        {"Speed",  {0, 20}},
        {"Gravity", {-1, 1}}
    };

    // animation list
    std::vector<std::string> m_aniAssets = {};

    // entity types for menu
    std::vector<std::string> m_entityTypes
    {
        "tile", "dec", "item", "npc"
    };

    // menus
    int m_menuSelection = 0;
    int m_pageSelection = 0;
    int m_saveLimit = 10;

    // level properties
    std::map<std::string, std::vector<std::string>>m_levelAssetList =
    {
        {"Music", {"MusicTitle", "Play", "OverWorld", "BossFight"}},
        {"Background", {"Background1", "Background2", "Background3", "None"}}
    };

    void init();

    void setUpSounds();

    void fillAssetList();

    std::string formatFloat(float f);

    void loadBlankLevel();
    void loadLevel(const std::string& filename);
    void saveLevel();

    void update();
    void onEnd();

    void spawnPlayer();
    void spawnCamera();

    bool pasteEntity(std::shared_ptr<Entity> entity);

    Vec2 windowToWorld(const Vec2& wpos) const;

    Vec2 gridToMidPixel(float x, float y, std::shared_ptr<Entity> entity);
    Vec2 midPixelToGrid(std::shared_ptr<Entity> entity);
    Vec2 midPixelToGrid(std::shared_ptr<Entity> entity, Vec2& pos);

    bool snapToGrid(std::shared_ptr<Entity> entity);
    bool snapToGrid(std::shared_ptr<Entity> entity, Vec2& point);

    void showEntityPage(int page);
    void clearMenu();
    void showSLMenu();

    void changeValue(int& i, int c, int min, int max);
    void changeValue(float& f, int c, int min, int max, float g);

    void modConfig();

    std::shared_ptr<Entity> createEntity(std::string animation);

    void sDoAction(const Action& action);

    void renderEntity(std::shared_ptr<Entity> e);
    void renderTile(std::shared_ptr<Entity> e);

    void sState();
    void sMovement();
    void sAnimation();
    void sCollision();
    void sRender();

public:

    Scene_Editor(GameEngine* gameEngine);

};
