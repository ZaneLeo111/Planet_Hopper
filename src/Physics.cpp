
#include "Physics.h"
#include "Components.h"
#include <math.h>

Vec2 Physics::GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
    
    if (!a->hasComponent<CBoundingBox>() || !b->hasComponent<CBoundingBox>()) return Vec2(0, 0);

    CTransform& atrans = a->getComponent<CTransform>();
    CTransform& btrans = b->getComponent<CTransform>();
    CBoundingBox& abox = a->getComponent<CBoundingBox>();
    CBoundingBox& bbox = b->getComponent<CBoundingBox>();

    Vec2 delta = Vec2( abs(atrans.pos.x - btrans.pos.x), abs(atrans.pos.y - btrans.pos.y) );
    Vec2 overlap = abox.halfSize + bbox.halfSize - delta;

    return overlap;
}

Vec2 Physics::GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
             
    if (!a->hasComponent<CBoundingBox>() || !b->hasComponent<CBoundingBox>()) return Vec2(0, 0);

    CTransform& atrans = a->getComponent<CTransform>();
    CTransform& btrans = b->getComponent<CTransform>();
    CBoundingBox& abox = a->getComponent<CBoundingBox>();
    CBoundingBox& bbox = b->getComponent<CBoundingBox>();

    Vec2 delta = Vec2(abs(atrans.prevPos.x - btrans.prevPos.x), abs(atrans.prevPos.y - btrans.prevPos.y));
    Vec2 overlap = abox.halfSize + bbox.halfSize - delta;

    return overlap;
}

bool Physics::IsInside(const Vec2& pos, std::shared_ptr<Entity> e)
{
    auto ePos = e->getComponent<CTransform>().pos;
    Vec2 size = (e->hasComponent<CBoundingBox>() ? 
        e->getComponent<CBoundingBox>().size : 
        e->getComponent<CAnimation>().animation.getSize());

    float dx = fabs(pos.x - ePos.x);
    float dy = fabs(pos.y - ePos.y);

    return ((dx <= size.x / 2) && (dy <= size.y / 2));
}

Intersect Physics::LineIntersect(const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& d)
{
    Vec2 r = (b - a);
    Vec2 s = (d - c);
    float rxs = r * s;
    Vec2 cma = (c - a);
    float t = (cma * s) / rxs;
    float u = (cma * r) / rxs;
    if (t >= 0 && t <= 1 && u >= 0 && u <= 1)
    {
        return { true, Vec2(a.x + t * r.x, a.y + t * r.y) };
    }

    return { false, Vec2(0,0) };
}

bool Physics::EntityIntersect(const Vec2& a, const Vec2& b, std::shared_ptr<Entity> e)
{
    auto& bBox = e->getComponent<CBoundingBox>();
    auto& pos = e->getComponent<CTransform>().pos;
    Vec2 e1 = { pos.x - bBox.halfSize.x, pos.y - bBox.halfSize.y };
    Vec2 e2 = { pos.x + bBox.halfSize.x, pos.y - bBox.halfSize.y };
    Vec2 e3 = { pos.x + bBox.halfSize.x, pos.y + bBox.halfSize.y };
    Vec2 e4 = { pos.x - bBox.halfSize.x, pos.y + bBox.halfSize.y };

    return LineIntersect(a, b, e1, e2).result || LineIntersect(a, b, e2, e3).result || LineIntersect(a, b, e3, e4).result || LineIntersect(a, b, e4, e1).result;
}

