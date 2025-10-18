#pragma once
#include <raylib.h>
#include <box2d/box2d.h>
#include <memory>

class Circle
{
private: 
    b2CircleShape shape;
    float radius;
    float bounciness;
    b2Body* body;
    Color color;

public:
    Circle(std::shared_ptr<b2World> world, Vector2 centerPosition, float radius, b2BodyType bodyType, Color color, float linearDamping, float angularDamping, float restitution, float friction, float density);
    void Draw();
};

