#include "TexturedBox.h"

TexturedBox::TexturedBox(std::shared_ptr<b2World> world, Vector2 centerPosition, Vector2 size, float angleDegrees, b2BodyType bodyType, Texture2D texture, float linearDamping, float angularDamping,float restitution, float friction, float density) :texture(texture), size(size)
{
    // Define the body
    b2BodyDef bodyDef;
    bodyDef.type = bodyType;
    bodyDef.position.Set(centerPosition.x / PIXELS_PER_METER, (GetScreenHeight() - centerPosition.y) / PIXELS_PER_METER);
    bodyDef.angle = angleDegrees * DEG2RAD; // Convert degrees to radians for Box2D
    bodyDef.linearDamping = linearDamping;
    bodyDef.angularDamping = angularDamping;

    // Create the body in the Box2D world
    body = world->CreateBody(&bodyDef);

    // Define the shape as a box
    shape.SetAsBox((size.x / 2.0f) / PIXELS_PER_METER, (size.y / 2.0f) / PIXELS_PER_METER);

    // Define fixture properties
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = density;
    fixtureDef.friction = friction;
    fixtureDef.restitution = restitution;   

    // Create the fixture on the body
    body->CreateFixture(&fixtureDef);
}

void TexturedBox::Draw()
{
    Vector2 position = getRaylibPosition(body); // Get the position in Raylib coordinates
    float angleDegrees = -(body->GetAngle() * RAD2DEG); // Convert Box2D angle (radians) to degrees

    // Calculate the origin for rotation (center of the texture)
    Vector2 origin = { size.x / 2, size.y / 2 };

    // Create a transformation matrix for rotation
    Matrix rotationMatrix = MatrixRotateZ(angleDegrees * DEG2RAD);

    // Apply the rotation to the texture's origin
    Vector2 rotatedOrigin = Vector2Transform(origin, rotationMatrix);

    // Draw the texture with the rotated origin and position adjustment
    DrawTextureEx(texture, { position.x - rotatedOrigin.x, position.y - rotatedOrigin.y }, angleDegrees, 1.0f, WHITE);
}
