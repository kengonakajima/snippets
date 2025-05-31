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
#define MAX_BALLS 1000

#define BLOCK_COST 10

typedef enum {
    BLOCK_TYPE_BLUE = 1,    // 1 hit to destroy
    BLOCK_TYPE_ORANGE = 2,  // 2 hits to destroy
    BLOCK_TYPE_GRAY = 3     // Indestructible
} BlockType;

typedef struct {
    bool active;
    BlockType type;
} Block;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    bool active;
} Ball;

Block blocks[FIELD_SIZE][FIELD_SIZE];
Ball balls[MAX_BALLS];
int ballCount = 0;
int blocksRemaining;
int score = 0;
bool gameCleared = false;
Vector2 camera = {0, 0};
float zoom = 1.0f;
Sound bounceSound;
Sound ballCollisionSound;
Vector2 lastMousePos = {0, 0};
bool isDragging = false;

void AddBall(void) {
    if (ballCount >= MAX_BALLS) return;
    
    balls[ballCount].position.x = (FIELD_SIZE/2) * BLOCK_SIZE + BLOCK_SIZE/2;
    balls[ballCount].position.y = (FIELD_SIZE/2) * BLOCK_SIZE + BLOCK_SIZE/2;
    balls[ballCount].active = true;
    
    float angle = GetRandomValue(0, 360) * DEG2RAD;
    float speed = 50.0f;
    balls[ballCount].velocity.x = cos(angle) * speed;
    balls[ballCount].velocity.y = sin(angle) * speed;
    
    ballCount++;
}

void InitializeGame(void) {
    blocksRemaining = 0;
    score = 0;
    ballCount = 0;
    
    // Initialize all balls as inactive
    for (int i = 0; i < MAX_BALLS; i++) {
        balls[i].active = false;
    }
    
    for (int i = 0; i < FIELD_SIZE; i++) {
        for (int j = 0; j < FIELD_SIZE; j++) {
            if (i != FIELD_SIZE/2 || j != FIELD_SIZE/2) {
                blocks[i][j].active = true;
                // Randomly assign block types: 70% blue, 30% orange
                if (GetRandomValue(1, 100) <= 70) {
                    blocks[i][j].type = BLOCK_TYPE_BLUE;
                } else {
                    blocks[i][j].type = BLOCK_TYPE_ORANGE;
                }
                blocksRemaining++;
            } else {
                blocks[i][j].active = false;
            }
        }
    }
    
    // Add first ball
    AddBall();
    
    // Center camera on first ball
    camera.x = balls[0].position.x - WINDOW_WIDTH/2;
    camera.y = balls[0].position.y - WINDOW_HEIGHT/2;
    
    // Keep camera within bounds
    camera.x = fmaxf(0, fminf(FIELD_SIZE * BLOCK_SIZE - WINDOW_WIDTH, camera.x));
    camera.y = fmaxf(0, fminf(FIELD_SIZE * BLOCK_SIZE - WINDOW_HEIGHT, camera.y));
}

void UpdateBalls(void) {
    float deltaTime = GetFrameTime();
    
    for (int ballIndex = 0; ballIndex < ballCount; ballIndex++) {
        if (!balls[ballIndex].active) continue;
        
        Ball* ball = &balls[ballIndex];
        Vector2 newPosition = {
            ball->position.x + ball->velocity.x * deltaTime,
            ball->position.y + ball->velocity.y * deltaTime
        };
        
        // Check ball's 4 corners against walls
        float ballLeft = newPosition.x - BALL_SIZE/2;
        float ballRight = newPosition.x + BALL_SIZE/2;
        float ballTop = newPosition.y - BALL_SIZE/2;
        float ballBottom = newPosition.y + BALL_SIZE/2;
        
        // Check left and right walls
        if (ballLeft <= 0 || ballRight >= FIELD_SIZE * BLOCK_SIZE) {
            ball->velocity.x = -ball->velocity.x;
            PlaySound(bounceSound);
            if (ballLeft <= 0) {
                newPosition.x = BALL_SIZE/2;
            } else {
                newPosition.x = FIELD_SIZE * BLOCK_SIZE - BALL_SIZE/2;
            }
        }
        
        // Check top and bottom walls
        if (ballTop <= 0 || ballBottom >= FIELD_SIZE * BLOCK_SIZE) {
            ball->velocity.y = -ball->velocity.y;
            PlaySound(bounceSound);
            if (ballTop <= 0) {
                newPosition.y = BALL_SIZE/2;
            } else {
                newPosition.y = FIELD_SIZE * BLOCK_SIZE - BALL_SIZE/2;
            }
        }
        
        ball->position = newPosition;
        
        // Check collision with blocks using ball's 4 corners
        ballLeft = ball->position.x - BALL_SIZE/2;
        ballRight = ball->position.x + BALL_SIZE/2;
        ballTop = ball->position.y - BALL_SIZE/2;
        ballBottom = ball->position.y + BALL_SIZE/2;
        
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
                    if (blocks[blockY][blockX].type == BLOCK_TYPE_ORANGE) {
                        // Orange block becomes blue after one hit
                        blocks[blockY][blockX].type = BLOCK_TYPE_BLUE;
                        score++;
                    } else if (blocks[blockY][blockX].type == BLOCK_TYPE_BLUE) {
                        // Blue block is destroyed after one hit
                        blocks[blockY][blockX].active = false;
                        blocksRemaining--;
                        score++;
                    }
                    // Gray blocks don't change or get destroyed
                    
                    float blockCenterX = blockX * BLOCK_SIZE + BLOCK_SIZE/2;
                    float blockCenterY = blockY * BLOCK_SIZE + BLOCK_SIZE/2;
                    
                    float diffX = ball->position.x - blockCenterX;
                    float diffY = ball->position.y - blockCenterY;
                    
                    if (fabs(diffX) > fabs(diffY)) {
                        ball->velocity.x = -ball->velocity.x;
                    } else {
                        ball->velocity.y = -ball->velocity.y;
                    }
                    
                    PlaySound(bounceSound);
                    break; // Only handle one collision per frame
                }
            }
        }
        
        // Check collision with other balls
        for (int otherIndex = ballIndex + 1; otherIndex < ballCount; otherIndex++) {
            if (!balls[otherIndex].active) continue;
            
            Ball* otherBall = &balls[otherIndex];
            
            // Calculate distance between ball centers
            float dx = ball->position.x - otherBall->position.x;
            float dy = ball->position.y - otherBall->position.y;
            float distance = sqrtf(dx * dx + dy * dy);
            
            // Check if balls are overlapping (collision)
            if (distance < BALL_SIZE) {
                // Prevent division by zero
                if (distance == 0) {
                    dx = 1;
                    dy = 0;
                    distance = 1;
                }
                
                // Normalize collision vector
                float nx = dx / distance;
                float ny = dy / distance;
                
                // Separate balls to prevent overlap
                float overlap = BALL_SIZE - distance;
                float separateDistance = overlap / 2;
                
                ball->position.x += nx * separateDistance;
                ball->position.y += ny * separateDistance;
                otherBall->position.x -= nx * separateDistance;
                otherBall->position.y -= ny * separateDistance;
                
                // Calculate relative velocity
                float rvx = ball->velocity.x - otherBall->velocity.x;
                float rvy = ball->velocity.y - otherBall->velocity.y;
                
                // Calculate relative velocity in collision normal direction
                float speedAlongNormal = rvx * nx + rvy * ny;
                
                // Do not resolve if velocities are separating
                if (speedAlongNormal > 0) continue;
                
                // Apply impulse to separate the balls
                ball->velocity.x -= speedAlongNormal * nx;
                ball->velocity.y -= speedAlongNormal * ny;
                otherBall->velocity.x += speedAlongNormal * nx;
                otherBall->velocity.y += speedAlongNormal * ny;
                
                PlaySound(ballCollisionSound);
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
    
    // Add ball on space key press
    if (IsKeyPressed(KEY_SPACE)) {
        AddBall();
    }
    
    // Handle mouse dragging for camera movement
    Vector2 mousePos = GetMousePosition();
    
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        lastMousePos = mousePos;
        isDragging = true;
    }
    
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && isDragging) {
        Vector2 mouseDelta = {
            mousePos.x - lastMousePos.x,
            mousePos.y - lastMousePos.y
        };
        
        // Move camera in opposite direction of mouse movement
        camera.x -= mouseDelta.x;
        camera.y -= mouseDelta.y;
        
        lastMousePos = mousePos;
    }
    
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        if (isDragging) {
            // Check if it was a click (small movement) vs drag
            Vector2 totalDelta = {
                mousePos.x - lastMousePos.x,
                mousePos.y - lastMousePos.y
            };
            float totalDistance = sqrtf(totalDelta.x * totalDelta.x + totalDelta.y * totalDelta.y);
            
            // If movement was small, treat as click to place gray block
            if (totalDistance < 5.0f && score >= BLOCK_COST) {
                // Convert screen coordinates to world coordinates
                float worldX = (mousePos.x + camera.x) / zoom;
                float worldY = (mousePos.y + camera.y) / zoom;
                
                // Convert to block coordinates
                int blockX = (int)(worldX / BLOCK_SIZE);
                int blockY = (int)(worldY / BLOCK_SIZE);
                
                // Place gray block if within bounds and empty, and not overlapping with any ball
                if (blockX >= 0 && blockX < FIELD_SIZE && blockY >= 0 && blockY < FIELD_SIZE) {
                    if (!blocks[blockY][blockX].active) {
                        // Check if any ball would overlap with this block position
                        bool canPlace = true;
                        float blockLeft = blockX * BLOCK_SIZE;
                        float blockRight = blockX * BLOCK_SIZE + BLOCK_SIZE;
                        float blockTop = blockY * BLOCK_SIZE;
                        float blockBottom = blockY * BLOCK_SIZE + BLOCK_SIZE;
                        
                        for (int i = 0; i < ballCount; i++) {
                            if (balls[i].active) {
                                float ballLeft = balls[i].position.x - BALL_SIZE/2;
                                float ballRight = balls[i].position.x + BALL_SIZE/2;
                                float ballTop = balls[i].position.y - BALL_SIZE/2;
                                float ballBottom = balls[i].position.y + BALL_SIZE/2;
                                
                                // Check if ball rectangle overlaps with block rectangle
                                if (ballRight > blockLeft && ballLeft < blockRight &&
                                    ballBottom > blockTop && ballTop < blockBottom) {
                                    canPlace = false;
                                    break;
                                }
                            }
                        }
                        
                        if (canPlace) {
                            blocks[blockY][blockX].active = true;
                            blocks[blockY][blockX].type = BLOCK_TYPE_GRAY;
                            score -= BLOCK_COST;
                        }
                    }
                }
            }
        }
        isDragging = false;
    }
    
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
        zoom = fmaxf(0.01f, fminf(5.0f, zoom));
        
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
                
                Color blockColor = BLUE; // Default
                if (blocks[i][j].type == BLOCK_TYPE_BLUE) {
                    blockColor = BLUE;
                } else if (blocks[i][j].type == BLOCK_TYPE_ORANGE) {
                    blockColor = ORANGE;
                } else if (blocks[i][j].type == BLOCK_TYPE_GRAY) {
                    blockColor = GRAY;
                }
                
                DrawRectangle(screenX, screenY, screenSize, screenSize, blockColor);
            }
        }
    }
    
    // Draw balls
    for (int i = 0; i < ballCount; i++) {
        if (balls[i].active) {
            float ballScreenX = balls[i].position.x * zoom - camera.x - (BALL_SIZE * zoom) / 2;
            float ballScreenY = balls[i].position.y * zoom - camera.y - (BALL_SIZE * zoom) / 2;
            float ballScreenSize = BALL_SIZE * zoom;
            DrawRectangle(ballScreenX, ballScreenY, ballScreenSize, ballScreenSize, WHITE);
        }
    }
    
    if (gameCleared) {
        DrawText("GAME CLEAR!", WINDOW_WIDTH/2 - 100, WINDOW_HEIGHT/2, 30, GREEN);
        DrawText("Press R to restart", WINDOW_WIDTH/2 - 80, WINDOW_HEIGHT/2 + 40, 20, WHITE);
    }
    
    char scoreText[100];
    sprintf(scoreText, "Score: %d | Blocks: %d", score, blocksRemaining);
    DrawText(scoreText, 10, 10, 32, WHITE);
    
    // Display camera and ball position
    char posText[100];
    sprintf(posText, "Camera: (%.0f, %.0f) Balls: %d Zoom: %.1fx", 
            camera.x, camera.y, ballCount, zoom);
    DrawText(posText, 10, 50, 16, LIGHTGRAY);
    
    EndDrawing();
}

int main(void) {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Raylib Block Breaker");
    SetTargetFPS(60);
    
    InitAudioDevice();
    
    // Create a bounce sound (wall/block collision)
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
    
    // Create a high-pitched short sound for ball-ball collision
    const int shortSampleCount = sampleRate / 40; // 0.025 seconds (shorter)
    float *shortSamples = (float *)malloc(shortSampleCount * sizeof(float));
    
    for (int i = 0; i < shortSampleCount; i++) {
        float t = (float)i / sampleRate;
        shortSamples[i] = sinf(2 * PI * 1600 * t) * expf(-t * 60); // Higher frequency, faster decay
    }
    
    Wave shortWave = {0};
    shortWave.frameCount = shortSampleCount;
    shortWave.sampleRate = sampleRate;
    shortWave.sampleSize = 32;
    shortWave.channels = 1;
    shortWave.data = shortSamples;
    
    ballCollisionSound = LoadSoundFromWave(shortWave);
    free(shortSamples);
    
    InitializeGame();
    
    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_R)) {
            gameCleared = false;
            InitializeGame();
        }
        
        HandleInput();
        
        if (!gameCleared) {
            UpdateBalls();
        }
        
        DrawGame();
    }
    
    UnloadSound(bounceSound);
    UnloadSound(ballCollisionSound);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
