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
#define WAKE_UP_IMPULSE_THRESHOLD 50000.0f  // この衝撃を超えたら起こす
#define MIN_BREAK_SIZE 10.0f  // この大きさ以上の石を砕ける

typedef enum {
    DEBRIS_STONE,
    DEBRIS_WOOD
} DebrisType;

typedef struct {
    cpBody *body;
    cpShape *shape;
    cpVect vertices[MAX_VERTICES];
    int vertexCount;
    float size;
    float density;      // 密度 (石:0.7〜2.7, 木:0.7)
    DebrisType type;    // 石か木か
    bool active;
    bool isStatic;      // スタティック化されたか
    bool wakeUp;        // ダイナミックに戻すフラグ
    bool shouldBreak;   // 衝撃で割れるフラグ
    int breakFlash;     // 割れた直後の黄色表示フレーム数
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

// ポリゴンの面積を計算（Shoelace formula）
static float calcPolygonArea(cpVect *verts, int count) {
    float area = 0.0f;
    for (int i = 0; i < count; i++) {
        int j = (i + 1) % count;
        area += verts[i].x * verts[j].y;
        area -= verts[j].x * verts[i].y;
    }
    return fabsf(area) * 0.5f;
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

    // サイズ: 2〜30 (小さいものが多い分布)
    float size = 2.0f + powf(rand01(), 2.0f) * 28.0f;
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

    // 密度と質量を計算
    float density = 0.7f + rand01() * 2.0f;  // 0.7〜2.7
    float area = calcPolygonArea(points, vertexCount);
    cpFloat mass = area * density;
    cpFloat moment = cpMomentForPoly(mass, vertexCount, points, cpvzero, 0.0f);
    cpBody *body = cpBodyNew(mass, moment);
    cpBodySetPosition(body, cpv(x, y));
    cpBodySetAngle(body, rand01() * 2.0f * PI);
    cpSpaceAddBody(space, body);

    cpShape *shape = cpPolyShapeNew(body, vertexCount, points, cpTransformIdentity, 0.0f);
    cpShapeSetFriction(shape, 0.5f);
    // 小さいほど跳ねる: サイズ2で0.8、サイズ30で0.1
    float elasticity = 0.8f - (size - 2.0f) / 28.0f * 0.7f;
    if (elasticity < 0.01f) elasticity = 0.01f;
    if (elasticity > 1.0f) elasticity = 1.0f;
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
    debris[slot].density = density;
    debris[slot].type = DEBRIS_STONE;
    debris[slot].active = true;
    debris[slot].isStatic = false;
    debris[slot].wakeUp = false;
    debris[slot].shouldBreak = false;
    debris[slot].breakFlash = 0;
    debris[slot].idleTime = 0.0f;
    debris[slot].lastPos = cpv(x, y);
    if (slot >= debrisCount) debrisCount = slot + 1;
}

// 木片を生成（長方形、枝っぽい形状）
static void spawnWood(void) {
    int slot = findFreeSlot();
    if (slot < 0) return;

    // サイズ: 10〜60（大きめ）
    float size = 10.0f + rand01() * 50.0f;
    float x = FIELD_WIDTH / 2.0f - 200.0f + rand01() * 300.0f;
    float y = -50.0f;

    // 枝っぽい縦横比（長さ:幅 = 3:1 〜 8:1）
    float aspectRatio = 3.0f + rand01() * 5.0f;
    float length = size;
    float width = size / aspectRatio;
    if (width < 1.5f) width = 1.5f;

    // 長方形の頂点
    cpVect points[4];
    float hw = length / 2.0f;
    float hh = width / 2.0f;
    points[0] = cpv(-hw, -hh);
    points[1] = cpv(-hw, hh);
    points[2] = cpv(hw, hh);
    points[3] = cpv(hw, -hh);

    // 密度と質量
    float density = 0.7f;
    float area = length * width;
    cpFloat mass = area * density;
    cpFloat moment = cpMomentForPoly(mass, 4, points, cpvzero, 0.0f);
    cpBody *body = cpBodyNew(mass, moment);
    cpBodySetPosition(body, cpv(x, y));
    cpBodySetAngle(body, rand01() * 2.0f * PI);
    cpSpaceAddBody(space, body);

    cpShape *shape = cpPolyShapeNew(body, 4, points, cpTransformIdentity, 0.0f);
    cpShapeSetFriction(shape, 0.6f);
    cpShapeSetElasticity(shape, 0.3f);  // 木は跳ねにくい
    cpShapeSetCollisionType(shape, COLLISION_TYPE_DEBRIS);
    cpShapeSetUserData(shape, (void*)(intptr_t)slot);
    cpSpaceAddShape(space, shape);

    debris[slot].body = body;
    debris[slot].shape = shape;
    debris[slot].vertexCount = 4;
    for (int i = 0; i < 4; i++) {
        debris[slot].vertices[i] = points[i];
    }
    debris[slot].size = size;
    debris[slot].density = density;
    debris[slot].type = DEBRIS_WOOD;
    debris[slot].active = true;
    debris[slot].isStatic = false;
    debris[slot].wakeUp = false;
    debris[slot].shouldBreak = false;
    debris[slot].breakFlash = 0;
    debris[slot].idleTime = 0.0f;
    debris[slot].lastPos = cpv(x, y);
    if (slot >= debrisCount) debrisCount = slot + 1;
}

// 指定位置に指定サイズ・密度の破片を生成
static void spawnDebrisAt(float x, float y, float size, float density) {
    int slot = findFreeSlot();
    if (slot < 0) return;
    if (size < 2.0f) size = 2.0f;  // 最小サイズ保証

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

    // 面積と質量を計算
    float area = calcPolygonArea(points, vertexCount);
    cpFloat mass = area * density;
    cpFloat moment = cpMomentForPoly(mass, vertexCount, points, cpvzero, 0.0f);
    cpBody *body = cpBodyNew(mass, moment);
    cpBodySetPosition(body, cpv(x, y));
    cpBodySetAngle(body, rand01() * 2.0f * PI);
    // 破片に少し速度を与える
    float angle = rand01() * 2.0f * PI;
    float speed = 50.0f + rand01() * 100.0f;
    cpBodySetVelocity(body, cpv(speed * cosf(angle), speed * sinf(angle) - 50.0f));
    cpSpaceAddBody(space, body);

    cpShape *shape = cpPolyShapeNew(body, vertexCount, points, cpTransformIdentity, 0.0f);
    cpShapeSetFriction(shape, 0.5f);
    float elasticity = 0.8f - (size - 2.0f) / 28.0f * 0.7f;
    if (elasticity < 0.01f) elasticity = 0.01f;
    if (elasticity > 1.0f) elasticity = 1.0f;
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
    debris[slot].density = density;
    debris[slot].type = DEBRIS_STONE;
    debris[slot].active = true;
    debris[slot].isStatic = false;
    debris[slot].wakeUp = false;
    debris[slot].shouldBreak = false;
    debris[slot].breakFlash = 5;  // 破片は黄色で表示
    debris[slot].idleTime = 0.0f;
    debris[slot].lastPos = cpv(x, y);
    if (slot >= debrisCount) debrisCount = slot + 1;
}

// 石を砕く（面積を保存）
static void breakDebris(int index) {
    if (!debris[index].active) return;
    if (debris[index].type != DEBRIS_STONE) return;  // 木片は割れない
    if (debris[index].size < MIN_BREAK_SIZE) return;

    float originalSize = debris[index].size;
    // 実際のポリゴン面積を計算
    float originalArea = calcPolygonArea(debris[index].vertices, debris[index].vertexCount);

    // 元の位置を取得
    cpVect pos;
    if (debris[index].isStatic) {
        pos = debris[index].staticPos;
    } else {
        pos = cpBodyGetPosition(debris[index].body);
    }

    // 元の石を削除
    cpSpaceRemoveShape(space, debris[index].shape);
    cpShapeFree(debris[index].shape);
    if (!debris[index].isStatic && debris[index].body) {
        cpSpaceRemoveBody(space, debris[index].body);
        cpBodyFree(debris[index].body);
    }
    debris[index].active = false;
    debris[index].body = NULL;
    debris[index].shape = NULL;

    // 破片のサイズを決定（合計面積 = 元の面積）
    // 平均的な多角形の面積 ≈ 2 * radius^2（3〜7角形の平均）
    float remainingArea = originalArea;
    float sizes[20];
    int numPieces = 0;
    float areaFactor = 2.0f;  // 多角形面積係数

    while (remainingArea > 4.0f * areaFactor && numPieces < 20) {
        // 残り面積からランダムにサイズを決定
        float maxSize = sqrtf(remainingArea / areaFactor);
        if (maxSize > 30.0f) maxSize = 30.0f;
        float minSize = 2.0f;

        float pieceSize;
        if (remainingArea < 8.0f * areaFactor) {
            // 残りが少なければ全部使う
            pieceSize = sqrtf(remainingArea / areaFactor);
            if (pieceSize < 2.0f) pieceSize = 2.0f;
        } else {
            // ランダムなサイズ（小さめに偏らせる）
            pieceSize = minSize + powf(rand01(), 0.5f) * (maxSize - minSize);
        }

        sizes[numPieces++] = pieceSize;
        remainingArea -= pieceSize * pieceSize * areaFactor;
    }

    // 破片を生成（元の石の密度を継承）
    float density = debris[index].density;
    for (int i = 0; i < numPieces; i++) {
        float offsetX = (rand01() - 0.5f) * originalSize;
        float offsetY = (rand01() - 0.5f) * originalSize;
        spawnDebrisAt((float)pos.x + offsetX, (float)pos.y + offsetY, sizes[i], density);
    }
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

// 衝突後のコールバック: スタティックな石に大きな衝撃があったら起こす＆衝撃で割れる
static void postSolveHandler(cpArbiter *arb, cpSpace *space, void *data) {
    (void)space;
    (void)data;

    cpShape *a, *b;
    cpArbiterGetShapes(arb, &a, &b);

    // 衝撃力を計算
    cpFloat impulse = cpvlength(cpArbiterTotalImpulse(arb));

    // 両方のシェイプをチェック
    cpShape *shapes[2] = {a, b};
    for (int s = 0; s < 2; s++) {
        if (cpShapeGetCollisionType(shapes[s]) != COLLISION_TYPE_DEBRIS) continue;

        int idx = (int)(intptr_t)cpShapeGetUserData(shapes[s]);
        if (idx < 0 || idx >= debrisCount) continue;
        if (!debris[idx].active) continue;

        // スタティックな石を起こす
        if (debris[idx].isStatic && impulse > WAKE_UP_IMPULSE_THRESHOLD) {
            debris[idx].wakeUp = true;
        }

        // 衝撃で割れる判定（サイズ3より大きい石のみ、木片は割れない）
        float size = debris[idx].size;
        if (size > 3.0f && !debris[idx].shouldBreak && debris[idx].type == DEBRIS_STONE) {
            // 質量を考慮した閾値（質量 = 面積 × 密度 ≈ 2*size^2 * density）
            float breakThreshold = size * size * size * 500.0f;
            if (impulse > breakThreshold) {
                // 確率で割れる（衝撃が閾値の2倍なら100%、閾値ギリギリなら低確率）
                float breakChance = (impulse - breakThreshold) / breakThreshold;
                if (breakChance > 1.0f) breakChance = 1.0f;
                if (rand01() < breakChance) {
                    debris[idx].shouldBreak = true;
                }
            }
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

        // ダイナミックボディを再作成（面積×密度で質量計算）
        float area = calcPolygonArea(debris[i].vertices, debris[i].vertexCount);
        cpFloat mass = area * debris[i].density;
        cpFloat moment = cpMomentForPoly(mass, debris[i].vertexCount, debris[i].vertices, cpvzero, 0.0f);
        cpBody *body = cpBodyNew(mass, moment);
        cpBodySetPosition(body, debris[i].staticPos);
        cpBodySetAngle(body, debris[i].staticAngle);
        cpSpaceAddBody(space, body);

        cpShape *shape = cpPolyShapeNew(body, debris[i].vertexCount, debris[i].vertices, cpTransformIdentity, 0.0f);
        cpShapeSetFriction(shape, 0.5f);
        float elasticity = 0.8f - (debris[i].size - 2.0f) / 28.0f * 0.7f;
        if (elasticity < 0.01f) elasticity = 0.01f;
        if (elasticity > 1.0f) elasticity = 1.0f;
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

// 衝撃で割れるフラグが立った石を処理
static void processBreakingDebris(void) {
    for (int i = 0; i < debrisCount; i++) {
        if (!debris[i].active) continue;

        // breakFlashを減らす
        if (debris[i].breakFlash > 0) {
            debris[i].breakFlash--;
        }

        // 割れる処理
        if (debris[i].shouldBreak) {
            debris[i].shouldBreak = false;
            breakDebris(i);
        }
    }
}

static Color getStoneColor(float size) {
    // サイズ 2〜30 を 0〜1 に正規化
    float t = (size - 2.0f) / 28.0f;
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    // 小さい: 明るい灰色(180)、大きい: 暗い灰色(80)
    unsigned char gray = (unsigned char)(180 - t * 100);
    return (Color){gray, gray, gray, 255};
}

static Color getWoodColor(float size) {
    // サイズに応じて茶色の濃さを変える (10〜60)
    float t = (size - 10.0f) / 50.0f;
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    // 明るい茶色(180,130,70)〜暗い茶色(100,60,30)
    return (Color){
        (unsigned char)(180 - t * 80),
        (unsigned char)(130 - t * 70),
        (unsigned char)(70 - t * 40),
        255
    };
}

static void drawDebris(int index) {
    if (!debris[index].active) return;

    Color color;
    if (debris[index].breakFlash > 0) {
        color = (Color){255, 220, 50, 255};  // 割れた直後は黄色
    } else if (debris[index].isStatic) {
        color = (Color){80, 80, 180, 255};  // スタティックは青
    } else if (cpBodyIsSleeping(debris[index].body)) {
        color = (Color){180, 80, 80, 255};  // スリープ中は赤
    } else if (debris[index].type == DEBRIS_WOOD) {
        color = getWoodColor(debris[index].size);
    } else {
        color = getStoneColor(debris[index].size);
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

        // 右クリックで石を砕く
        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
            float worldX = GetMouseX() + cameraX;
            float worldY = GetMouseY();
            cpPointQueryInfo info;
            cpShape *shape = cpSpacePointQueryNearest(space, cpv(worldX, worldY), 0.0f, CP_SHAPE_FILTER_ALL, &info);
            if (shape && cpShapeGetCollisionType(shape) == COLLISION_TYPE_DEBRIS) {
                int idx = (int)(intptr_t)cpShapeGetUserData(shape);
                if (idx >= 0 && idx < debrisCount && debris[idx].active) {
                    breakDebris(idx);
                }
            }
        }

        if (spawning) {
            spawnTimer += dt;
            if (spawnTimer > 0.016f) {
                // 3個生成、25%の確率で木片
                for (int i = 0; i < 3; i++) {
                    if (rand01() < 0.25f) {
                        spawnWood();
                    } else {
                        spawnDebris();
                    }
                }
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
        processBreakingDebris();

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
