#include "raylib.h"
#include "chipmunk/chipmunk.h"
#include "chipmunk/cpHastySpace.h"
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <stdbool.h>

#define SCREEN_WIDTH 1248
#define SCREEN_HEIGHT 980
#define FIELD_WIDTH (SCREEN_WIDTH * 8)  // 横方向に8倍広い
#define SCALE 64.0f
#define PI 3.14159265358979323846f
#define MAX_DEBRIS 50000
#define SIEVE_COUNT 10
#define SIEVE_ANGLE (20.0f * PI / 180.0f)
#define SIEVE_LENGTH 60.0f
#define SIEVE_THICKNESS 6.0f
#define SIEVE_GAP 10.0f
#define SIEVE_FREQ 8.0f
#define SIEVE_AMP 3.0f
#define MAX_VERTICES 8
#define COLLISION_TYPE_DEBRIS 1
#define WAKE_UP_IMPULSE_THRESHOLD 120.0f  // この衝撃を超えたら起こす

typedef struct {
    cpBody *body;
    cpShape *shape;
    cpVect vertices[MAX_VERTICES];
    int vertexCount;
    float size;
    bool active;
    bool isStatic;      // スタティック化されたか
    bool wakeUp;        // ダイナミックに戻すフラグ
    float idleTime;     // 静止している時間
    cpVect lastPos;     // 前フレームの位置
    cpVect staticPos;   // スタティック化時の位置
    cpFloat staticAngle;// スタティック化時の角度
} Debris;

static Debris debris[MAX_DEBRIS];
static int debrisCount = 0;

static cpBody *sieveBodies[SIEVE_COUNT];
static cpShape *sieveShapes[SIEVE_COUNT];
static cpVect sieveBasePos[SIEVE_COUNT];
static cpShape *groundShape;
static cpShape *leftWallShape;
static cpShape *rightWallShape;
static cpSpace *space;

// カメラオフセット（スクロール用）
static float cameraX = 0.0f;

static int compareFloats(const void* a, const void* b) {
    float fa = *(const float*)a;
    float fb = *(const float*)b;
    return (fa > fb) - (fa < fb);
}

static float rand01(void) {
    return (float)rand() / (float)RAND_MAX;
}

static int findFreeSlot(void) {
    for (int i = 0; i < MAX_DEBRIS; i++) {
        if (!debris[i].active) return i;
    }
    return -1;
}

static void spawnDebris(void) {
    int slot = findFreeSlot();
    if (slot < 0) return;

    // サイズ: 3〜18 (小さいものが多い分布)
    float size = 3.0f + powf(rand01(), 2.0f) * 15.0f;
    // フィールド中央付近から生成
    float x = FIELD_WIDTH / 2.0f - 200.0f + rand01() * 300.0f;
    float y = -50.0f;

    int vertexCount = 3 + rand() % 5;
    float angles[MAX_VERTICES];
    cpVect points[MAX_VERTICES];

    for (int i = 0; i < vertexCount; i++) {
        angles[i] = ((float)i / vertexCount + rand01() * 0.1f) * 2.0f * PI;
    }
    qsort(angles, vertexCount, sizeof(float), compareFloats);

    for (int i = 0; i < vertexCount; i++) {
        float r = size * (0.7f + rand01() * 0.3f);
        points[i] = cpv(r * cosf(angles[i]), r * sinf(angles[i]));
    }

    cpFloat mass = 1.0f;
    cpFloat moment = cpMomentForPoly(mass, vertexCount, points, cpvzero, 0.0f);
    cpBody *body = cpBodyNew(mass, moment);
    cpBodySetPosition(body, cpv(x, y));
    cpBodySetAngle(body, rand01() * 2.0f * PI);
    cpSpaceAddBody(space, body);

    cpShape *shape = cpPolyShapeNew(body, vertexCount, points, cpTransformIdentity, 0.0f);
    cpShapeSetFriction(shape, 0.5f);
    // 小さいほど跳ねる: サイズ3で0.8、サイズ18で0.1
    float elasticity = 0.8f - (size - 3.0f) / 15.0f * 0.7f;
    cpShapeSetElasticity(shape, elasticity);
    cpShapeSetCollisionType(shape, COLLISION_TYPE_DEBRIS);
    cpShapeSetUserData(shape, (void*)(intptr_t)slot);
    cpSpaceAddShape(space, shape);

    debris[slot].body = body;
    debris[slot].shape = shape;
    debris[slot].vertexCount = vertexCount;
    for (int i = 0; i < vertexCount; i++) {
        debris[slot].vertices[i] = points[i];
    }
    debris[slot].size = size;
    debris[slot].active = true;
    debris[slot].isStatic = false;
    debris[slot].wakeUp = false;
    debris[slot].idleTime = 0.0f;
    debris[slot].lastPos = cpv(x, y);
    if (slot >= debrisCount) debrisCount = slot + 1;
}

static void cleanupDebris(void) {
    for (int i = 0; i < debrisCount; i++) {
        if (!debris[i].active || debris[i].isStatic) continue;
        cpVect pos = cpBodyGetPosition(debris[i].body);
        if (pos.x > FIELD_WIDTH + 100 || pos.x < -100.0f || pos.y > SCREEN_HEIGHT + 100) {
            cpSpaceRemoveShape(space, debris[i].shape);
            cpSpaceRemoveBody(space, debris[i].body);
            cpShapeFree(debris[i].shape);
            cpBodyFree(debris[i].body);
            debris[i].active = false;
        }
    }
}

// 安定した石をスタティック化
static void staticizeDebris(float dt) {
    cpBody *staticBody = cpSpaceGetStaticBody(space);
    for (int i = 0; i < debrisCount; i++) {
        if (!debris[i].active || debris[i].isStatic || !debris[i].body) continue;

        cpVect pos = cpBodyGetPosition(debris[i].body);
        cpVect diff = cpvsub(pos, debris[i].lastPos);
        float moved = cpvlength(diff);

        // ほとんど動いていない場合（高さ制限なし）
        if (moved < 1.0f) {
            debris[i].idleTime += dt;
        } else {
            debris[i].idleTime = 0.0f;
        }
        debris[i].lastPos = pos;

        // 1秒以上静止していたらスタティック化
        if (debris[i].idleTime > 1.0f) {
            cpFloat angle = cpBodyGetAngle(debris[i].body);

            // 位置と角度を保存
            debris[i].staticPos = pos;
            debris[i].staticAngle = angle;

            // 古いボディとシェイプを削除
            cpSpaceRemoveShape(space, debris[i].shape);
            cpSpaceRemoveBody(space, debris[i].body);
            cpShapeFree(debris[i].shape);
            cpBodyFree(debris[i].body);

            // スタティックシェイプとして再作成
            cpTransform t = cpTransformRigid(pos, angle);
            debris[i].shape = cpPolyShapeNew(staticBody, debris[i].vertexCount,
                debris[i].vertices, t, 0.0f);
            cpShapeSetFriction(debris[i].shape, 0.5f);
            cpShapeSetCollisionType(debris[i].shape, COLLISION_TYPE_DEBRIS);
            cpShapeSetUserData(debris[i].shape, (void*)(intptr_t)i);
            cpSpaceAddShape(space, debris[i].shape);

            debris[i].body = NULL;
            debris[i].isStatic = true;
            debris[i].wakeUp = false;
        }
    }
}

// 衝突後のコールバック: スタティックな石に大きな衝撃があったら起こす
static void postSolveHandler(cpArbiter *arb, cpSpace *space, void *data) {
    (void)space;
    (void)data;

    cpShape *a, *b;
    cpArbiterGetShapes(arb, &a, &b);

    // 両方のシェイプをチェック
    cpShape *shapes[2] = {a, b};
    for (int s = 0; s < 2; s++) {
        if (cpShapeGetCollisionType(shapes[s]) != COLLISION_TYPE_DEBRIS) continue;

        int idx = (int)(intptr_t)cpShapeGetUserData(shapes[s]);
        if (idx < 0 || idx >= debrisCount) continue;
        if (!debris[idx].active || !debris[idx].isStatic) continue;

        // 衝撃力を計算
        cpFloat impulse = cpvlength(cpArbiterTotalImpulse(arb));
        if (impulse > WAKE_UP_IMPULSE_THRESHOLD) {
            debris[idx].wakeUp = true;
        }
    }
}

// スタティック化した石をダイナミックに戻す
static void wakeUpStaticDebris(void) {
    for (int i = 0; i < debrisCount; i++) {
        if (!debris[i].active || !debris[i].isStatic || !debris[i].wakeUp) continue;

        // 古いスタティックシェイプを削除
        cpSpaceRemoveShape(space, debris[i].shape);
        cpShapeFree(debris[i].shape);

        // ダイナミックボディを再作成
        cpFloat mass = 1.0f;
        cpFloat moment = cpMomentForPoly(mass, debris[i].vertexCount, debris[i].vertices, cpvzero, 0.0f);
        cpBody *body = cpBodyNew(mass, moment);
        cpBodySetPosition(body, debris[i].staticPos);
        cpBodySetAngle(body, debris[i].staticAngle);
        cpSpaceAddBody(space, body);

        cpShape *shape = cpPolyShapeNew(body, debris[i].vertexCount, debris[i].vertices, cpTransformIdentity, 0.0f);
        cpShapeSetFriction(shape, 0.5f);
        float elasticity = 0.8f - (debris[i].size - 3.0f) / 15.0f * 0.7f;
        cpShapeSetElasticity(shape, elasticity);
        cpShapeSetCollisionType(shape, COLLISION_TYPE_DEBRIS);
        cpShapeSetUserData(shape, (void*)(intptr_t)i);
        cpSpaceAddShape(space, shape);

        debris[i].body = body;
        debris[i].shape = shape;
        debris[i].isStatic = false;
        debris[i].wakeUp = false;
        debris[i].idleTime = 0.0f;
        debris[i].lastPos = debris[i].staticPos;
    }
}

static Color getDebrisColor(float size) {
    // サイズ 3〜18 を 0〜1 に正規化
    float t = (size - 3.0f) / 15.0f;
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    // 小さい: 明るい灰色(180)、大きい: 暗い灰色(80)
    unsigned char gray = (unsigned char)(180 - t * 100);
    return (Color){gray, gray, gray, 255};
}

static void drawDebris(int index) {
    if (!debris[index].active) return;

    Color color;
    if (debris[index].isStatic) {
        color = (Color){80, 80, 180, 255};  // スタティックは青
    } else if (cpBodyIsSleeping(debris[index].body)) {
        color = (Color){180, 80, 80, 255};  // スリープ中は赤
    } else {
        color = getDebrisColor(debris[index].size);
    }

    Vector2 screenPoints[MAX_VERTICES];
    if (debris[index].isStatic) {
        // スタティック化した石は保存した位置と角度を使用
        cpFloat c = cpfcos(debris[index].staticAngle);
        cpFloat s = cpfsin(debris[index].staticAngle);
        for (int i = 0; i < debris[index].vertexCount; i++) {
            cpVect v = debris[index].vertices[i];
            cpVect rotated = cpv(v.x * c - v.y * s, v.x * s + v.y * c);
            cpVect worldPt = cpvadd(debris[index].staticPos, rotated);
            screenPoints[i] = (Vector2){(float)worldPt.x - cameraX, (float)worldPt.y};
        }
    } else {
        for (int i = 0; i < debris[index].vertexCount; i++) {
            cpVect worldPt = cpBodyLocalToWorld(debris[index].body, debris[index].vertices[i]);
            screenPoints[i] = (Vector2){(float)worldPt.x - cameraX, (float)worldPt.y};
        }
    }

    for (int i = 1; i < debris[index].vertexCount - 1; i++) {
        DrawTriangle(screenPoints[0], screenPoints[i + 1], screenPoints[i], color);
    }

    Color outline = {color.r / 2, color.g / 2, color.b / 2, 255};
    for (int i = 0; i < debris[index].vertexCount; i++) {
        int next = (i + 1) % debris[index].vertexCount;
        DrawLineV(screenPoints[i], screenPoints[next], outline);
    }
}

static void createSieve(void) {
    float spacing = SIEVE_LENGTH + SIEVE_GAP;
    // フィールド中央付近に配置
    float startX = FIELD_WIDTH / 2.0f - 300.0f;
    float startY = 150.0f;

    for (int i = 0; i < SIEVE_COUNT; i++) {
        float offsetX = i * spacing * cosf(SIEVE_ANGLE);
        float offsetY = i * spacing * sinf(SIEVE_ANGLE);

        sieveBasePos[i] = cpv(startX + offsetX, startY + offsetY);

        cpBody *body = cpBodyNewKinematic();
        cpBodySetPosition(body, sieveBasePos[i]);
        cpBodySetAngle(body, SIEVE_ANGLE);
        cpSpaceAddBody(space, body);

        cpFloat hw = SIEVE_LENGTH / 2.0f;
        cpFloat hh = SIEVE_THICKNESS / 2.0f;
        cpVect verts[4] = {
            cpv(-hw, -hh), cpv(-hw, hh), cpv(hw, hh), cpv(hw, -hh)
        };
        cpShape *shape = cpPolyShapeNew(body, 4, verts, cpTransformIdentity, 0.0f);
        cpShapeSetFriction(shape, 0.5f);
        cpSpaceAddShape(space, shape);

        sieveBodies[i] = body;
        sieveShapes[i] = shape;
    }
}

static void updateSieveVibration(float time) {
    float omega = 2.0f * PI * SIEVE_FREQ;
    float velocityPhase = cosf(time * omega);
    float speed = SIEVE_AMP * omega * velocityPhase;

    float normalAngle = SIEVE_ANGLE + PI / 2.0f;
    float vx = speed * cosf(normalAngle);
    float vy = speed * sinf(normalAngle);

    cpVect velocity = cpv(vx, vy);
    for (int i = 0; i < SIEVE_COUNT; i++) {
        cpBodySetVelocity(sieveBodies[i], velocity);
    }
}

static void drawSieve(void) {
    for (int i = 0; i < SIEVE_COUNT; i++) {
        cpVect pos = cpBodyGetPosition(sieveBodies[i]);
        cpFloat angle = cpBodyGetAngle(sieveBodies[i]);

        Rectangle rect = {(float)pos.x - cameraX, (float)pos.y, SIEVE_LENGTH, SIEVE_THICKNESS};
        Vector2 origin = {SIEVE_LENGTH / 2.0f, SIEVE_THICKNESS / 2.0f};
        DrawRectanglePro(rect, origin, angle * 180.0f / PI, DARKGRAY);
    }
}

static void createGround(void) {
    cpBody *staticBody = cpSpaceGetStaticBody(space);

    // 地面（フィールド全体）
    cpFloat hw = FIELD_WIDTH / 2.0f;
    cpFloat hh = 10.0f;
    cpVect verts[4] = {
        cpv(-hw, -hh), cpv(-hw, hh), cpv(hw, hh), cpv(hw, -hh)
    };
    groundShape = cpPolyShapeNew(staticBody, 4, verts,
        cpTransformTranslate(cpv(FIELD_WIDTH / 2.0f, SCREEN_HEIGHT - 10.0f)), 0.0f);
    cpShapeSetFriction(groundShape, 1.0f);
    cpSpaceAddShape(space, groundShape);

    // 左壁
    cpFloat wallHW = 10.0f;
    cpFloat wallHH = SCREEN_HEIGHT / 2.0f;
    cpVect leftVerts[4] = {
        cpv(-wallHW, -wallHH), cpv(-wallHW, wallHH), cpv(wallHW, wallHH), cpv(wallHW, -wallHH)
    };
    leftWallShape = cpPolyShapeNew(staticBody, 4, leftVerts,
        cpTransformTranslate(cpv(-10.0f, SCREEN_HEIGHT / 2.0f)), 0.0f);
    cpShapeSetFriction(leftWallShape, 0.5f);
    cpSpaceAddShape(space, leftWallShape);

    // 右壁（フィールド右端）
    cpVect rightVerts[4] = {
        cpv(-wallHW, -wallHH), cpv(-wallHW, wallHH), cpv(wallHW, wallHH), cpv(wallHW, -wallHH)
    };
    rightWallShape = cpPolyShapeNew(staticBody, 4, rightVerts,
        cpTransformTranslate(cpv(FIELD_WIDTH + 10.0f, SCREEN_HEIGHT / 2.0f)), 0.0f);
    cpShapeSetFriction(rightWallShape, 0.5f);
    cpSpaceAddShape(space, rightWallShape);
}

static void drawGround(void) {
    // カメラオフセットを適用して地面を描画
    DrawRectangle((int)(0 - cameraX), SCREEN_HEIGHT - 20, FIELD_WIDTH, 20, DARKGRAY);
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sieve Simulation (Chipmunk2D)");
    SetTargetFPS(60);
    srand(42);

    space = cpHastySpaceNew();
    cpHastySpaceSetThreads(space, 0);  // 0 = 自動検出
    cpSpaceSetGravity(space, cpv(0.0f, 9.8f * SCALE));
    cpSpaceSetIterations(space, 20);
    cpSpaceSetCollisionSlop(space, 0.5f);
    cpSpaceSetCollisionBias(space, cpfpow(1.0f - 0.3f, 60.0f));
    // 0.2秒動かなければスリープ、閾値を大きく
    cpSpaceSetSleepTimeThreshold(space, 0.2f);
    cpSpaceSetIdleSpeedThreshold(space, 50.0f);

    createSieve();
    createGround();

    // 衝突ハンドラを登録
    cpCollisionHandler *handler = cpSpaceAddDefaultCollisionHandler(space);
    handler->postSolveFunc = postSolveHandler;

    float spawnTimer = 0.0f;
    float physicsTimeMs = 0.0f;
    bool spawning = false;

    // カメラをフィールド中央に初期化
    cameraX = FIELD_WIDTH / 2.0f - SCREEN_WIDTH / 2.0f;

    // マウスドラッグ用
    bool dragging = false;
    float dragStartX = 0.0f;
    float cameraStartX = 0.0f;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // マウスドラッグでスクロール
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            dragging = true;
            dragStartX = GetMouseX();
            cameraStartX = cameraX;
        }
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            dragging = false;
        }
        if (dragging) {
            float dx = GetMouseX() - dragStartX;
            cameraX = cameraStartX - dx;
            // カメラ範囲制限
            if (cameraX < 0) cameraX = 0;
            if (cameraX > FIELD_WIDTH - SCREEN_WIDTH) cameraX = FIELD_WIDTH - SCREEN_WIDTH;
        }

        if (IsKeyPressed(KEY_SPACE)) {
            spawning = !spawning;
        }

        if (spawning) {
            spawnTimer += dt;
            if (spawnTimer > 0.016f) {
                spawnDebris();
                spawnDebris();
                spawnDebris();
                spawnTimer = 0.0f;
            }
        }

        updateSieveVibration((float)GetTime());

        double startTime = GetTime();
        cpHastySpaceStep(space, 1.0f / 60.0f);
        double endTime = GetTime();
        physicsTimeMs = (float)((endTime - startTime) * 1000.0);

        cleanupDebris();
        staticizeDebris(dt);
        wakeUpStaticDebris();

        BeginDrawing();
        ClearBackground(RAYWHITE);

        drawGround();
        drawSieve();

        for (int i = 0; i < debrisCount; i++) {
            drawDebris(i);
        }

        int activeCount = 0;
        for (int i = 0; i < debrisCount; i++) {
            if (debris[i].active) activeCount++;
        }
        // ワールド座標でのマウス位置
        float worldMouseX = GetMouseX() + cameraX;
        float worldMouseY = GetMouseY();

        DrawRectangle(5, 5, 220, 95, (Color){255, 255, 255, 200});
        DrawFPS(10, 10);
        DrawText(TextFormat("Debris: %d", activeCount), 10, 35, 20, BLACK);
        DrawText(TextFormat("Physics: %.2f ms", physicsTimeMs), 10, 55, 20, BLACK);
        DrawText(TextFormat("Mouse: %.0f, %.0f", worldMouseX, worldMouseY), 10, 75, 20, BLACK);

        EndDrawing();
    }

    for (int i = 0; i < debrisCount; i++) {
        if (debris[i].active) {
            cpSpaceRemoveShape(space, debris[i].shape);
            cpShapeFree(debris[i].shape);
            if (!debris[i].isStatic) {
                cpSpaceRemoveBody(space, debris[i].body);
                cpBodyFree(debris[i].body);
            }
        }
    }
    for (int i = 0; i < SIEVE_COUNT; i++) {
        cpSpaceRemoveShape(space, sieveShapes[i]);
        cpSpaceRemoveBody(space, sieveBodies[i]);
        cpShapeFree(sieveShapes[i]);
        cpBodyFree(sieveBodies[i]);
    }
    cpSpaceRemoveShape(space, groundShape);
    cpSpaceRemoveShape(space, leftWallShape);
    cpSpaceRemoveShape(space, rightWallShape);
    cpShapeFree(groundShape);
    cpShapeFree(leftWallShape);
    cpShapeFree(rightWallShape);
    cpHastySpaceFree(space);

    CloseWindow();
    return 0;
}
