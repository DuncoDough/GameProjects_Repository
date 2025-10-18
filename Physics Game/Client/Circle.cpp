#include "Circle.h"
#include "Common.h"

Circle::Circle(std::shared_ptr<b2World> world, Vector2 centerPosition, float radius, b2BodyType bodyType, Color color, float linearDamping, float angularDamping, float restitution, float friction, float density)
    : radius(radius), color(color) {
 

    b2BodyDef bodyDef;
    bodyDef.type = bodyType;
    bodyDef.position.Set(centerPosition.x / PIXELS_PER_METER, (GetScreenHeight() - centerPosition.y) / PIXELS_PER_METER);
    //without damping the ball will roll/bounce forever
    bodyDef.linearDamping = linearDamping;
    bodyDef.angularDamping = angularDamping;


    body = world->CreateBody(&bodyDef);
    shape.m_radius = radius / PIXELS_PER_METER;

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = density; // Adjust density
    fixtureDef.friction = friction; // Adjust friction
    fixtureDef.restitution = restitution;
    
    body->CreateFixture(&fixtureDef);
}

void Circle::Draw() {
	Vector2 position = getRaylibPosition(body);
	DrawCircleV(position, radius, color);
}