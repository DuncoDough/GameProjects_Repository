#pragma once
#include <raylib.h>
#include <box2d/box2d.h>
#include <memory>
#include "Common.h"
#include "raymath.h"
class TexturedBox
{
private:   
    b2PolygonShape shape;
    b2Body* body;
    Texture2D texture;
    Vector2 size;
public:
    TexturedBox(std::shared_ptr<b2World> world, Vector2 centerPosition, Vector2 size, float angleDegrees, b2BodyType bodyType, Texture2D texture, float linearDamping, float angularDamping, float restitution, float friction, float density);
    virtual void Draw();
};

