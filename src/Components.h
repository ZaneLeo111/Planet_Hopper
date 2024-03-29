
#pragma once

#include "Animation.h"
#include "Assets.h"

class Component
{
public:
    bool has = false;
};

class CDraggable : public Component
{
public:
    bool dragging = false;
    CDraggable() {}
    CDraggable(bool d)
        : dragging(d) {}
};

class CClickable : public Component
{
public:
    bool clicking = false;
    CClickable() {}
};
                                                     
class CTransform : public Component 
{
public:
    Vec2 originalPos  = { 0.0, 0.0 };
    Vec2 pos          = { 0.0, 0.0 };
    Vec2 prevPos      = { 0.0, 0.0 };
    Vec2 scale        = { 1.0, 1.0 };
    Vec2 prevScale    = { 1.0, 1.0 };
    Vec2 velocity     = { 0.0, 0.0 };
    float scrollFactor = 0;
    float angle     = 0;
    float prevAngle = 0;
                                                     
    CTransform() {}
    CTransform(const Vec2 & p)
        : pos(p), prevPos(p) {}
    CTransform(const Vec2& p, const Vec2& sc)
        : pos(p), prevPos(p), scale(sc) {}
    CTransform(const Vec2& p, const Vec2& sc, float sf)
        : pos(p), prevPos(p), originalPos(p), scale(sc), scrollFactor(sf) {}
    CTransform(const Vec2 & p, const Vec2 & sp, const Vec2 & sc, float a)
        : pos(p), prevPos(p), velocity(sp), scale(sc), angle(a) {}

};

class CStatusEffect : public Component
{
public:
    std::string currentEffect = "NONE";
    int frameCreated = 0;
    int duration = 0;
    CStatusEffect() {}
    CStatusEffect(std::string effect, int frame, int dur)
        : currentEffect(effect), frameCreated(frame), duration(dur) {}
};

class CWeapon : public Component
{
public:
    std::string currentWeapon  = "Raygun";
    std::string attackState    = "Idle";
    int duration               = 30;
    int frameCreated           = 0;
    int lastFiredLauncher      = 0;
    int lastFiredRaygun        = 0;
    int lastFiredBomb          = 0;
    int lastFiredNpc           = 0;
    Vec2 target                = { 0.0, 0.0 };
    CWeapon() {}
    CWeapon(std::string weapon) 
        : currentWeapon(weapon) {}
    CWeapon(std::string weapon, std::string atk, int dur, int created)
        : currentWeapon(weapon), attackState(atk), duration(dur), frameCreated(created) {}
};

class CLifeSpan : public Component
{
public:
    int lifespan = 0;
    int frameCreated = 0;
    CLifeSpan() {}
    CLifeSpan(int duration, int frame) 
        : lifespan(duration), frameCreated(frame) {}
};

class CBulletTimer : public Component
{
public:
    int interval = 0;
    int frameCreated = 0;
    CBulletTimer() {}
    CBulletTimer(int i, int frame)
        : interval(i), frameCreated(frame) {}
};

class CDamage : public Component
{
public:
    int damage = 1;
    CDamage() {}
    CDamage(int d)
        : damage(d) {}
};

class CHealth : public Component
{
public:
    int max = 1;
    int current = 1;
    CHealth() {}
    CHealth(int m, int c)
        : max(m), current(c) {}
};

class CInvincibility : public Component
{
public:
    int iframes = 0;
    CInvincibility() {}
    CInvincibility(int f)
        : iframes(f) {}
};

class CInput : public Component
{
public:
    bool up         = false;
    bool down       = false;
    bool left       = false;
    bool right      = false;
    bool shoot      = false;
    bool canShoot   = true;
    bool canJump    = true;
    bool sliding    = false;
    // mouse inputs
    bool click1 = false;
    bool click2 = false;
    bool click3 = false;

    CInput() {}
};
                                                     
class CBoundingBox : public Component
{
public:
    Vec2 size;
    Vec2 halfSize;
    bool blockMove = false;
    bool blockVision = false;
    CBoundingBox() {}
    CBoundingBox(const Vec2& s)
        : size(s), halfSize(s.x / 2, s.y / 2) {}
    CBoundingBox(const Vec2& s, bool m, bool v)
        : size(s), blockMove(m), blockVision(v), halfSize(s.x / 2, s.y / 2) {}
};

class CAnimation : public Component
{
public:
    Animation animation;
    bool repeat = false;
    CAnimation() {}
    CAnimation(const Animation & animation, bool r)
        : animation(animation), repeat(r) {}
};

class CGravity : public Component
{
public:
    float gravity = 0;
    bool flipped = false;
    CGravity() {}
    CGravity(float g) : gravity(g) {}
};

class CState : public Component
{
public:
    std::string state = "jumping";
    CState() {}
    CState(const std::string & s) : state(s) {}
};

class CFollowPlayer : public Component
{
public:
    Vec2 home = { 0, 0 };
    float speed = 0;
    CFollowPlayer() {}
    CFollowPlayer(Vec2 p, float s)
        : home(p), speed(s) {}

};

class CPatrol : public Component
{
public:
    std::vector<Vec2> positions;
    size_t currentPosition = 0;
    float speed = 0;
    CPatrol() {}
    CPatrol(std::vector<Vec2>& pos, float s) : positions(pos), speed(s) {}
};

class CLevelStatus : public Component
{
public:
    bool completed = false;
    bool unlocked  = false;

    CLevelStatus() {}
    CLevelStatus(bool i) : unlocked(i) {}
};

class CButton : public Component
{
public:
    std::string value;

    CButton() {}
    CButton(const std::string & s) : value(s) {}
};

class CJump : public Component
{
public:
    float lastJumped = 0;
    float jump = 0;
    CJump() {}
    CJump(float j)
        : jump(j) {}
};

