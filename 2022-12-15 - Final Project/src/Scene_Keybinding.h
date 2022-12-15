///\/\/\\\/\\\\\////\///\\//\\/\//\/\/\\\/\\\\\////\///\\//\\/\/
//
//  Assignment       COMP4300 - Assignment 3
//  Professor:       David Churchill
//  Year / Term:     2022-09
//  File Name:       Scene_Menu.h
//
//  Student Name:    Nathan French
//  Student User:    ncfrench
//  Student Email:   ncfrench@mun.ca
//  Student ID:      201943859
//  Group Member(s): [enter student name(s)]
//
///\/\/\\\/\\\\\////\///\\//\\/\//\/\/\\\/\\\\\////\///\\//\\/\/

#pragma once

#include "Common.h"
#include "Scene.h"
#include <map>
#include <memory>
#include <deque>

#include "EntityManager.h"

class Scene_Keybinding : public Scene
{

protected:
    std::string m_title;
    std::vector<std::string> m_menuStrings;
    sf::Text m_menuText;
    sf::Text confirmText;
    size_t m_selectedMenuIndex = 0;
    sf::Clock clock;
    std::string shootKey1;
    std::string moveKey1;

    void init();
    void update();
    void onEnd();
    void sDoAction(const Action &action);

public:
    Scene_Keybinding(GameEngine *gameEngine);
    void sRender();
};

// Copyright (C) David Churchill - All Rights Reserved
// COMP4300 - 2022-09 - Assignment 3
// Written by David Churchill (dave.churchill@gmail.com)
// Unauthorized copying of these files are strictly prohibited
// Distributed only for course work at Memorial University
// If you see this file online please contact email above