#pragma once
#include <raylib.h>
#include <box2d/box2d.h>
#include <memory>
#include "CustomDebugDraw.h"
#include <vector>




class Game : public b2ContactListener
{

	Vector2 dragStart;
	bool isDragging = false;

	int score = 0;
	bool gameWon = false;

	Sound scoreSound;
	Sound missSound;

	int misses = 0;
	bool gameOver = false;

	Texture2D background;

	struct Ball
	{
		b2Body* body;
		bool scored = false;
	};

	std::vector<Ball> balls;

private:
	//Physics world
	std::shared_ptr<b2World> world;
	
	//Debug draw
	std::shared_ptr<CustomDebugDraw> debugDraw;


public:
	Game();
	~Game();
	void Update();
	void Draw();
	void BeginContact(b2Contact* contact) override;



	
};

