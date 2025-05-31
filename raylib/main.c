#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef PI
#define PI 3.14159265358979323846f
#endif

#define FIELD_SIZE 1000
#define BLOCK_SIZE 24
#define BALL_SIZE 10
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

typedef struct {
    bool active;
} Block;

typedef struct {
    Vector2 position;
    Vector2 velocity;
} Ball;

Block blocks[FIELD_SIZE][FIELD_SIZE];
Ball ball;
int blocksRemaining;
int score = 0;
bool gameCleared = false;
Vector2 camera = {0, 0};
float zoom = 1.0f;
Sound bounceSound;

void InitializeGame(void) {
    blocksRemaining = 0;
    score = 0;
    
    for (int i = 0; i < FIELD_SIZE; i++) {
        for (int j = 0; j < FIELD_SIZE; j++) {
            if (i != FIELD_SIZE/2 || j != FIELD_SIZE/2) {
                blocks[i][j].active = true;
                blocksRemaining++;
            } else {
                blocks[i][j].active = false;
            }
        }
    }
    
    ball.position.x = (FIELD_SIZE/2) * BLOCK_SIZE + BLOCK_SIZE/2;
    ball.position.y = (FIELD_SIZE/2) * BLOCK_SIZE + BLOCK_SIZE/2;
    
    // Center camera on ball
    camera.x = ball.position.x - WINDOW_WIDTH/2;
    camera.y = ball.position.y - WINDOW_HEIGHT/2;
    
    // Keep camera within bounds
    camera.x = fmaxf(0, fminf(FIELD_SIZE * BLOCK_SIZE - WINDOW_WIDTH, camera.x));
    camera.y = fmaxf(0, fminf(FIELD_SIZE * BLOCK_SIZE - WINDOW_HEIGHT, camera.y));
    
    float angle = GetRandomValue(0, 360) * DEG2RAD;
    float speed = 50.0f;
    ball.velocity.x = cos(angle) * speed;
    ball.velocity.y = sin(angle) * speed;
}

void UpdateBall(void) {
    float deltaTime = GetFrameTime();
    Vector2 newPosition = {
        ball.position.x + ball.velocity.x * deltaTime,
        ball.position.y + ball.velocity.y * deltaTime
    };
    
    // Check ball's 4 corners against walls
    float ballLeft = newPosition.x - BALL_SIZE/2;
    float ballRight = newPosition.x + BALL_SIZE/2;
    float ballTop = newPosition.y - BALL_SIZE/2;
    float ballBottom = newPosition.y + BALL_SIZE/2;
    
    // Check left and right walls
    if (ballLeft <= 0 || ballRight >= FIELD_SIZE * BLOCK_SIZE) {
        ball.velocity.x = -ball.velocity.x;
        PlaySound(bounceSound);
        if (ballLeft <= 0) {
            newPosition.x = BALL_SIZE/2;
        } else {
            newPosition.x = FIELD_SIZE * BLOCK_SIZE - BALL_SIZE/2;
        }
    }
    
    // Check top and bottom walls
    if (ballTop <= 0 || ballBottom >= FIELD_SIZE * BLOCK_SIZE) {
        ball.velocity.y = -ball.velocity.y;
        PlaySound(bounceSound);
        if (ballTop <= 0) {
            newPosition.y = BALL_SIZE/2;
        } else {
            newPosition.y = FIELD_SIZE * BLOCK_SIZE - BALL_SIZE/2;
        }
    }
    
    ball.position = newPosition;
    
    // Check collision with blocks using ball's 4 corners
    ballLeft = ball.position.x - BALL_SIZE/2;
    ballRight = ball.position.x + BALL_SIZE/2;
    ballTop = ball.position.y - BALL_SIZE/2;
    ballBottom = ball.position.y + BALL_SIZE/2;
    
    // Check 4 corners: top-left, top-right, bottom-left, bottom-right
    int corners[4][2] = {
        {(int)(ballLeft / BLOCK_SIZE), (int)(ballTop / BLOCK_SIZE)},      // top-left
        {(int)(ballRight / BLOCK_SIZE), (int)(ballTop / BLOCK_SIZE)},     // top-right
        {(int)(ballLeft / BLOCK_SIZE), (int)(ballBottom / BLOCK_SIZE)},   // bottom-left
        {(int)(ballRight / BLOCK_SIZE), (int)(ballBottom / BLOCK_SIZE)}   // bottom-right
    };
    
    for (int i = 0; i < 4; i++) {
        int blockX = corners[i][0];
        int blockY = corners[i][1];
        
        if (blockX >= 0 && blockX < FIELD_SIZE && blockY >= 0 && blockY < FIELD_SIZE) {
            if (blocks[blockY][blockX].active) {
                blocks[blockY][blockX].active = false;
                blocksRemaining--;
                score++;
                
                float blockCenterX = blockX * BLOCK_SIZE + BLOCK_SIZE/2;
                float blockCenterY = blockY * BLOCK_SIZE + BLOCK_SIZE/2;
                
                float diffX = ball.position.x - blockCenterX;
                float diffY = ball.position.y - blockCenterY;
                
                if (fabs(diffX) > fabs(diffY)) {
                    ball.velocity.x = -ball.velocity.x;
                } else {
                    ball.velocity.y = -ball.velocity.y;
                }
                
                PlaySound(bounceSound);
                break; // Only handle one collision per frame
            }
        }
    }
    
    if (blocksRemaining == 0) {
        gameCleared = true;
    }
}

void HandleInput(void) {
    float scrollSpeed = 300.0f * GetFrameTime();
    
    if (IsKeyDown(KEY_W)) camera.y -= scrollSpeed;
    if (IsKeyDown(KEY_S)) camera.y += scrollSpeed;
    if (IsKeyDown(KEY_A)) camera.x -= scrollSpeed;
    if (IsKeyDown(KEY_D)) camera.x += scrollSpeed;
    
    // Handle zoom with mouse wheel
    float wheelMove = GetMouseWheelMove();
    if (wheelMove != 0) {
        Vector2 mousePos = GetMousePosition();
        
        // Get world position before zoom
        Vector2 worldPos = {
            (mousePos.x + camera.x) / zoom,
            (mousePos.y + camera.y) / zoom
        };
        
        // Apply zoom
        zoom += wheelMove * 0.1f * zoom;
        zoom = fmaxf(0.1f, fminf(5.0f, zoom));
        
        // Adjust camera to keep mouse position fixed
        camera.x = worldPos.x * zoom - mousePos.x;
        camera.y = worldPos.y * zoom - mousePos.y;
    }
    
    // Keep camera within bounds
    float worldWidth = FIELD_SIZE * BLOCK_SIZE * zoom;
    float worldHeight = FIELD_SIZE * BLOCK_SIZE * zoom;
    camera.x = fmaxf(0, fminf(worldWidth - WINDOW_WIDTH, camera.x));
    camera.y = fmaxf(0, fminf(worldHeight - WINDOW_HEIGHT, camera.y));
}

void DrawGame(void) {
    BeginDrawing();
    ClearBackground(BLACK);
    
    // Calculate visible block range with zoom
    float scaledBlockSize = BLOCK_SIZE * zoom;
    int startX = (int)(camera.x / scaledBlockSize);
    int endX = (int)((camera.x + WINDOW_WIDTH) / scaledBlockSize) + 1;
    int startY = (int)(camera.y / scaledBlockSize);
    int endY = (int)((camera.y + WINDOW_HEIGHT) / scaledBlockSize) + 1;
    
    // Clamp to field bounds
    startX = fmaxf(0, startX);
    endX = fminf(FIELD_SIZE, endX);
    startY = fmaxf(0, startY);
    endY = fminf(FIELD_SIZE, endY);
    
    // Draw visible blocks
    for (int i = startY; i < endY; i++) {
        for (int j = startX; j < endX; j++) {
            if (blocks[i][j].active) {
                float screenX = j * scaledBlockSize - camera.x;
                float screenY = i * scaledBlockSize - camera.y;
                float screenSize = scaledBlockSize - 1;
                DrawRectangle(screenX, screenY, screenSize, screenSize, BLUE);
            }
        }
    }
    
    // Draw ball
    float ballScreenX = ball.position.x * zoom - camera.x - (BALL_SIZE * zoom) / 2;
    float ballScreenY = ball.position.y * zoom - camera.y - (BALL_SIZE * zoom) / 2;
    float ballScreenSize = BALL_SIZE * zoom;
    DrawRectangle(ballScreenX, ballScreenY, ballScreenSize, ballScreenSize, WHITE);
    
    if (gameCleared) {
        DrawText("GAME CLEAR!", WINDOW_WIDTH/2 - 100, WINDOW_HEIGHT/2, 30, GREEN);
        DrawText("Press R to restart", WINDOW_WIDTH/2 - 80, WINDOW_HEIGHT/2 + 40, 20, WHITE);
    }
    
    char scoreText[100];
    sprintf(scoreText, "Score: %d | Blocks: %d", score, blocksRemaining);
    DrawText(scoreText, 10, 10, 32, WHITE);
    
    // Display camera and ball position
    char posText[100];
    sprintf(posText, "Camera: (%.0f, %.0f) Ball: (%.0f, %.0f) Zoom: %.1fx", 
            camera.x, camera.y, ball.position.x, ball.position.y, zoom);
    DrawText(posText, 10, 50, 16, LIGHTGRAY);
    
    EndDrawing();
}

int main(void) {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Raylib Block Breaker");
    SetTargetFPS(60);
    
    InitAudioDevice();
    
    // Create a simple click sound
    const int sampleRate = 44100;
    const int sampleCount = sampleRate / 20; // 0.05 seconds
    float *samples = (float *)malloc(sampleCount * sizeof(float));
    
    for (int i = 0; i < sampleCount; i++) {
        float t = (float)i / sampleRate;
        samples[i] = sinf(2 * PI * 800 * t) * expf(-t * 30); // Damped sine wave
    }
    
    Wave wave = {0};
    wave.frameCount = sampleCount;
    wave.sampleRate = sampleRate;
    wave.sampleSize = 32;
    wave.channels = 1;
    wave.data = samples;
    
    bounceSound = LoadSoundFromWave(wave);
    free(samples);
    
    InitializeGame();
    
    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_R)) {
            gameCleared = false;
            InitializeGame();
        }
        
        HandleInput();
        
        if (!gameCleared) {
            UpdateBall();
        }
        
        DrawGame();
    }
    
    UnloadSound(bounceSound);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
