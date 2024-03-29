
#pragma once
                                                     
#include "Common.h"
#include "Scene.h"
#include <map>
#include <memory>

#include "EntityManager.h"
                                                     
class Scene_Play : public Scene
{
    struct PlayerConfig
    {
        float X, Y, CX, CY, SPEED, MAXSPEED, JUMP, GRAVITY;
    };

protected:

    std::shared_ptr<Entity> m_player;
    std::map<std::string, std::vector<std::shared_ptr<Entity>>> m_backgroundsMap;
    sf::RenderTexture       m_renderTexture;
    sf::Texture             m_lightTexture;
    sf::Shader              electric_shader, bright_shader, speed_shader, rainbow_shader, red_shader;
    sf::Clock               time;
    std::string             m_levelPath;
    int                     m_levelEnd;
    int                     m_transition = 0, m_creditCountdown = 0;
    PlayerConfig            m_playerConfig;
    Vec2                    m_prevCameraPos;
    bool                    m_drawTextures = true;
    bool                    m_inventory = false;
    bool                    m_night = false;
    bool                    m_gameOver = false;
    const Vec2              m_gridSize = { 64, 64 };
    int                     m_countdown = 150;
    int                     m_action = 0;
    std::vector<sf::Text>   m_credits;
    sf::Text                m_gridText;
    sf::Text                m_weaponUIText;
    bool goal = false;
    std::string             m_levelMusic;

    std::shared_ptr<Entity> m_touchedPlatform;

    // inventory stuff
    std::vector< std::shared_ptr<Entity>> m_invContent = { NULL, NULL, NULL, NULL, NULL };
    int                     m_invSelect = 0;

    Vec2                    m_mPos;

    void init(const std::string & levelPath);

    void loadLevel(const std::string & filename);

    sf::Text displayText(std::string, float x, float y);
    sf::Sprite displayBox(float x, float y);
    std::shared_ptr<Entity> setupBullet(Vec2 size, Vec2 pos, int lifetime, int dmg, Vec2 speed, std::string name);
    sf::Text getText(std::string str);
    void loadBoss();
    void drawWeapon();
    void drawWeaponDisplay();
    void updateBackgrounds();
    void update();
    void onEnd();
    void spawnPlayer();
    void spawnBullet(std::shared_ptr<Entity> entity);

    sf::Sprite getLightingSprite(sf::Sprite gaugeSprite);

    Vec2 windowToWorld(const Vec2& window) const;
    Vec2 gridToMidPixel(float x, float y, std::shared_ptr<Entity> entity);
    
    void sDoAction(const Action& action);

    bool sInventory(std::string action, std::string name, int index);
    void sStatus();
    void sAI();
    void sCamera();
    void sMovement();
    void sLifespan();
    void sAnimation();
    void sCollision();
    void sRender();

public:

    Scene_Play(GameEngine * gameEngine, const std::string & levelPath);
                                                     
};

