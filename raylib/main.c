#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define FIELD_SIZE 20
#define BLOCK_SIZE 30
#define BALL_RADIUS 8
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
bool gameCleared = false;

void InitializeGame(void) {
    blocksRemaining = 0;
    
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
    
    float angle = GetRandomValue(0, 360) * DEG2RAD;
    float speed = 200.0f;
    ball.velocity.x = cos(angle) * speed;
    ball.velocity.y = sin(angle) * speed;
}

void UpdateBall(void) {
    float deltaTime = GetFrameTime();
    Vector2 newPosition = {
        ball.position.x + ball.velocity.x * deltaTime,
        ball.position.y + ball.velocity.y * deltaTime
    };
    
    if (newPosition.x - BALL_RADIUS <= 0) {
        ball.velocity.x = -ball.velocity.x;
        newPosition.x = BALL_RADIUS;
    } else if (newPosition.x + BALL_RADIUS >= FIELD_SIZE * BLOCK_SIZE) {
        ball.velocity.x = -ball.velocity.x;
        newPosition.x = FIELD_SIZE * BLOCK_SIZE - BALL_RADIUS;
    }
    
    if (newPosition.y - BALL_RADIUS <= 0) {
        ball.velocity.y = -ball.velocity.y;
        newPosition.y = BALL_RADIUS;
    } else if (newPosition.y + BALL_RADIUS >= FIELD_SIZE * BLOCK_SIZE) {
        ball.velocity.y = -ball.velocity.y;
        newPosition.y = FIELD_SIZE * BLOCK_SIZE - BALL_RADIUS;
    }
    
    ball.position = newPosition;
    
    int blockX = (int)(ball.position.x / BLOCK_SIZE);
    int blockY = (int)(ball.position.y / BLOCK_SIZE);
    
    if (blockX >= 0 && blockX < FIELD_SIZE && blockY >= 0 && blockY < FIELD_SIZE) {
        if (blocks[blockY][blockX].active) {
            blocks[blockY][blockX].active = false;
            blocksRemaining--;
            
            float blockCenterX = blockX * BLOCK_SIZE + BLOCK_SIZE/2;
            float blockCenterY = blockY * BLOCK_SIZE + BLOCK_SIZE/2;
            
            float diffX = ball.position.x - blockCenterX;
            float diffY = ball.position.y - blockCenterY;
            
            if (fabs(diffX) > fabs(diffY)) {
                ball.velocity.x = -ball.velocity.x;
            } else {
                ball.velocity.y = -ball.velocity.y;
            }
        }
    }
    
    if (blocksRemaining == 0) {
        gameCleared = true;
    }
}

void DrawGame(void) {
    BeginDrawing();
    ClearBackground(BLACK);
    
    for (int i = 0; i < FIELD_SIZE; i++) {
        for (int j = 0; j < FIELD_SIZE; j++) {
            if (blocks[i][j].active) {
                DrawRectangle(j * BLOCK_SIZE, i * BLOCK_SIZE, BLOCK_SIZE - 1, BLOCK_SIZE - 1, BLUE);
            }
        }
    }
    
    DrawCircleV(ball.position, BALL_RADIUS, WHITE);
    
    if (gameCleared) {
        DrawText("GAME CLEAR!", WINDOW_WIDTH/2 - 100, WINDOW_HEIGHT/2, 30, GREEN);
        DrawText("Press R to restart", WINDOW_WIDTH/2 - 80, WINDOW_HEIGHT/2 + 40, 20, WHITE);
    }
    
    char scoreText[50];
    sprintf(scoreText, "Blocks: %d", blocksRemaining);
    DrawText(scoreText, 10, 10, 20, WHITE);
    
    EndDrawing();
}

int main(void) {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Raylib Block Breaker");
    SetTargetFPS(60);
    
    InitializeGame();
    
    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_R)) {
            gameCleared = false;
            InitializeGame();
        }
        
        if (!gameCleared) {
            UpdateBall();
        }
        
        DrawGame();
    }
    
    CloseWindow();
    return 0;
}