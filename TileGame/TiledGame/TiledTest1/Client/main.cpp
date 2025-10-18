#include "Includes.h"
#include "tileson.hpp"
#include <memory>

const int NUM_FRAMES_IDLE = 9;
const int NUM_FRAMES_RUN = 7;

const int ENEMY_FRAMES_IDLE = 9;
const int ENEMY_FRAMES_RUN = 9;

Texture2D idleFrames[NUM_FRAMES_IDLE];
Texture2D runFrames[NUM_FRAMES_RUN];

Texture2D enemyIdleFrames[ENEMY_FRAMES_IDLE];
Texture2D enemyRunFrames[ENEMY_FRAMES_RUN];

Texture2D potionTexture;

Texture2D cointTexture;
std::vector<Vector2> coinPositions = { {200, 150}, {400, 175}, {200, 700},{300,700},{250,700} };
std::vector<bool> coinsCollected(coinPositions.size(), false);
int playerScore = 0;

Texture2D trapTexture;
std::vector<Vector2> trapPositions = { {700, 500}, {700, 400} };

enum AnimationState { IDLE, RUN };
AnimationState currentState = IDLE;

class Collidable {
public:
    std::string tag;
    Collidable(const std::string& tag) : tag(tag) {}
    virtual bool CheckCollision(const Rectangle& playerRect) = 0;
    virtual ~Collidable() = default;
};

class CollidableRectangle : public Collidable {
private:
    Rectangle rectangle;
public:
    CollidableRectangle(const Rectangle& rect, const std::string& tag = "")
        : Collidable(tag), rectangle(rect) {
    }

    bool CheckCollision(const Rectangle& playerRect) override {
        return CheckCollisionRecs(this->rectangle, playerRect);
    }
};

class CollidablePolygon : public Collidable {
private:
    std::vector<Vector2> polygonPoints;

    bool CustomCheckCollisionPointPoly(Vector2 point, Vector2* points, int pointCount) {
        bool inside = false;
        if (pointCount > 2) {
            for (int i = 0, j = pointCount - 1; i < pointCount; j = i++) {
                if ((points[i].y > point.y) != (points[j].y > point.y) &&
                    (point.x < (points[j].x - points[i].x) * (point.y - points[i].y) /
                        (points[j].y - points[i].y) + points[i].x)) {
                    inside = !inside;
                }
            }
        }
        return inside;
    }

    bool CheckCollisionRectPoly(Rectangle rect, std::vector<Vector2>& polygonPoints) {
        Vector2 corners[4] = {
            {rect.x, rect.y},
            {rect.x + rect.width, rect.y},
            {rect.x, rect.y + rect.height},
            {rect.x + rect.width, rect.y + rect.height}
        };

        for (const auto& corner : corners) {
            if (CustomCheckCollisionPointPoly(corner, polygonPoints.data(), polygonPoints.size())) {
                return true;
            }
        }

        for (const auto& point : polygonPoints) {
            if (CheckCollisionPointRec(point, rect)) {
                return true;
            }
        }

        return false;
    }

public:
    CollidablePolygon(const std::vector<Vector2>& points, const std::string& tag = "")
        : Collidable(tag), polygonPoints(points) {
    }

    bool CheckCollision(const Rectangle& playerRect) override {
        return CheckCollisionRectPoly(playerRect, polygonPoints);
    }
};

void LoadAnimations() {
    potionTexture = LoadTexture("images/hp_potion.png");
    cointTexture = LoadTexture("images/coin.png");
    trapTexture = LoadTexture("images/trap.png");
    for (int i = 0; i < NUM_FRAMES_IDLE; i++) {
        char fileName[100];
        sprintf(fileName, "images/Idle/MC/Idle_%d.png", i);
        idleFrames[i] = LoadTexture(fileName);
    }
    for (int i = 0; i < NUM_FRAMES_RUN; i++) {
        char fileName[100];
        sprintf(fileName, "images/Run/MC/Run_%d.png", i);
        runFrames[i] = LoadTexture(fileName);
    }
    for (int i = 0; i < ENEMY_FRAMES_IDLE; i++) {
        char fileName[100];
        sprintf(fileName, "images/Idle/Enemy/Idle_%d.png", i);
        enemyIdleFrames[i] = LoadTexture(fileName);
    }
    for (int i = 0; i < ENEMY_FRAMES_RUN; i++) {
        char fileName[100];
        sprintf(fileName, "images/Run/Enemy/Run_%d.png", i);
        enemyRunFrames[i] = LoadTexture(fileName);
    }
    
}

void UnloadAnimations() {
    UnloadTexture(potionTexture);
    UnloadTexture(cointTexture);
    UnloadTexture(trapTexture);
    for (int i = 0; i < NUM_FRAMES_IDLE; i++) UnloadTexture(idleFrames[i]);
    for (int i = 0; i < NUM_FRAMES_RUN; i++) UnloadTexture(runFrames[i]);
    for (int i = 0; i < ENEMY_FRAMES_IDLE; i++) UnloadTexture(enemyIdleFrames[i]);
    for (int i = 0; i < ENEMY_FRAMES_RUN; i++) UnloadTexture(enemyRunFrames[i]);
    
}

struct Enemy {
    Vector2 position;
    Rectangle rect;
    bool isChasing;
    float animTimer;
    int currentFrame;

    Enemy(Vector2 startPos)
        : position(startPos), rect{ startPos.x, startPos.y, 32, 32 }, isChasing(false), animTimer(0.0f), currentFrame(0) {
    }
};

const int npcSize = 32;
float npcSpeed = 75.0f;
float chaseRadius = 200.0f;
float stopRadius = 150.0f;

std::vector<Enemy> enemies = {
    Enemy({900, 400}),
    Enemy({800, 300}),
    Enemy({950, 500})
};

bool gameOver = false;
int playerHealth = 3;
float damageCooldown = 1.0f;
float damageTimer = 0.0f;

bool youWin = false;

Vector2 potionPosition = { 500,800 };
const int potionSize = 32;
Rectangle potionRect = { potionPosition.x, potionPosition.y, potionSize, potionSize };
bool potionCollected = false;
const int MAX_HEALTH = 20;

int main() {
    const float mapScale = 2.0f;
    int screenWidth = 1280;
    int screenHeight = 960;
    float scaleFactor = 2.0f;

    InitWindow(screenWidth, screenHeight, "Task1");
    LoadAnimations();
    SetTargetFPS(60);

    tson::Tileson loader;
    auto map = loader.parse("map/Dungeon.tmj");
    if (map->getStatus() != tson::ParseStatus::OK) return -1;

    Texture2D tilesetTexture = { 0 };
    if (!map->getTilesets().empty())
        tilesetTexture = LoadTexture(map->getTilesets()[0].getImagePath().u8string().c_str());
    if (tilesetTexture.id == 0) return -1;

    std::vector<std::unique_ptr<Collidable>> collidables;
    for (auto& layer : map->getLayers()) {
        if (layer.getType() == tson::LayerType::TileLayer) {
            for (auto& [pos, tileObject] : layer.getTileObjects()) {
                if (tileObject.getTile() == nullptr) continue;
                if (tileObject.getTile()->getObjectgroup().getObjects().size() > 0 &&
                    tileObject.getTile()->getProperties().hasProperty("collidable") &&
                    tileObject.getTile()->getProperties().getProperty("collidable")->getValue<bool>()) {

                    std::string tag = "";
                    if (tileObject.getTile()->getProperties().hasProperty("tag"))
                        tag = tileObject.getTile()->getProperties().getProperty("tag")->getValue<std::string>();

                    for (auto& obj : tileObject.getTile()->getObjectgroup().getObjects()) {
                        if (obj.getPolygons().empty()) {
                            Rectangle colliderRect = {
                                (float)(tileObject.getPosition().x + obj.getPosition().x) * scaleFactor,
                                (float)(tileObject.getPosition().y + obj.getPosition().y) * scaleFactor,
                                (float)obj.getSize().x * scaleFactor,
                                (float)obj.getSize().y * scaleFactor
                            };
                            collidables.push_back(std::make_unique<CollidableRectangle>(colliderRect, tag));
                        }
                        else {
                            std::vector<Vector2> scaledPolygonPoints;
                            for (const auto& point : obj.getPolygons()) {
                                scaledPolygonPoints.push_back({
                                    (float)(tileObject.getPosition().x + obj.getPosition().x + point.x) * scaleFactor,
                                    (float)(tileObject.getPosition().y + obj.getPosition().y + point.y) * scaleFactor
                                    });
                            }
                            collidables.push_back(std::make_unique<CollidablePolygon>(scaledPolygonPoints, tag));
                        }
                    }
                }
            }
        }
    }

    Vector2 playerPosition = { 100, 100 };
    const int playerSize = 32;
    Rectangle playerRect = { playerPosition.x, playerPosition.y, playerSize, playerSize };
    float playerSpeed = 150.0f;
    float animTimer = 0.0f;
    int currentFrame = 0;
    const float frameChangeRate = 0.1f;

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        Vector2 potentialPosition = playerPosition;
        bool moving = false;

        if (IsKeyDown(KEY_RIGHT)) { potentialPosition.x += playerSpeed * deltaTime; moving = true; }
        if (IsKeyDown(KEY_LEFT)) { potentialPosition.x -= playerSpeed * deltaTime; moving = true; }
        if (IsKeyDown(KEY_DOWN)) { potentialPosition.y += playerSpeed * deltaTime; moving = true; }
        if (IsKeyDown(KEY_UP)) { potentialPosition.y -= playerSpeed * deltaTime; moving = true; }
        currentState = moving ? RUN : IDLE;

        Rectangle potentialPlayerRect = { potentialPosition.x, potentialPosition.y, playerRect.width, playerRect.height };
        bool collisionDetected = false;
        for (const auto& collidable : collidables) {
            if (collidable->CheckCollision(potentialPlayerRect)) {
                collisionDetected = true;
                break;
            }
        }
        if (!collisionDetected) playerPosition = potentialPosition;

        if (!potionCollected && CheckCollisionRecs(playerRect, potionRect)) {
            potionCollected = true;
            playerHealth += 7;
            if (playerHealth > MAX_HEALTH) playerHealth = MAX_HEALTH;
            std::cout << "Potion collected! Health increased by 7." << std::endl;
        }

        animTimer += deltaTime;
        if (animTimer >= frameChangeRate) {
            animTimer = 0.0f;
            currentFrame++;
            if (currentState == IDLE && currentFrame >= NUM_FRAMES_IDLE) currentFrame = 0;
            else if (currentState == RUN && currentFrame >= NUM_FRAMES_RUN) currentFrame = 0;
        }

        playerRect.x = playerPosition.x;
        playerRect.y = playerPosition.y;

        for (const auto& trapPos : trapPositions) {
            Rectangle trapRect = { trapPos.x, trapPos.y, 32, 32 };
            if (CheckCollisionRecs(playerRect, trapRect) && damageTimer >= damageCooldown && !gameOver) {
                playerHealth--;
                damageTimer = 0.0f;
                std::cout << "Hit by a trap! Health: " << playerHealth << std::endl;
                if (playerHealth <= 0) {
                    gameOver = true;
                }
            }
        }


        for (size_t i = 0; i < coinPositions.size(); i++) {
            if (!coinsCollected[i]) {
                Rectangle coinRect = { coinPositions[i].x, coinPositions[i].y, 32, 32 };
                if (CheckCollisionRecs(playerRect, coinRect)) {
                    coinsCollected[i] = true;
                    playerScore += 1;
                    std::cout << "Coin collected! Score: " << playerScore << std::endl;
                }
            }
        }

        bool allCoinsCollected = true;
        for (bool collected : coinsCollected) {
            if (!collected) {
                allCoinsCollected = false;
                break;
            }
        }
        if (allCoinsCollected && !youWin) 
        {
            youWin = true;
            std::cout << "You Win! All coins collected!" << std::endl;
        }


        damageTimer += deltaTime;
        for (Enemy& enemy : enemies) {
            float distance = Vector2Distance(playerPosition, enemy.position);
            if (!enemy.isChasing && distance < chaseRadius) enemy.isChasing = true;
            else if (enemy.isChasing && distance > stopRadius) enemy.isChasing = false;

            if (enemy.isChasing && !gameOver) {
                Vector2 direction = Vector2Normalize(Vector2Subtract(playerPosition, enemy.position));
                Vector2 potentialPos = Vector2Add(enemy.position, Vector2Scale(direction, npcSpeed * deltaTime));
                Rectangle potentialRect = { potentialPos.x, potentialPos.y, (float)npcSize, (float)npcSize };
                bool npcCollision = false;
                for (const auto& collidable : collidables) {
                    if (collidable->CheckCollision(potentialRect)) {
                        npcCollision = true;
                        break;
                    }
                }
                if (!npcCollision) enemy.position = potentialPos;
            }

            enemy.rect.x = enemy.position.x;
            enemy.rect.y = enemy.position.y;

            if (CheckCollisionRecs(playerRect, enemy.rect) && damageTimer >= damageCooldown && !gameOver) {
                playerHealth--;
                damageTimer = 0.0f;
                if (playerHealth <= 0) gameOver = true;
            }

            enemy.animTimer += deltaTime;
            if (enemy.animTimer >= frameChangeRate) {
                enemy.animTimer = 0.0f;
                enemy.currentFrame++;
                if (enemy.isChasing && enemy.currentFrame >= ENEMY_FRAMES_RUN)
                    enemy.currentFrame = 0;
                else if (!enemy.isChasing && enemy.currentFrame >= ENEMY_FRAMES_IDLE)
                    enemy.currentFrame = 0;
            }
        }

        BeginDrawing();
        ClearBackground(WHITE);

        for (auto& layer : map->getLayers()) {
            if (layer.getType() == tson::LayerType::TileLayer) {
                for (auto& [pos, tileObject] : layer.getTileObjects()) {
                    tson::Rect drawingRect = tileObject.getDrawingRect();
                    Rectangle destRect = {
                        (float)tileObject.getPosition().x * scaleFactor,
                        (float)tileObject.getPosition().y * scaleFactor,
                        (float)map->getTileSize().x * scaleFactor,
                        (float)map->getTileSize().y * scaleFactor
                    };
                    Rectangle sourceRect = {
                        (float)drawingRect.x,
                        (float)drawingRect.y,
                        (float)drawingRect.width,
                        (float)drawingRect.height
                    };
                    DrawTexturePro(tilesetTexture, sourceRect, destRect, Vector2{ 0, 0 }, 0.0f, WHITE);
                }
            }
        }

        for (const auto& trapPos : trapPositions) {
            DrawTextureEx(trapTexture, trapPos, 0.0f, 0.5f, WHITE);
        }

        float scale = 0.1f;
        if (currentState == IDLE)
            DrawTextureEx(idleFrames[currentFrame], playerPosition, 0.0f, scale, WHITE);
        else if (currentState == RUN)
            DrawTextureEx(runFrames[currentFrame], playerPosition, 0.0f, scale, WHITE);

        for (size_t i = 0; i < coinPositions.size(); i++) {
            if (!coinsCollected[i]) {
                DrawTextureEx(cointTexture, coinPositions[i], 0.0f, 1.0f, WHITE);
            }
        }


        float enemyScale = 0.1f;
        for (const Enemy& enemy : enemies) {
            if (enemy.isChasing)
                DrawTextureEx(enemyRunFrames[enemy.currentFrame], enemy.position, 0.0f, enemyScale, WHITE);
            else
                DrawTextureEx(enemyIdleFrames[enemy.currentFrame], enemy.position, 0.0f, enemyScale, WHITE);
        }

        if (!potionCollected) {
            DrawTextureEx(potionTexture, potionPosition, 0.0f, 1.f, WHITE);
        }

        DrawText(TextFormat("Score: %d", playerScore), 20, 50, 20, YELLOW);
        if (youWin) {
            DrawText("You Win", screenWidth / 2 - 100, screenHeight / 2 - 20, 40, YELLOW);
            EndDrawing();
            continue;
        }

        DrawText(TextFormat("Health: %d", playerHealth), 20, 20, 20, DARKGREEN);
        if (gameOver) {
            DrawText("Game Over", screenWidth / 2 - 100, screenHeight / 2 - 20, 40, RED);
            EndDrawing();
            continue;
        }

        EndDrawing();
    }
    UnloadTexture(trapTexture);
    UnloadTexture(cointTexture);
    UnloadAnimations();
    UnloadTexture(tilesetTexture);
    CloseWindow();
    return 0;
}
