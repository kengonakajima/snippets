#include "raylib.h"
#include "box2d/box2d.h"
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define SCREEN_WIDTH 1248
#define SCREEN_HEIGHT 980
#define SCALE 64.0f
#define PI 3.14159265358979323846f
#define MAX_DEBRIS 50000
#define SIEVE_COUNT 10
#define SIEVE_ANGLE (20.0f * PI / 180.0f)  // 20度傾斜
#define SIEVE_LENGTH 60.0f   // 板の長さ
#define SIEVE_THICKNESS 6.0f // 板の厚さ
#define SIEVE_GAP 10.0f      // 板間の隙間
#define SIEVE_FREQ 8.0f      // 振動周波数 Hz
#define SIEVE_AMP 3.0f       // 振動振幅 px

typedef struct {
    b2BodyId bodyId;
    b2Polygon shape;
    float size;
    bool active;
} Debris;

static Debris debris[MAX_DEBRIS];
static int debrisCount = 0;

static b2BodyId sieveIds[SIEVE_COUNT];
static b2Vec2 sieveBasePos[SIEVE_COUNT];
static b2BodyId groundId;

// 角度をソートするための比較関数
static int compareFloats(const void* a, const void* b) {
    float fa = *(const float*)a;
    float fb = *(const float*)b;
    return (fa > fb) - (fa < fb);
}

// 0〜1のランダム値
static float rand01(void) {
    return (float)rand() / (float)RAND_MAX;
}

// ランダムな凸多角形を生成
static b2Polygon createRandomPolygon(float size) {
    int vertexCount = 3 + rand() % 5;  // 3〜7角形
    float angles[7];
    b2Vec2 points[7];

    for (int i = 0; i < vertexCount; i++) {
        angles[i] = ((float)i / vertexCount + rand01() * 0.1f) * 2.0f * PI;
    }
    qsort(angles, vertexCount, sizeof(float), compareFloats);

    for (int i = 0; i < vertexCount; i++) {
        float r = size * (0.7f + rand01() * 0.3f);
        points[i].x = r * cosf(angles[i]);
        points[i].y = r * sinf(angles[i]);
    }

    b2Hull hull = b2ComputeHull(points, vertexCount);
    if (hull.count >= 3) {
        return b2MakePolygon(&hull, 0.0f);
    }
    return b2MakeBox(size, size);
}

// 空きスロットを探す
static int findFreeSlot(void) {
    for (int i = 0; i < MAX_DEBRIS; i++) {
        if (!debris[i].active) return i;
    }
    return -1;
}

// 土砂を生成
static void spawnDebris(b2WorldId worldId) {
    int slot = findFreeSlot();
    if (slot < 0) return;

    // 小さいものが多い分布: size = 3 + pow(rand01, 2) * 9 → 3〜12ピクセル
    float size = 3.0f + powf(rand01(), 2.0f) * 9.0f;

    // 生成位置: 左上から広めに
    float x = 80.0f + rand01() * 300.0f;
    float y = -50.0f;

    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = (b2Vec2){x, y};
    bodyDef.rotation = b2MakeRot(rand01() * 2.0f * PI);
    b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

    b2Polygon shape = createRandomPolygon(size);
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0f;
    b2CreatePolygonShape(bodyId, &shapeDef, &shape);

    debris[slot].bodyId = bodyId;
    debris[slot].shape = shape;
    debris[slot].size = size;
    debris[slot].active = true;
    if (slot >= debrisCount) debrisCount = slot + 1;
}

// 画面外の土砂を削除
static void cleanupDebris(b2WorldId worldId) {
    for (int i = 0; i < debrisCount; i++) {
        if (!debris[i].active) continue;
        b2Vec2 pos = b2Body_GetPosition(debris[i].bodyId);
        // 画面外チェック: 右、左のみ（下は地面がある）
        if (pos.x > SCREEN_WIDTH + 100 || pos.x < -100.0f) {
            b2DestroyBody(debris[i].bodyId);
            debris[i].active = false;
        }
    }
}

// サイズに応じた色を取得
static Color getDebrisColor(float size) {
    // サイズ 3〜12 を 0〜1 に正規化
    float t = (size - 3.0f) / 9.0f;
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    // 小さい: 薄茶色(210,180,140)、大きい: 灰色(100,100,100)
    return (Color){
        (unsigned char)(210 - t * 110),
        (unsigned char)(180 - t * 80),
        (unsigned char)(140 - t * 40),
        255
    };
}

// 土砂を描画
static void drawDebris(int index) {
    if (!debris[index].active) return;

    b2BodyId bodyId = debris[index].bodyId;
    b2Polygon* shape = &debris[index].shape;
    Color color = getDebrisColor(debris[index].size);

    Vector2 screenPoints[8];
    for (int i = 0; i < shape->count; i++) {
        b2Vec2 worldPt = b2Body_GetWorldPoint(bodyId, shape->vertices[i]);
        screenPoints[i] = (Vector2){worldPt.x, worldPt.y};
    }

    // 塗りつぶし
    for (int i = 1; i < shape->count - 1; i++) {
        DrawTriangle(screenPoints[0], screenPoints[i], screenPoints[i + 1], color);
    }
    // 輪郭
    Color outline = {color.r / 2, color.g / 2, color.b / 2, 255};
    for (int i = 0; i < shape->count; i++) {
        int next = (i + 1) % shape->count;
        DrawLineV(screenPoints[i], screenPoints[next], outline);
    }
}

// ふるい板を作成
static void createSieve(b2WorldId worldId) {
    float spacing = SIEVE_LENGTH + SIEVE_GAP;
    float startX = 100.0f;
    float startY = 150.0f;

    for (int i = 0; i < SIEVE_COUNT; i++) {
        float offsetX = i * spacing * cosf(SIEVE_ANGLE);
        float offsetY = i * spacing * sinf(SIEVE_ANGLE);

        sieveBasePos[i] = (b2Vec2){startX + offsetX, startY + offsetY};

        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2_kinematicBody;
        bodyDef.position = sieveBasePos[i];
        bodyDef.rotation = b2MakeRot(SIEVE_ANGLE);
        sieveIds[i] = b2CreateBody(worldId, &bodyDef);

        b2Polygon sieveBox = b2MakeBox(SIEVE_LENGTH / 2.0f, SIEVE_THICKNESS / 2.0f);
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        b2CreatePolygonShape(sieveIds[i], &shapeDef, &sieveBox);
    }
}

// ふるい板の振動を更新（速度ベース）
static void updateSieveVibration(float time) {
    // 速度 = d(position)/dt = d(A * sin(ωt))/dt = A * ω * cos(ωt)
    float omega = 2.0f * PI * SIEVE_FREQ;
    float velocityPhase = cosf(time * omega);
    float speed = SIEVE_AMP * omega * velocityPhase;

    // 振動方向: ふるい面の法線方向（斜め上下）
    float normalAngle = SIEVE_ANGLE + PI / 2.0f;
    float vx = speed * cosf(normalAngle);
    float vy = speed * sinf(normalAngle);

    b2Vec2 velocity = {vx, vy};
    for (int i = 0; i < SIEVE_COUNT; i++) {
        b2Body_SetLinearVelocity(sieveIds[i], velocity);
    }
}

// ふるい板を描画
static void drawSieve(void) {
    for (int i = 0; i < SIEVE_COUNT; i++) {
        b2Vec2 pos = b2Body_GetPosition(sieveIds[i]);
        b2Rot rot = b2Body_GetRotation(sieveIds[i]);
        float angle = b2Rot_GetAngle(rot);

        Rectangle rect = {pos.x, pos.y, SIEVE_LENGTH, SIEVE_THICKNESS};
        Vector2 origin = {SIEVE_LENGTH / 2.0f, SIEVE_THICKNESS / 2.0f};
        DrawRectanglePro(rect, origin, angle * 180.0f / PI, DARKGRAY);
    }
}

// 地面を作成
static void createGround(b2WorldId worldId) {
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.position = (b2Vec2){SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT - 10.0f};
    groundId = b2CreateBody(worldId, &bodyDef);

    b2Polygon groundBox = b2MakeBox(SCREEN_WIDTH / 2.0f, 10.0f);
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    b2CreatePolygonShape(groundId, &shapeDef, &groundBox);
}

// 地面を描画
static void drawGround(void) {
    b2Vec2 pos = b2Body_GetPosition(groundId);
    DrawRectangle(0, (int)(pos.y - 10), SCREEN_WIDTH, 20, DARKGRAY);
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sieve Simulation");
    SetTargetFPS(60);
    srand(42);

    // Box2D ワールド作成
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2){0.0f, 9.8f * SCALE};
    b2WorldId worldId = b2CreateWorld(&worldDef);

    // ふるい板を作成
    createSieve(worldId);

    // 地面を作成
    createGround(worldId);

    float spawnTimer = 0.0f;
    float physicsTimeMs = 0.0f;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // 土砂生成（複数個同時に生成）
        spawnTimer += dt;
        if (spawnTimer > 0.016f) {  // 約60fpsで3個ずつ
            spawnDebris(worldId);
            spawnDebris(worldId);
            spawnDebris(worldId);
            spawnTimer = 0.0f;
        }

        // ふるい板の振動を更新
        updateSieveVibration((float)GetTime());

        // 物理シミュレーション更新（時間計測）
        // サブステップを増やしてめり込みを防ぐ
        double startTime = GetTime();
        b2World_Step(worldId, dt, 8);
        double endTime = GetTime();
        physicsTimeMs = (float)((endTime - startTime) * 1000.0);

        // 画面外の土砂を削除
        cleanupDebris(worldId);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // 地面を描画
        drawGround();

        // ふるい板を描画
        drawSieve();

        // 土砂を描画
        for (int i = 0; i < debrisCount; i++) {
            drawDebris(i);
        }

        // 統計表示
        int activeCount = 0;
        for (int i = 0; i < debrisCount; i++) {
            if (debris[i].active) activeCount++;
        }
        DrawRectangle(5, 5, 200, 75, (Color){255, 255, 255, 200});
        DrawFPS(10, 10);
        DrawText(TextFormat("Debris: %d", activeCount), 10, 35, 20, BLACK);
        DrawText(TextFormat("Physics: %.2f ms", physicsTimeMs), 10, 55, 20, BLACK);

        EndDrawing();
    }

    b2DestroyWorld(worldId);
    CloseWindow();
    return 0;
}
