#include <iostream>
#include <raylib.h>
#include <vector>
#include <cmath>

const int screenWidth = 800;
const int screenHeight = 800;
bool running = true;

class UFO {
public:
    Vector2 pos;
    Texture2D ufoImg;
    int speedX = 7;
    int speedY = 7;
    int acceleration = 1;
    int jumpForce = 15;

    Rectangle rect;
    float immunityTime = 0.75f;
    float immunityTimer = 0.0f;

    UFO() {
        ufoImg = LoadTexture("ufo.png");
        pos.x = screenWidth / 2 - ufoImg.width / 2;
        pos.y = screenHeight / 2 - ufoImg.height / 2;
    }

    ~UFO() {
        UnloadTexture(ufoImg);
    }

    void Draw() {
        DrawTexture(ufoImg, pos.x, pos.y, WHITE);

        rect = { pos.x + 5, pos.y + 5, (float)ufoImg.width - 10, (float)ufoImg.height - 10 };
    }

    void Update() {
        Draw();

        if (immunityTimer > 0) {
            immunityTimer -= GetFrameTime();
        }

        speedY += acceleration;
        pos.y += speedY;

        if (IsMouseButtonPressed(0) || IsKeyPressed(KEY_SPACE)) {
            speedY = -jumpForce;
        } else if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
            pos.x -= speedX;
        } else if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
            pos.x += speedX;
        }
    }

    bool IsImmune() const {
        return immunityTimer > 0;
    }

    void ActivateImmunity() {
        immunityTimer = immunityTime;
    }
};

class Asteroid {
public:
    Vector2 pos;
    int width;
    int height;
    int topBottom[2] = {0, 1};
    float rotation;
    bool notCentered = true;

    Rectangle rect;

    Asteroid() {
        pos = {(float)GetRandomValue(-100, 900), (float)GetRandomValue(-100, 900)};
        width = GetRandomValue(30, 100);
        height = GetRandomValue(30, 100);
        rotation = GetRandomValue(0, 360);

        if (pos.x > 0 && pos.x < 800) {
            if (topBottom[GetRandomValue(0, 1) == 0]) {
                pos.y = -100;
            } else {
                pos.y = 900;
            }
        } else if (pos.y > 0 && pos.y < 800) {
            if (topBottom[GetRandomValue(0, 1) == 0]) {
                pos.x = -100;
            } else {
                pos.x = 900;
            }
        }
    }

    void Draw() {
        if (notCentered) {
            rect = { pos.x, pos.y, (float)width, (float)height };
            DrawRectanglePro({pos.x, pos.y, (float)width, (float)height},
                             {(float)width / 2, (float)height / 2}, rotation, WHITE);
        }
    }

    void Update() {
        if (notCentered) {
            Draw();
        }

        float targetX = (float)GetScreenWidth() / 2;
        float targetY = (float)GetScreenHeight() / 2;

        float directionX = targetX - pos.x;
        float directionY = targetY - pos.y;

        float distance = sqrt(directionX * directionX + directionY * directionY);

        if (distance > 0) {
            directionX /= distance;
            directionY /= distance;
        }

        const float threshold = 2.0f;

        if (distance <= threshold) {
            notCentered = false;
        } else {
            pos.x += 2 * directionX;
            pos.y += 2 * directionY;
        }
    }
};

class Health {
    public:
        int width = 50;
        int height = 50;

        Rectangle hp[5] = {{10, 10, (float)width, (float)height}, {(float)width + 60, 10, (float)width, (float)height}, {(float)width * 2 + 120, 10, (float)width, (float)height}, {(float)width * 3 + 180, 10, (float)width, (float)height}, {(float)width * 4 + 240, 10, (float)width, (float)height}};

        void Draw(int size) {
            for (int i = 0; i <= size; i++) {
                DrawRectangle(hp[i].x, hp[i].y, hp[i].width, hp[i].height, RED);
            }
        }
};

int main() {
    const int screenWidth = 800;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "UFO Game");

    UFO player;
    std::vector<Asteroid> asteroids;
    Health health;

    float startTime = GetTime();
    float elapsedTime = 0;
    float spawnInterval = 0.5f;

    int size = 5;
    int time = 0;
    int score = 0;

    SetTargetFPS(60);

    while (WindowShouldClose() == false && running) {
        BeginDrawing();

        ClearBackground(BLACK);

        elapsedTime = GetTime() - startTime;

        time++;

        if (time >= 60) {
            time = 0;
            score++;
        }

        DrawText(TextFormat("%i", score), screenWidth / 2 - 50, screenHeight / 2 - 50, 100, GRAY);

        player.Update();

        for (auto& asteroid : asteroids) {
            asteroid.Update();
            asteroid.Draw();

            if (CheckCollisionRecs(player.rect, asteroid.rect) && !player.IsImmune()) {
                size -= 1;

                if (size <= -1) {
                    running = false;
                }

                player.ActivateImmunity();
            }
        }

        if (elapsedTime >= spawnInterval) {
            asteroids.push_back(Asteroid());
            startTime = GetTime();
        }

        health.Draw(size);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}