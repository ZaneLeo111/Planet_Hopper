///\/\/\\\/\\\\\////\///\\//\\/\//\/\/\\\/\\\\\////\///\\//\\/\/
//
//  Assignment       COMP4300 - Assignment 3
//  Professor:       David Churchill
//  Year / Term:     2022-09
//  File Name:       Vec2.cpp
// 
//  Student Name:    Nathan French
//  Student User:    ncfrench
//  Student Email:   ncfrench@mun.ca
//  Student ID:      201943859
//  Group Member(s): [enter student name(s)]
//
///\/\/\\\/\\\\\////\///\\//\\/\//\/\/\\\/\\\\\////\///\\//\\/\/

#include "Vec2.h"
#include <math.h>

Vec2::Vec2()
{
                                                     
}

Vec2::Vec2(float xin, float yin)
    : x(xin)
    , y(yin)
{

}
                                                     
Vec2 Vec2::operator + (const Vec2 & rhs) const
{
    return Vec2(x + rhs.x, y + rhs.y);
}

Vec2 Vec2::operator - (const Vec2 & rhs) const
{
    return Vec2(x - rhs.x, y - rhs.y);
}
                                                     
Vec2 Vec2::operator / (const float & val) const
{
    return Vec2(x / val, y / val);
}
                                                     
Vec2 Vec2::operator * (const float & val) const
{
    return Vec2(x * val, y * val);
}
                                                     
bool Vec2::operator == (const Vec2 & rhs) const
{
    return x == rhs.x && y == rhs.y;
}

bool Vec2::operator != (const Vec2 & rhs) const
{
    return !(*this == rhs);
}
                                                     
void Vec2::operator += (const Vec2 & rhs)
{
    x += rhs.x;
    y += rhs.y;
}

void Vec2::operator -= (const Vec2 & rhs)
{
    x -= rhs.x;
    y -= rhs.y;
}
                                                     
void Vec2::operator *= (const float & val)
{
    x *= val;
    y *= val;
}

void Vec2::operator /= (const float & val)
{
    x /= val;
    y /= val;
}

float Vec2::dist(const Vec2 & rhs) const
{
    return sqrtf((x - rhs.x)*(x - rhs.x) + (y - rhs.y)*(y - rhs.y));
}

Vec2 Vec2::abs() const
{
    return Vec2(x < 0 ? -x : x, y < 0 ? -y : y);
}
                                                     
// Copyright (C) David Churchill - All Rights Reserved
// COMP4300 - 2022-09 - Assignment 3
// Written by David Churchill (dave.churchill@gmail.com)
// Unauthorized copying of these files are strictly prohibited
// Distributed only for course work at Memorial University
// If you see this file online please contact email above