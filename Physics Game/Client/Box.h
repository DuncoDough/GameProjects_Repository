#pragma once
#include <raylib.h>
#include <box2d/box2d.h>
#include <memory>

class Box
{
private:
    b2PolygonShape shape;
    float width, height;
    b2Body* body;
    Color color;

public:
    Box(std::shared_ptr<b2World> world, Vector2 centerPosition, float width, float height, b2BodyType bodyType, float angle, Color color,float linearDamping, float angularDamping, float restitution, float friction, float density);
    void Draw();
};

