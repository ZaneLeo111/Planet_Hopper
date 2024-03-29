
#include "Scene_OptionMenu.h"
#include "Scene_Play.h"
#include "Scene_Editor.h"
#include "Scene_Overworld.h"
#include "Scene_Keybinding.h"
#include "Assets.h"
#include "GameEngine.h"
#include "Components.h"
#include "Action.h"

Scene_OptionMenu::Scene_OptionMenu(GameEngine *gameEngine)
    : Scene(gameEngine)
{
    init();
}

void Scene_OptionMenu::init()
{
    registerAction(sf::Keyboard::W, "UP");
    registerAction(sf::Keyboard::Up, "UP");
    registerAction(sf::Keyboard::S, "DOWN");
    registerAction(sf::Keyboard::Down, "DOWN");

    registerAction(sf::Keyboard::D, "INCREASE");
    registerAction(sf::Keyboard::Right, "INCREASE");
    registerAction(sf::Keyboard::A, "DECREASE");
    registerAction(sf::Keyboard::Left, "DECREASE");

    registerAction(sf::Keyboard::E, "INCREASE10");
    registerAction(sf::Keyboard::Q, "DECREASE10");

    registerAction(sf::Keyboard::Escape, "QUIT");
    registerAction(sf::Keyboard::Enter, "CONFIRM");

    registerAction(sf::Keyboard::Space, "ENTER_KEYBINDING");

    // use for counting for displaying the confirm text
    clock.restart();

    if ( m_game->m_currentScene == "MENU" || m_game->m_currentScene == "PLAY")
    {
        std::string prevScene = m_game->m_currentScene;
        m_game -> optionMenu_return_scene = prevScene;
    }

    m_title = "Options";
    
    music_volume = m_game->assets().getMusicVolume();
    sounds_volume = m_game -> assets().getSoundsVolume();

    diff1 = m_game -> getDiff();

    m_menuStrings.push_back("Music Volume: " + std::to_string(int(music_volume)) + "/100");
    m_menuStrings.push_back("Sounds Effects Volume: " + std::to_string(int(sounds_volume)) + "/100");
    m_menuStrings.push_back("Difficulty: " + diff1);
    m_menuStrings.push_back("                        PRESS SPACE ENTER Key Binding");

    m_menuText.setFont(m_game->assets().getFont("ChunkFive"));
    m_menuText.setCharacterSize(64);

    int num = 1;
}

void Scene_OptionMenu::update()
{
    m_entityManager.update();
}

void Scene_OptionMenu::sDoAction(const Action &action)
{
    if (action.type() == "START")
    {
        if (action.name() == "UP")
        {
            m_game->playSound("option_menu_switch");
            if (m_selectedMenuIndex > 0)
            {
                m_selectedMenuIndex--;
            }
            else
            {
                m_selectedMenuIndex = m_menuStrings.size() - 2;
            }
        }
        else if (action.name() == "DOWN")
        {
            m_game->playSound("option_menu_switch");
            m_selectedMenuIndex = (m_selectedMenuIndex + 1) % (m_menuStrings.size() - 1);
        }
        else if (action.name() == "INCREASE")
        {
            // increase the music volume
            if (m_selectedMenuIndex == 0)
            {
                if (music_volume >= 0 && music_volume <= 99)
                {
                    m_game->assets().changeMusicVolume(music_volume += 1);
                }
            }
            // increase the sounds effect volume
            if (m_selectedMenuIndex == 1)
            {
                if (sounds_volume >= 0 && sounds_volume <= 99)
                {
                    m_game->assets().changeSoundsVolume(sounds_volume += 1);
                }
            }
            // increase the difficulty
            if (m_selectedMenuIndex == 2)
            {
                if (diff1 == "EASY")
                {
                    diff1 = "NORMAL";
                }
                else if (diff1 == "NORMAL")
                {
                    diff1 = "HARD";
                }
                else if (diff1 == "HARD")
                {
                    diff1 = "EASY";
                }
            }
        }
        else if (action.name() == "DECREASE")
        {
            // decrease the music volume
            if (m_selectedMenuIndex == 0)
            {
                if (music_volume >= 1 && music_volume <= 100)
                {
                    m_game->assets().changeMusicVolume(music_volume -= 1);
                }
            }
            // decrease the sounds effect volume
            if (m_selectedMenuIndex == 1)
            {
                if (sounds_volume >= 1 && sounds_volume <= 100)
                {
                    m_game->assets().changeSoundsVolume(sounds_volume -= 1);
                }
            }
            // decrease the difficulty
            if (m_selectedMenuIndex == 2)
            {
                if (diff1 == "EASY")
                {
                    diff1 = "HARD";
                }
                else if (diff1 == "NORMAL")
                {
                    diff1 = "EASY";
                }
                else if (diff1 == "HARD")
                {
                    diff1 = "NORMAL";
                }
            }
        }
        else if (action.name() == "INCREASE10")
        {
            // increase the music volume
            if (m_selectedMenuIndex == 0)
            {
                if (music_volume >= 0 && music_volume <= 90)
                {
                    m_game->assets().changeMusicVolume(music_volume += 10);
                }
            }
            // increase the sounds effect volume
            if (m_selectedMenuIndex == 1)
            {
                if (sounds_volume >= 0 && sounds_volume <= 90)
                {
                    m_game->assets().changeSoundsVolume(sounds_volume += 10);
                }
            }
        }
        else if (action.name() == "DECREASE10")
        {
            // decrease the music volume
            if (m_selectedMenuIndex == 0)
            {
                if (music_volume >= 10 && music_volume <= 100)
                {
                    m_game->assets().changeMusicVolume(music_volume -= 10);
                }
            }
            // decrease the sounds effect volume
            if (m_selectedMenuIndex == 1)
            {
                if (sounds_volume >= 10 && sounds_volume <= 100)
                {
                    m_game->assets().changeSoundsVolume(sounds_volume -= 10);
                }
            }
        }
        else if (action.name() == "CONFIRM")
        {   
            m_game -> playSound("confirm");
            m_game->setDiff(diff1);

            // display the confirmation text (prompt)
            confirmText.setString("Setting saved! Difficulty: " + diff1 + ".");
            confirmText.setCharacterSize(20);
            confirmText.setFillColor(sf::Color::Red);
            confirmText.setFont(m_game->assets().getFont("ChunkFive"));
            confirmText.setPosition(sf::Vector2f(500, 10));
        }
        else if (action.name() == "ENTER_KEYBINDING")
        {
            // enter the Scene_KeyBinding scene
            m_game->changeScene("KEYBINDING", std::make_shared<Scene_Keybinding>(m_game));
        }
        else if (action.name() == "QUIT")
        {
            onEnd();
        }
    }
}

void Scene_OptionMenu::sRender()
{   
    // std::cout << "prevScene" << m_game -> optionMenu_return_scene << std::endl;
    sf::Time elapsed1 = clock.getElapsedTime();

    // clear the window to a blue
    m_game->window().setView(m_game->window().getDefaultView());
    m_game->window().clear(sf::Color(150, 200, 255));

    // draw the game title in the top-left of the screen
    m_menuText.setCharacterSize(48);
    m_menuText.setString(m_title);
    m_menuText.setFillColor(sf::Color::Black);
    m_menuText.setPosition(sf::Vector2f(10, 10));
    m_game->window().draw(m_menuText);

    // draw all of the menu options
    // update the option text
    for (size_t i = 0; i < m_menuStrings.size(); i++)
    {
        m_menuStrings[0] = "Music Volume: " + std::to_string(int(music_volume)) + "/100";
        m_menuStrings[1] = "Sounds Effect Volume: " + std::to_string(int(sounds_volume)) + "/100";
        m_menuStrings[2] = "Difficulty: " + diff1;
        m_menuText.setString(m_menuStrings[i]);
        m_menuText.setFillColor(i == m_selectedMenuIndex ? sf::Color::White : sf::Color(0, 0, 0));
        m_menuText.setPosition(sf::Vector2f(10, 110 + i * 72));
        m_game->window().draw(m_menuText);
    }

    // draw the controls in the bottom-left
    m_menuText.setCharacterSize(20);
    m_menuText.setFillColor(sf::Color::Black);
    m_menuText.setString("UP: w/up DOWN: s/down ADJUST: a/d/left/right,  QUIT: esc  CONFIRM: enter  ENTER KEYBINDING: space");
    m_menuText.setPosition(sf::Vector2f(10, 690));

    // disapper the confirmation text after 2 seconds
    if (elapsed1.asSeconds() > 2)
    {
        confirmText.setString("");
        clock.restart();
    }
    m_game->window().draw(confirmText);


    m_game->window().draw(m_menuText);
}

void Scene_OptionMenu::onEnd()
{
    m_hasEnded = true;
    m_game->changeScene(m_game->optionMenu_return_scene, nullptr, true);
    m_game->getScene(m_game->m_currentScene)->setOptionMenu(false);
    m_game->getScene(m_game->m_currentScene)->setPaused(false);
}

