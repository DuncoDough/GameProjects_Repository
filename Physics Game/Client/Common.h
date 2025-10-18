#pragma once
#include <raylib.h>
#include <box2d/box2d.h>

//constants
const float PIXELS_PER_METER = 30.0f;
const float TIME_STEP = 1.0f / 60.0f;
const int VELOCITY_ITERATIONS = 6;
const int POSITION_ITERATIONS = 2;

// convert a Box2D position to a Raylib Vector2
static Vector2 getRaylibPosition(b2Body* body) {
	b2Vec2 pos = body->GetPosition();
	return Vector2{ pos.x * PIXELS_PER_METER, GetScreenHeight() - pos.y * PIXELS_PER_METER };
}

