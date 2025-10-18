#include "Game.h"
#include "Common.h"
#include "CustomDebugDraw.h"
#include "fmt/core.h"
#include <raymath.h>

#define SCALE 30.0F

Game::Game()
{

	//create physics world
	world = std::make_shared<b2World>(b2Vec2(0.0f, -10.0f));//gravity vector (0, -10) means gravity is pointing down with a magnitude of 10
	world->SetContactListener(this);

	// Common box shape for walls
	b2PolygonShape wallShape;
	wallShape.SetAsBox(0.5f, 15.0f); // Vertical walls (1m wide, 30m tall)

	// LEFT wall
	b2BodyDef leftWallDef;
	leftWallDef.position.Set(-0.5f, 15.0f); // X = -0.5 to place edge on screen
	b2Body* leftWall = world->CreateBody(&leftWallDef);
	leftWall->CreateFixture(&wallShape, 0.0f);

	// RIGHT wall
	b2BodyDef rightWallDef;
	rightWallDef.position.Set(34.5f, 15.0f); // X = 1024px / 30 + 0.5 = 34.5m
	b2Body* rightWall = world->CreateBody(&rightWallDef);
	rightWall->CreateFixture(&wallShape, 0.0f);

	// TOP wall (horizontal)
	b2PolygonShape topShape;
	topShape.SetAsBox(16.0f, 0.5f); // 32m wide × 1m tall

	b2BodyDef topWallDef;
	topWallDef.position.Set(16.0f, 29.5f); // Y = near top of screen (800px / 30)
	b2Body* topWall = world->CreateBody(&topWallDef);
	topWall->CreateFixture(&topShape, 0.0f);

	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0.0f, 1.0f);
	b2Body* groundBody = world->CreateBody(&groundBodyDef);

	b2PolygonShape groundBox;
	groundBox.SetAsBox(16.0f, 0.5);
	groundBody->CreateFixture(&groundBox, 0.f);

	b2BodyDef goalDef;
	goalDef.position.Set(30.5f, 5.0f);
	b2Body* goal = world->CreateBody(&goalDef);

	b2PolygonShape goalShape;
	goalShape.SetAsBox(1.5f, 0.5f);

	b2FixtureDef goalFixture;
	goalFixture.shape = &goalShape;
	goalFixture.isSensor = true;

	b2Fixture* fixture = goal->CreateFixture(&goalFixture);
	fixture->SetUserData((void*)1);

	background = LoadTexture("images/background.png");

	InitAudioDevice();
	scoreSound = LoadSound("images/score.mp3");
	missSound = LoadSound("images/miss.mp3");

	// Create an instance of the custom debug drawing class
	debugDraw = std::make_shared<CustomDebugDraw>();
	debugDraw->DrawSolidOrOutline(CustomDebugDraw::DRAW_OUTLINE); 
	debugDraw->SetFlags(b2Draw::e_shapeBit);

	// Set the debug drawing object for the Box2D world
	world->SetDebugDraw(debugDraw.get()); 	
		
}
void Game::Update()
{
	world->Step(TIME_STEP, VELOCITY_ITERATIONS, POSITION_ITERATIONS);


	if (IsKeyPressed(KEY_R)) {
		misses = 0;
		score = 0;
		gameOver = false;
		gameWon = false;

		for (const Ball& ball : balls) {
			world->DestroyBody(ball.body);
		}
		balls.clear();
	}

	if (gameOver || gameWon) return;

	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		dragStart = GetMousePosition();
		isDragging = true;
	}

	if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && isDragging) {
		Vector2 dragEnd = GetMousePosition();
		Vector2 forceVec = Vector2Subtract(dragStart, dragEnd);

		float x = dragStart.x / SCALE;
		float y = (GetScreenHeight() - dragStart.y) / SCALE;
	
		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position.Set(x, y);

		b2Body* body = world->CreateBody(&bodyDef);

		b2CircleShape circleShape;
		circleShape.m_radius = 0.3f;

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &circleShape;
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 0.3f;
		fixtureDef.restitution = 0.6f; // bounce on it craaazzyy stylee

		body->CreateFixture(&fixtureDef);

		b2Vec2 impulse = b2Vec2(forceVec.x / 10.0f, -forceVec.y / 10.0f);
		body->ApplyLinearImpulseToCenter(impulse, true);

		balls.push_back({body, false});

		isDragging = false;

	}

	for (auto it = balls.begin(); it != balls.end();)
	{
		b2Vec2 pos = it->body->GetPosition();

		if (pos.y < 0.3f) {
			PlaySound(missSound);
			misses++;
			if (misses >= 5) {
				gameOver = true;
			}
			it = balls.erase(it);
		}
		else 
		{
			++it;
		}
	}

	for (auto it = balls.begin(); it != balls.end();) {
		if (it->scored) {
			world->DestroyBody(it->body);
			it = balls.erase(it);
		}
		else {
			++it;
		}
	}

	balls.erase(std::remove_if(balls.begin(), balls.end(), [](const Ball& b) {
		return b.scored;
		}), balls.end());

}

void Game::BeginContact(b2Contact* contact)
{

	

	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();

	if (fixtureA->GetUserData() == (void*)1 || fixtureB->GetUserData() == (void*)1)
	{
		for (Ball& ball : balls) {
			if (ball.body == fixtureA->GetBody() || ball.body == fixtureB->GetBody()) {
				if (!ball.scored) {
					ball.scored = true;
					PlaySound(scoreSound);
					score++;

					

					if (score >= 5) {
						gameWon = true;
					}
				}
				break;
			}
		}
	}
}

void Game::Draw()
{
	//enable/disable debug drawing
	world->DrawDebugData(); 

	DrawTexture(background, 0, 0, WHITE);

	if (isDragging) {
		Vector2 mousePos = GetMousePosition();
		DrawLineV(dragStart, mousePos, RED);
	}

	for (const Ball& ball : balls) {
		b2Vec2 pos = ball.body->GetPosition();
		Vector2 screenPos = {
			pos.x * 30.0F,
			GetScreenHeight() - (pos.y * 30.0F)
		};
		DrawCircleV(screenPos, 0.3f * 30.0F, RED);
	}

	DrawRectangle(0, GetScreenHeight() - (int)(1.0f * 30.0f), 1024, (int)(0.5f * 30.0f), DARKGRAY);
	DrawRectangle(0, 0, 1024, (int)(0.5f * 30.0f), GRAY);
	DrawRectangle(0, 0, (int)(0.5f * 30.0f),800, GRAY);
	DrawRectangle(1024 - (int)(0.5f * 30.0f), 0, (int)(0.5f * 30.0f),800,GRAY);

	DrawText(TextFormat("Score: %d", score), 20, 20, 30, DARKBLUE);
	DrawText(TextFormat("Misses: %d", misses), 20, 60, 30, MAROON);


	DrawRectangle((int)(29.0f * SCALE), GetScreenHeight() - (int)(2.5f * SCALE),
		(int)(3.0f * SCALE), (int)(1.0f * SCALE), GREEN);

	if (gameOver) {
		DrawText("GAME OVER", GetScreenWidth() / 2 - 150, GetScreenHeight() / 2 - 20, 40, RED);
		DrawText("Press R to Restart", GetScreenWidth() / 2 - 170, GetScreenHeight() / 2 + 30, 30, DARKGRAY);
	}

	if (gameWon) {
		DrawText("YOU WIN!!", GetScreenWidth() / 2 - 150, GetScreenHeight() / 2 - 20, 40, RED);
		DrawText("Press R to Restart", GetScreenWidth() / 2 - 170, GetScreenHeight() / 2 + 30, 30, DARKGRAY);
	}
	
}

// Destructor
Game::~Game()
{
	UnloadTexture(background);
	UnloadSound(scoreSound);
	UnloadSound(missSound);
	CloseAudioDevice();
}

