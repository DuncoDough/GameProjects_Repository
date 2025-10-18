#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <raylib.h>
#include <Game.h>
#include <memory>



int main() {
    // Initialize the Raylib window
    const int screenWidth = 1024;
    const int screenHeight = 800;
    InitWindow(screenWidth, screenHeight, "Task2: Pyhsics Game, 'BasketBall'");

    // Set target framerate (matching Box2D update rate)
    SetTargetFPS(60);

    // Create the game    
    std::shared_ptr<Game> game = std::make_shared<Game>();

    // Game loop
    while (!WindowShouldClose()) {
        game->Update();
    
    	// Start drawing with Raylib
        BeginDrawing();
        ClearBackground(RAYWHITE);
        game->Draw();
    

     
      
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
