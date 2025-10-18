#include "TexturedCircle.h"


TexturedCircle::TexturedCircle(std::shared_ptr<b2World> world, Vector2 centerPosition, Texture2D texture, b2BodyType bodyType, float linearDamping, float angularDamping, float restitution, float friction, float density) :texture(texture)
{
    this->shape.m_radius = std::max(texture.width, texture.height) / 2.0f / PIXELS_PER_METER; // Set the radius of the circle to the larger of the two dimensions of the texture

    b2BodyDef bodyDef;
    bodyDef.type = bodyType;
    bodyDef.position.Set(centerPosition.x / PIXELS_PER_METER, (GetScreenHeight() - centerPosition.y) / PIXELS_PER_METER);
    //without damping the ball will roll/bounce forever
    bodyDef.linearDamping = linearDamping;
    bodyDef.angularDamping = angularDamping;

    body = world->CreateBody(&bodyDef);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = density;
    fixtureDef.friction =friction;
    fixtureDef.restitution = restitution;

    body->CreateFixture(&fixtureDef);
}


void TexturedCircle::Draw()
{
    Vector2 position = getRaylibPosition(body); // Get the position in Raylib coordinates
    Vector2 origin = { texture.width / 2.0f, texture.height / 2.0f };
    float angleDegrees = body->GetAngle() * RAD2DEG;

    // Create a transformation matrix for rotation
    Matrix rotationMatrix = MatrixRotateZ(angleDegrees * DEG2RAD);

    // Apply the rotation to the texture's origin
    Vector2 rotatedOrigin = Vector2Transform(origin, rotationMatrix);

    // Draw the texture with the rotated origin and position adjustment
    DrawTextureEx(texture, { position.x - rotatedOrigin.x, position.y - rotatedOrigin.y }, angleDegrees, 1.0f, WHITE);
}

Vector2 const TexturedCircle::GetRaylibPosition()
{
    return getRaylibPosition(body);
}

float const TexturedCircle::GetRaylibRadius()
{
    return shape.m_radius * PIXELS_PER_METER;
}
