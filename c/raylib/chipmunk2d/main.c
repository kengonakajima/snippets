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
    DEBRIS_WOOD,
    DEBRIS_CLAY
} DebrisType;

#define MAX_STICKY_JOINTS 10000
typedef struct {
    cpConstraint *joint;
    int debrisA;
    int debrisB;
    bool active;
} StickyJoint;

static StickyJoint stickyJoints[MAX_STICKY_JOINTS];
static int stickyJointCount = 0;

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
static float cameraY = 0.0f;
static float zoom = 1.0f;

// 操作モード (1: 破壊, 2: ドラッグ移動, 3: 水配置, 4: コンベア, 5: Output, 6: ふるい)
static int actionMode = 1;
static const char* actionModeNames[] = {"", "Break", "Drag", "Water", "Conveyor", "Output", "Sieve"};

// ドラッグ移動用
static int draggedDebrisIdx = -1;
static cpVect dragOffset = {0, 0};

// 水
#define MAX_WATER 100
#define WATER_SIZE 200.0f
typedef struct {
    float x, y;
    bool active;
} Water;
static Water waters[MAX_WATER];
static int waterCount = 0;

// コンベア
#define MAX_CONVEYOR 50
#define CONVEYOR_LENGTH 300.0f
#define CONVEYOR_THICKNESS 10.0f
#define CONVEYOR_DEFAULT_ANGLE (-30.0f * PI / 180.0f)  // デフォルト30度斜め上向き
#define CONVEYOR_FORCE 500000.0f  // コンベアが物体に加える力
#define CONVEYOR_MAX_SPEED 150.0f  // コンベア上の最大速度
#define CONVEYOR_HANDLE_RADIUS 15.0f  // 回転用ハンドルの半径
#define ROTATE_SPEED 0.02f  // 回転速度
typedef struct {
    cpBody *body;
    cpShape *shape;
    float x, y;
    float angle;
    bool active;
    bool reversed;  // 力の方向を反転
} Conveyor;
static Conveyor conveyors[MAX_CONVEYOR];
static int conveyorCount = 0;

// Output（排出口）
#define MAX_OUTPUT 20
#define OUTPUT_SIZE 200.0f
#define OUTPUT_WALL_THICKNESS 10.0f
#define OUTPUT_FILL_THRESHOLD 0.5f  // 50%で排出
typedef struct {
    float x, y;
    cpShape *leftWall;
    cpShape *rightWall;
    cpShape *bottomWall;
    bool active;
} Output;
static Output outputs[MAX_OUTPUT];
static int outputCount = 0;

// ユーザー配置ふるい
#define MAX_USER_SIEVE 50
typedef struct {
    cpBody *body;
    cpShape *shape;
    cpVect basePos;
    bool active;
} UserSieve;
static UserSieve userSieves[MAX_USER_SIEVE];
static int userSieveCount = 0;

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

// 粘土質の石を生成（他の石にくっつく）
static void spawnClay(void) {
    int slot = findFreeSlot();
    if (slot < 0) return;

    // サイズ: 5〜20（中くらい）
    float size = 5.0f + rand01() * 15.0f;
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

    // 粘土は密度高め
    float density = 1.8f;
    float area = calcPolygonArea(points, vertexCount);
    cpFloat mass = area * density;
    cpFloat moment = cpMomentForPoly(mass, vertexCount, points, cpvzero, 0.0f);
    cpBody *body = cpBodyNew(mass, moment);
    cpBodySetPosition(body, cpv(x, y));
    cpBodySetAngle(body, rand01() * 2.0f * PI);
    cpSpaceAddBody(space, body);

    cpShape *shape = cpPolyShapeNew(body, vertexCount, points, cpTransformIdentity, 0.0f);
    cpShapeSetFriction(shape, 0.9f);  // 高摩擦
    cpShapeSetElasticity(shape, 0.05f);  // ほとんど跳ねない
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
    debris[slot].type = DEBRIS_CLAY;
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

    // 接続されているジョイントを削除
    for (int j = 0; j < stickyJointCount; j++) {
        if (!stickyJoints[j].active) continue;
        if (stickyJoints[j].debrisA == index || stickyJoints[j].debrisB == index) {
            cpSpaceRemoveConstraint(space, stickyJoints[j].joint);
            cpConstraintFree(stickyJoints[j].joint);
            stickyJoints[j].active = false;
        }
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

// 指定debrisに接続されているジョイントを全て削除
static void removeJointsForDebris(int idx) {
    for (int i = 0; i < stickyJointCount; i++) {
        if (!stickyJoints[i].active) continue;
        if (stickyJoints[i].debrisA == idx || stickyJoints[i].debrisB == idx) {
            cpSpaceRemoveConstraint(space, stickyJoints[i].joint);
            cpConstraintFree(stickyJoints[i].joint);
            stickyJoints[i].active = false;
        }
    }
}

static void cleanupDebris(void) {
    for (int i = 0; i < debrisCount; i++) {
        if (!debris[i].active || debris[i].isStatic) continue;
        cpVect pos = cpBodyGetPosition(debris[i].body);
        if (pos.x > FIELD_WIDTH + 100 || pos.x < -100.0f || pos.y > SCREEN_HEIGHT + 100) {
            removeJointsForDebris(i);
            cpSpaceRemoveShape(space, debris[i].shape);
            cpSpaceRemoveBody(space, debris[i].body);
            cpShapeFree(debris[i].shape);
            cpBodyFree(debris[i].body);
            debris[i].active = false;
        }
    }
}

// 位置が水中かチェック
static bool isInWater(cpVect pos) {
    for (int w = 0; w < waterCount; w++) {
        if (!waters[w].active) continue;
        if (pos.x >= waters[w].x && pos.x <= waters[w].x + WATER_SIZE &&
            pos.y >= waters[w].y && pos.y <= waters[w].y + WATER_SIZE) {
            return true;
        }
    }
    return false;
}

// 地面またはスタティックなデブリに接触しているかチェック用コールバック
static bool touchingGroundOrStatic;
static void checkGroundOrStaticContact(cpBody *body, cpArbiter *arb, void *data) {
    (void)body;
    (void)data;

    cpShape *a, *b;
    cpArbiterGetShapes(arb, &a, &b);

    // 地面に接触しているか（スタティックボディのシェイプ）
    cpBody *bodyA = cpShapeGetBody(a);
    cpBody *bodyB = cpShapeGetBody(b);

    // 相手がスタティックボディか確認（キネマティックボディ=コンベア/ふるいは除外）
    if (cpBodyGetType(bodyA) == CP_BODY_TYPE_KINEMATIC || cpBodyGetType(bodyB) == CP_BODY_TYPE_KINEMATIC) {
        return;  // コンベアやふるいの上ではスタティック化しない
    }
    if (cpBodyGetType(bodyA) == CP_BODY_TYPE_STATIC || cpBodyGetType(bodyB) == CP_BODY_TYPE_STATIC) {
        touchingGroundOrStatic = true;
        return;
    }

    // 相手がスタティック化されたデブリか確認
    if (cpShapeGetCollisionType(a) == COLLISION_TYPE_DEBRIS) {
        int idx = (int)(intptr_t)cpShapeGetUserData(a);
        if (idx >= 0 && idx < debrisCount && debris[idx].active && debris[idx].isStatic) {
            touchingGroundOrStatic = true;
            return;
        }
    }
    if (cpShapeGetCollisionType(b) == COLLISION_TYPE_DEBRIS) {
        int idx = (int)(intptr_t)cpShapeGetUserData(b);
        if (idx >= 0 && idx < debrisCount && debris[idx].active && debris[idx].isStatic) {
            touchingGroundOrStatic = true;
            return;
        }
    }
}

// 安定した石をスタティック化
static void staticizeDebris(float dt) {
    cpBody *staticBody = cpSpaceGetStaticBody(space);
    for (int i = 0; i < debrisCount; i++) {
        if (!debris[i].active || debris[i].isStatic || !debris[i].body) continue;
        if (i == draggedDebrisIdx) continue;  // ドラッグ中はスキップ

        cpVect pos = cpBodyGetPosition(debris[i].body);

        // 水中の石はスタティック化しない
        if (isInWater(pos)) {
            debris[i].idleTime = 0.0f;
            debris[i].lastPos = pos;
            continue;
        }

        // 地面またはスタティックなデブリに接触しているかチェック
        touchingGroundOrStatic = false;
        cpBodyEachArbiter(debris[i].body, checkGroundOrStaticContact, NULL);
        if (!touchingGroundOrStatic) {
            debris[i].idleTime = 0.0f;
            debris[i].lastPos = pos;
            continue;
        }

        cpVect diff = cpvsub(pos, debris[i].lastPos);
        float moved = cpvlength(diff);

        // ほとんど動いていない場合
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

            // 接続されているジョイントを削除（ボディ解放前に必須）
            removeJointsForDebris(i);

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

// 粘土のくっつき予約用
#define MAX_PENDING_STICKY 100
static struct { int a; int b; cpVect point; } pendingSticky[MAX_PENDING_STICKY];
static int pendingStickyCount = 0;

// 既にジョイントがあるかチェック
static bool hasJointBetween(int idxA, int idxB) {
    for (int i = 0; i < stickyJointCount; i++) {
        if (!stickyJoints[i].active) continue;
        if ((stickyJoints[i].debrisA == idxA && stickyJoints[i].debrisB == idxB) ||
            (stickyJoints[i].debrisA == idxB && stickyJoints[i].debrisB == idxA)) {
            return true;
        }
    }
    return false;
}

// 衝突後のコールバック: スタティックな石に大きな衝撃があったら起こす＆衝撃で割れる
static void postSolveHandler(cpArbiter *arb, cpSpace *sp, void *data) {
    (void)sp;
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

    // 粘土のくっつき判定
    if (cpShapeGetCollisionType(a) == COLLISION_TYPE_DEBRIS &&
        cpShapeGetCollisionType(b) == COLLISION_TYPE_DEBRIS) {
        int idxA = (int)(intptr_t)cpShapeGetUserData(a);
        int idxB = (int)(intptr_t)cpShapeGetUserData(b);
        if (idxA >= 0 && idxA < debrisCount && idxB >= 0 && idxB < debrisCount) {
            if (debris[idxA].active && debris[idxB].active &&
                !debris[idxA].isStatic && !debris[idxB].isStatic) {
                // どちらかが粘土ならくっつく
                if (debris[idxA].type == DEBRIS_CLAY || debris[idxB].type == DEBRIS_CLAY) {
                    if (!hasJointBetween(idxA, idxB) && pendingStickyCount < MAX_PENDING_STICKY) {
                        cpContactPointSet contacts = cpArbiterGetContactPointSet(arb);
                        if (contacts.count > 0) {
                            pendingSticky[pendingStickyCount].a = idxA;
                            pendingSticky[pendingStickyCount].b = idxB;
                            pendingSticky[pendingStickyCount].point = contacts.points[0].pointA;
                            pendingStickyCount++;
                        }
                    }
                }
            }
        }
    }
}

// 粘土のくっつきを処理
static void processPendingSticky(void) {
    for (int i = 0; i < pendingStickyCount; i++) {
        int idxA = pendingSticky[i].a;
        int idxB = pendingSticky[i].b;
        if (!debris[idxA].active || !debris[idxB].active) continue;
        if (debris[idxA].isStatic || debris[idxB].isStatic) continue;
        if (!debris[idxA].body || !debris[idxB].body) continue;
        if (hasJointBetween(idxA, idxB)) continue;

        // 空きスロットを探す
        int slot = -1;
        for (int j = 0; j < stickyJointCount; j++) {
            if (!stickyJoints[j].active) { slot = j; break; }
        }
        if (slot < 0 && stickyJointCount < MAX_STICKY_JOINTS) {
            slot = stickyJointCount++;
        }
        if (slot < 0) continue;

        // ピボットジョイントを作成
        cpVect point = pendingSticky[i].point;
        cpConstraint *joint = cpPivotJointNew(debris[idxA].body, debris[idxB].body, point);
        cpConstraintSetMaxForce(joint, 500000.0f);  // 強めに接着
        cpConstraintSetCollideBodies(joint, cpTrue);
        cpSpaceAddConstraint(space, joint);

        stickyJoints[slot].joint = joint;
        stickyJoints[slot].debrisA = idxA;
        stickyJoints[slot].debrisB = idxB;
        stickyJoints[slot].active = true;
    }
    pendingStickyCount = 0;

    // 伸びすぎたジョイントを切る
    float maxDist = 50.0f;
    for (int i = 0; i < stickyJointCount; i++) {
        if (!stickyJoints[i].active) continue;
        int idxA = stickyJoints[i].debrisA;
        int idxB = stickyJoints[i].debrisB;
        if (!debris[idxA].active || !debris[idxB].active ||
            !debris[idxA].body || !debris[idxB].body) {
            cpSpaceRemoveConstraint(space, stickyJoints[i].joint);
            cpConstraintFree(stickyJoints[i].joint);
            stickyJoints[i].active = false;
            continue;
        }
        cpVect posA = cpBodyGetPosition(debris[idxA].body);
        cpVect posB = cpBodyGetPosition(debris[idxB].body);
        float dist = cpvlength(cpvsub(posA, posB));
        if (dist > maxDist) {
            cpSpaceRemoveConstraint(space, stickyJoints[i].joint);
            cpConstraintFree(stickyJoints[i].joint);
            stickyJoints[i].active = false;
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

static Color getClayColor(float size) {
    // サイズに応じてオレンジ〜茶色 (5〜20)
    float t = (size - 5.0f) / 15.0f;
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    // オレンジ(220,140,80)〜暗めの茶(160,90,50)
    return (Color){
        (unsigned char)(220 - t * 60),
        (unsigned char)(140 - t * 50),
        (unsigned char)(80 - t * 30),
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
    } else if (debris[index].type == DEBRIS_CLAY) {
        color = getClayColor(debris[index].size);
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
            screenPoints[i] = (Vector2){(float)worldPt.x, (float)worldPt.y};
        }
    } else {
        for (int i = 0; i < debris[index].vertexCount; i++) {
            cpVect worldPt = cpBodyLocalToWorld(debris[index].body, debris[index].vertices[i]);
            screenPoints[i] = (Vector2){(float)worldPt.x, (float)worldPt.y};
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

// ジョイントを線で描画
static void drawStickyJoints(void) {
    for (int i = 0; i < stickyJointCount; i++) {
        if (!stickyJoints[i].active) continue;
        int idxA = stickyJoints[i].debrisA;
        int idxB = stickyJoints[i].debrisB;
        if (!debris[idxA].active || !debris[idxB].active) continue;
        if (!debris[idxA].body || !debris[idxB].body) continue;

        cpVect posA = cpBodyGetPosition(debris[idxA].body);
        cpVect posB = cpBodyGetPosition(debris[idxB].body);
        Vector2 screenA = {(float)posA.x, (float)posA.y};
        Vector2 screenB = {(float)posB.x, (float)posB.y};
        DrawLineEx(screenA, screenB, 2.0f, (Color){255, 100, 100, 200});
    }
}

// 水を描画
static void drawWater(void) {
    for (int i = 0; i < waterCount; i++) {
        if (!waters[i].active) continue;
        DrawRectangle((int)waters[i].x, (int)waters[i].y, (int)WATER_SIZE, (int)WATER_SIZE,
            (Color){100, 150, 255, 120});
        DrawRectangleLines((int)waters[i].x, (int)waters[i].y, (int)WATER_SIZE, (int)WATER_SIZE,
            (Color){50, 100, 200, 200});
    }
}

// 水にいるかチェックし、浮力を適用
static void applyBuoyancy(void) {
    for (int i = 0; i < debrisCount; i++) {
        if (!debris[i].active || debris[i].isStatic || !debris[i].body) continue;

        cpVect pos = cpBodyGetPosition(debris[i].body);

        // 全ての水をチェック
        for (int w = 0; w < waterCount; w++) {
            if (!waters[w].active) continue;

            // 水の範囲内にいるか
            if (pos.x >= waters[w].x && pos.x <= waters[w].x + WATER_SIZE &&
                pos.y >= waters[w].y && pos.y <= waters[w].y + WATER_SIZE) {

                // 粘土は水に触れるとくっつかなくなる（石に変化）
                if (debris[i].type == DEBRIS_CLAY) {
                    debris[i].type = DEBRIS_STONE;
                    // 接続されているジョイントを削除
                    removeJointsForDebris(i);
                }

                // 密度1未満なら浮力を適用
                if (debris[i].density < 1.0f) {
                    float area = calcPolygonArea(debris[i].vertices, debris[i].vertexCount);
                    // 浮力 = (1 - 密度) * 面積 * 重力（上向き）
                    float buoyancy = (1.0f - debris[i].density) * area * 9.8f * SCALE * 5.0f;
                    // 現在の力に浮力を加算（回転させない）
                    cpVect currentForce = cpBodyGetForce(debris[i].body);
                    cpBodySetForce(debris[i].body, cpv(currentForce.x, currentForce.y - buoyancy));
                    // 水の抵抗（強め）
                    cpVect vel = cpBodyGetVelocity(debris[i].body);
                    cpBodySetVelocity(debris[i].body, cpvmult(vel, 0.85f));
                    // 回転も減衰
                    cpBodySetAngularVelocity(debris[i].body, cpBodyGetAngularVelocity(debris[i].body) * 0.85f);
                }
            }
        }
    }
}

// コンベアの力を適用
static void applyConveyorForce(void) {
    for (int i = 0; i < debrisCount; i++) {
        if (!debris[i].active || debris[i].isStatic || !debris[i].body) continue;

        // 各コンベアとの接触をチェック
        for (int c = 0; c < conveyorCount; c++) {
            if (!conveyors[c].active) continue;

            // コンベアとの衝突判定
            cpContactPointSet contacts = cpShapesCollide(debris[i].shape, conveyors[c].shape);
            if (contacts.count > 0) {
                float angle = (float)cpBodyGetAngle(conveyors[c].body);
                cpVect conveyorDir = cpv(cosf(angle), sinf(angle));

                // 反転フラグで方向を決定
                if (conveyors[c].reversed) {
                    conveyorDir = cpvneg(conveyorDir);
                }

                // 接触点の速度をチェック（回転による速度も含む）
                cpVect contactPoint = contacts.points[0].pointA;
                cpVect velAtContact = cpBodyGetVelocityAtWorldPoint(debris[i].body, contactPoint);
                float speedInDir = (float)cpvdot(velAtContact, conveyorDir);

                // 最大速度未満なら力を加える
                if (speedInDir < CONVEYOR_MAX_SPEED) {
                    cpFloat mass = cpBodyGetMass(debris[i].body);
                    cpVect force = cpvmult(conveyorDir, CONVEYOR_FORCE * mass / 100.0f);
                    // 接触点に力を加える（回転も発生する）
                    cpBodyApplyForceAtWorldPoint(debris[i].body, force, contactPoint);
                }
                break;  // 1つのコンベアからのみ力を受ける
            }
        }
    }
}

// 水を配置
static void placeWater(float x, float y) {
    // 空きスロットを探す
    int slot = -1;
    for (int i = 0; i < waterCount; i++) {
        if (!waters[i].active) { slot = i; break; }
    }
    if (slot < 0 && waterCount < MAX_WATER) {
        slot = waterCount++;
    }
    if (slot < 0) return;

    waters[slot].x = x - WATER_SIZE / 2.0f;
    waters[slot].y = y - WATER_SIZE / 2.0f;
    waters[slot].active = true;
}

// コンベアを配置
static void placeConveyor(float x, float y) {
    // 空きスロットを探す
    int slot = -1;
    for (int i = 0; i < conveyorCount; i++) {
        if (!conveyors[i].active) { slot = i; break; }
    }
    if (slot < 0 && conveyorCount < MAX_CONVEYOR) {
        slot = conveyorCount++;
    }
    if (slot < 0) return;

    // キネマティックボディを使用（回転できるように）
    cpBody *body = cpBodyNewKinematic();
    cpBodySetPosition(body, cpv(x, y));
    cpBodySetAngle(body, CONVEYOR_DEFAULT_ANGLE);
    cpSpaceAddBody(space, body);

    // コンベアの形状
    cpFloat hw = CONVEYOR_LENGTH / 2.0f;
    cpFloat hh = CONVEYOR_THICKNESS / 2.0f;
    cpVect verts[4] = {
        cpv(-hw, -hh), cpv(-hw, hh), cpv(hw, hh), cpv(hw, -hh)
    };

    cpShape *shape = cpPolyShapeNew(body, 4, verts, cpTransformIdentity, 0.0f);
    cpShapeSetFriction(shape, 1.0f);  // 高摩擦
    // 表面速度は使わず、力だけで動かす（回転に正しく追従させるため）

    cpSpaceAddShape(space, shape);

    conveyors[slot].body = body;
    conveyors[slot].shape = shape;
    conveyors[slot].x = x;
    conveyors[slot].y = y;
    conveyors[slot].angle = CONVEYOR_DEFAULT_ANGLE;
    conveyors[slot].active = true;
    conveyors[slot].reversed = false;
}

// コンベアを描画
static void drawConveyors(void) {
    for (int i = 0; i < conveyorCount; i++) {
        if (!conveyors[i].active) continue;

        cpVect pos = cpBodyGetPosition(conveyors[i].body);
        float angle = (float)cpBodyGetAngle(conveyors[i].body);

        // コンベア本体
        Rectangle rect = {(float)pos.x, (float)pos.y, CONVEYOR_LENGTH, CONVEYOR_THICKNESS};
        Vector2 origin = {CONVEYOR_LENGTH / 2.0f, CONVEYOR_THICKNESS / 2.0f};
        DrawRectanglePro(rect, origin, angle * 180.0f / PI, BLACK);

        // 中心に回転用の丸を描画
        float cx = (float)pos.x;
        float cy = (float)pos.y;
        DrawCircle((int)cx, (int)cy, CONVEYOR_HANDLE_RADIUS, DARKGRAY);
        DrawCircleLines((int)cx, (int)cy, CONVEYOR_HANDLE_RADIUS, WHITE);

        // 矢印を描画（進行方向を示す）
        float arrowLen = 30.0f;
        float drawAngle = angle;
        if (conveyors[i].reversed) {
            drawAngle += PI;  // 反転時は矢印を逆向きに
        }
        float dx = cosf(drawAngle) * arrowLen;
        float dy = sinf(drawAngle) * arrowLen;
        // 矢印の先端
        Vector2 tip = {cx + dx, cy + dy};
        // 矢印の根元
        Vector2 base = {cx - dx, cy - dy};
        DrawLineEx(base, tip, 3.0f, WHITE);
        // 矢じり
        float arrowHead = 10.0f;
        float headAngle = drawAngle + PI * 0.8f;
        Vector2 head1 = {tip.x + cosf(headAngle) * arrowHead, tip.y + sinf(headAngle) * arrowHead};
        headAngle = drawAngle - PI * 0.8f;
        Vector2 head2 = {tip.x + cosf(headAngle) * arrowHead, tip.y + sinf(headAngle) * arrowHead};
        DrawLineEx(tip, head1, 3.0f, WHITE);
        DrawLineEx(tip, head2, 3.0f, WHITE);
    }
}

// Outputを配置
static void placeOutput(float x, float y) {
    int slot = -1;
    for (int i = 0; i < outputCount; i++) {
        if (!outputs[i].active) { slot = i; break; }
    }
    if (slot < 0 && outputCount < MAX_OUTPUT) {
        slot = outputCount++;
    }
    if (slot < 0) return;

    float ox = x - OUTPUT_SIZE / 2.0f;
    float oy = y - OUTPUT_SIZE / 2.0f;
    outputs[slot].x = ox;
    outputs[slot].y = oy;

    cpBody *staticBody = cpSpaceGetStaticBody(space);
    cpFloat hw = OUTPUT_WALL_THICKNESS / 2.0f;
    cpFloat hh = OUTPUT_SIZE / 2.0f;

    // 左壁
    cpVect leftVerts[4] = {
        cpv(-hw, -hh), cpv(-hw, hh), cpv(hw, hh), cpv(hw, -hh)
    };
    cpTransform leftT = cpTransformTranslate(cpv(ox, oy + OUTPUT_SIZE / 2.0f));
    outputs[slot].leftWall = cpPolyShapeNew(staticBody, 4, leftVerts, leftT, 0.0f);
    cpShapeSetFriction(outputs[slot].leftWall, 0.5f);
    cpSpaceAddShape(space, outputs[slot].leftWall);

    // 右壁
    cpTransform rightT = cpTransformTranslate(cpv(ox + OUTPUT_SIZE, oy + OUTPUT_SIZE / 2.0f));
    outputs[slot].rightWall = cpPolyShapeNew(staticBody, 4, leftVerts, rightT, 0.0f);
    cpShapeSetFriction(outputs[slot].rightWall, 0.5f);
    cpSpaceAddShape(space, outputs[slot].rightWall);

    // 底壁
    cpFloat bhw = OUTPUT_SIZE / 2.0f;
    cpFloat bhh = OUTPUT_WALL_THICKNESS / 2.0f;
    cpVect bottomVerts[4] = {
        cpv(-bhw, -bhh), cpv(-bhw, bhh), cpv(bhw, bhh), cpv(bhw, -bhh)
    };
    cpTransform bottomT = cpTransformTranslate(cpv(ox + OUTPUT_SIZE / 2.0f, oy + OUTPUT_SIZE));
    outputs[slot].bottomWall = cpPolyShapeNew(staticBody, 4, bottomVerts, bottomT, 0.0f);
    cpShapeSetFriction(outputs[slot].bottomWall, 0.5f);
    cpSpaceAddShape(space, outputs[slot].bottomWall);

    outputs[slot].active = true;
}

// Outputを描画
static void drawOutputs(void) {
    for (int i = 0; i < outputCount; i++) {
        if (!outputs[i].active) continue;
        float ox = outputs[i].x;
        float oy = outputs[i].y;

        // 背景（半透明）
        DrawRectangle((int)ox, (int)oy, (int)OUTPUT_SIZE, (int)OUTPUT_SIZE,
            (Color){128, 128, 128, 50});

        // 左壁
        DrawRectangle((int)(ox - OUTPUT_WALL_THICKNESS / 2.0f), (int)oy,
            (int)OUTPUT_WALL_THICKNESS, (int)OUTPUT_SIZE, DARKGRAY);
        // 右壁
        DrawRectangle((int)(ox + OUTPUT_SIZE - OUTPUT_WALL_THICKNESS / 2.0f), (int)oy,
            (int)OUTPUT_WALL_THICKNESS, (int)OUTPUT_SIZE, DARKGRAY);
        // 底壁
        DrawRectangle((int)ox, (int)(oy + OUTPUT_SIZE - OUTPUT_WALL_THICKNESS / 2.0f),
            (int)OUTPUT_SIZE, (int)OUTPUT_WALL_THICKNESS, DARKGRAY);

        DrawText("OUTPUT", (int)ox + 60, (int)oy + 90, 20, (Color){60, 60, 60, 200});
    }
}

// Output内のデブリをチェックして排出
static void checkAndClearOutputs(void) {
    float outputArea = OUTPUT_SIZE * OUTPUT_SIZE;

    for (int o = 0; o < outputCount; o++) {
        if (!outputs[o].active) continue;

        float ox = outputs[o].x;
        float oy = outputs[o].y;

        // このOutput内のデブリを集める
        int insideIndices[MAX_DEBRIS];
        int insideCount = 0;
        float totalArea = 0.0f;

        for (int i = 0; i < debrisCount; i++) {
            if (!debris[i].active) continue;

            cpVect pos;
            if (debris[i].isStatic) {
                pos = debris[i].staticPos;
            } else if (debris[i].body) {
                pos = cpBodyGetPosition(debris[i].body);
            } else {
                continue;
            }

            // Output内にいるか
            if (pos.x >= ox && pos.x <= ox + OUTPUT_SIZE &&
                pos.y >= oy && pos.y <= oy + OUTPUT_SIZE) {
                insideIndices[insideCount++] = i;
                totalArea += calcPolygonArea(debris[i].vertices, debris[i].vertexCount);
            }
        }

        // 面積比が閾値を超えたら全部消す
        if (totalArea / outputArea >= OUTPUT_FILL_THRESHOLD && insideCount > 0) {
            for (int j = 0; j < insideCount; j++) {
                int idx = insideIndices[j];
                if (!debris[idx].active) continue;

                // ジョイントを削除
                removeJointsForDebris(idx);

                // シェイプとボディを削除
                cpSpaceRemoveShape(space, debris[idx].shape);
                cpShapeFree(debris[idx].shape);
                if (!debris[idx].isStatic && debris[idx].body) {
                    cpSpaceRemoveBody(space, debris[idx].body);
                    cpBodyFree(debris[idx].body);
                }
                debris[idx].active = false;
                debris[idx].body = NULL;
                debris[idx].shape = NULL;
            }
        }
    }
}

// ユーザーふるいを配置
static void placeUserSieve(float x, float y) {
    int slot = -1;
    for (int i = 0; i < userSieveCount; i++) {
        if (!userSieves[i].active) { slot = i; break; }
    }
    if (slot < 0 && userSieveCount < MAX_USER_SIEVE) {
        slot = userSieveCount++;
    }
    if (slot < 0) return;

    userSieves[slot].basePos = cpv(x, y);

    cpBody *body = cpBodyNewKinematic();
    cpBodySetPosition(body, cpv(x, y));
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

    userSieves[slot].body = body;
    userSieves[slot].shape = shape;
    userSieves[slot].active = true;
}

// ユーザーふるいを描画
static void drawUserSieves(void) {
    for (int i = 0; i < userSieveCount; i++) {
        if (!userSieves[i].active) continue;

        cpVect pos = cpBodyGetPosition(userSieves[i].body);
        cpFloat angle = cpBodyGetAngle(userSieves[i].body);

        Rectangle rect = {(float)pos.x, (float)pos.y, SIEVE_LENGTH, SIEVE_THICKNESS};
        Vector2 origin = {SIEVE_LENGTH / 2.0f, SIEVE_THICKNESS / 2.0f};
        DrawRectanglePro(rect, origin, angle * 180.0f / PI, DARKGRAY);
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
    // ユーザー配置ふるいを振動
    for (int i = 0; i < userSieveCount; i++) {
        if (!userSieves[i].active) continue;
        cpBodySetVelocity(userSieves[i].body, velocity);
    }
}

static void drawSieve(void) {
    // 初期ふるいは削除されたので、ユーザー配置ふるいのみ描画
    // (drawUserSieves()で描画されるので、この関数は空でOK)
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
    DrawRectangle(0, SCREEN_HEIGHT - 20, FIELD_WIDTH, 20, DARKGRAY);
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
    float dragStartY = 0.0f;
    float cameraStartX = 0.0f;
    float cameraStartY = 0.0f;

    // 回転用
    int rotatingConveyor = -1;
    int rotatingSieve = -1;
    int clickedConveyor = -1;  // クリック判定用
    float clickStartTime = 0.0f;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // カメラ用のワールド座標計算（回転判定用）
        Camera2D camForRotate = {0};
        camForRotate.target = (Vector2){cameraX + SCREEN_WIDTH / 2.0f, cameraY + SCREEN_HEIGHT / 2.0f};
        camForRotate.offset = (Vector2){SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
        camForRotate.zoom = zoom;
        Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camForRotate);

        // マウスドラッグでスクロール または 回転/反転
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            rotatingConveyor = -1;
            rotatingSieve = -1;
            clickedConveyor = -1;
            clickStartTime = (float)GetTime();

            // モード4か6の場合、コンベアまたはふるいの中心ハンドルをクリックしたか確認
            if (actionMode == 4) {
                for (int i = 0; i < conveyorCount; i++) {
                    if (!conveyors[i].active) continue;
                    cpVect pos = cpBodyGetPosition(conveyors[i].body);
                    float dist = sqrtf(powf(mouseWorldPos.x - (float)pos.x, 2) + powf(mouseWorldPos.y - (float)pos.y, 2));
                    if (dist < CONVEYOR_HANDLE_RADIUS) {
                        clickedConveyor = i;
                        break;
                    }
                }
            } else if (actionMode == 6) {
                for (int i = 0; i < userSieveCount; i++) {
                    if (!userSieves[i].active) continue;
                    cpVect pos = cpBodyGetPosition(userSieves[i].body);
                    float dist = sqrtf(powf(mouseWorldPos.x - (float)pos.x, 2) + powf(mouseWorldPos.y - (float)pos.y, 2));
                    if (dist < SIEVE_LENGTH / 2.0f) {
                        rotatingSieve = i;
                        break;
                    }
                }
            }

            // 回転対象がなければドラッグモード
            if (clickedConveyor < 0 && rotatingSieve < 0) {
                dragging = true;
                dragStartX = GetMouseX();
                dragStartY = GetMouseY();
                cameraStartX = cameraX;
                cameraStartY = cameraY;
            }
        }
        // 長押しで回転開始（0.2秒後）
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && clickedConveyor >= 0) {
            float holdTime = (float)GetTime() - clickStartTime;
            if (holdTime > 0.2f) {
                rotatingConveyor = clickedConveyor;
            }
        }
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            // 短いクリック（0.2秒未満）でコンベアの方向を反転
            if (clickedConveyor >= 0 && rotatingConveyor < 0) {
                conveyors[clickedConveyor].reversed = !conveyors[clickedConveyor].reversed;
            }
            dragging = false;
            rotatingConveyor = -1;
            rotatingSieve = -1;
            clickedConveyor = -1;
        }
        if (dragging) {
            float dx = GetMouseX() - dragStartX;
            float dy = GetMouseY() - dragStartY;
            cameraX = cameraStartX - dx / zoom;
            cameraY = cameraStartY - dy / zoom;
        }
        // 回転処理
        if (rotatingConveyor >= 0 && conveyors[rotatingConveyor].active) {
            cpFloat currentAngle = cpBodyGetAngle(conveyors[rotatingConveyor].body);
            cpBodySetAngle(conveyors[rotatingConveyor].body, currentAngle + ROTATE_SPEED);
        }
        if (rotatingSieve >= 0 && userSieves[rotatingSieve].active) {
            cpFloat currentAngle = cpBodyGetAngle(userSieves[rotatingSieve].body);
            cpBodySetAngle(userSieves[rotatingSieve].body, currentAngle + ROTATE_SPEED);
        }

        // A/Dキーでスクロール
        if (IsKeyDown(KEY_A)) {
            cameraX -= 10.0f / zoom;
        }
        if (IsKeyDown(KEY_D)) {
            cameraX += 10.0f / zoom;
        }
        // W/Sキーで縦スクロール
        if (IsKeyDown(KEY_W)) {
            cameraY -= 10.0f / zoom;
        }
        if (IsKeyDown(KEY_S)) {
            cameraY += 10.0f / zoom;
        }

        // マウスホイールでズーム
        float wheel = GetMouseWheelMove();
        if (wheel != 0) {
            float zoomDelta = wheel * 0.1f;
            zoom += zoomDelta;
            if (zoom < 0.1f) zoom = 0.1f;
            if (zoom > 3.0f) zoom = 3.0f;
        }

        if (IsKeyPressed(KEY_SPACE)) {
            spawning = !spawning;
        }

        // 1-9キーで操作モード切替
        if (IsKeyPressed(KEY_ONE)) actionMode = 1;
        if (IsKeyPressed(KEY_TWO)) actionMode = 2;
        if (IsKeyPressed(KEY_THREE)) actionMode = 3;
        if (IsKeyPressed(KEY_FOUR)) actionMode = 4;
        if (IsKeyPressed(KEY_FIVE)) actionMode = 5;
        if (IsKeyPressed(KEY_SIX)) actionMode = 6;

        // 右クリック操作（カメラを考慮したワールド座標を取得）
        Camera2D cam = {0};
        cam.target = (Vector2){cameraX + SCREEN_WIDTH / 2.0f, cameraY + SCREEN_HEIGHT / 2.0f};
        cam.offset = (Vector2){SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
        cam.rotation = 0.0f;
        cam.zoom = zoom;
        Vector2 mouseWorld = GetScreenToWorld2D(GetMousePosition(), cam);
        float worldX = mouseWorld.x;
        float worldY = mouseWorld.y;

        if (actionMode == 1) {
            // モード1: 破壊
            if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                cpPointQueryInfo info;
                cpShape *shape = cpSpacePointQueryNearest(space, cpv(worldX, worldY), 0.0f, CP_SHAPE_FILTER_ALL, &info);
                if (shape && cpShapeGetCollisionType(shape) == COLLISION_TYPE_DEBRIS) {
                    int idx = (int)(intptr_t)cpShapeGetUserData(shape);
                    if (idx >= 0 && idx < debrisCount && debris[idx].active) {
                        breakDebris(idx);
                    }
                }
            }
        } else if (actionMode == 2) {
            // モード2: ドラッグ移動
            if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) && draggedDebrisIdx < 0) {
                cpPointQueryInfo info;
                cpShape *shape = cpSpacePointQueryNearest(space, cpv(worldX, worldY), 10.0f, CP_SHAPE_FILTER_ALL, &info);
                if (shape && cpShapeGetCollisionType(shape) == COLLISION_TYPE_DEBRIS) {
                    int idx = (int)(intptr_t)cpShapeGetUserData(shape);
                    if (idx >= 0 && idx < debrisCount && debris[idx].active) {
                        // スタティックならダイナミックに戻す
                        if (debris[idx].isStatic) {
                            debris[idx].wakeUp = true;
                            wakeUpStaticDebris();
                        }
                        if (debris[idx].body) {
                            draggedDebrisIdx = idx;
                            cpVect bodyPos = cpBodyGetPosition(debris[idx].body);
                            dragOffset = cpvsub(bodyPos, cpv(worldX, worldY));
                        }
                    }
                }
            }
            if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON) && draggedDebrisIdx >= 0) {
                if (debris[draggedDebrisIdx].active && debris[draggedDebrisIdx].body) {
                    cpBody *body = debris[draggedDebrisIdx].body;
                    if (cpBodyIsSleeping(body)) {
                        cpBodyActivate(body);
                    }
                    // 速度ベースでマウスに追従（物理的にめり込まない）
                    cpVect targetPos = cpvadd(cpv(worldX, worldY), dragOffset);
                    cpVect currentPos = cpBodyGetPosition(body);
                    cpVect diff = cpvsub(targetPos, currentPos);
                    float speed = 2000.0f;  // 追従速度
                    cpBodySetVelocity(body, cpvmult(diff, speed / 60.0f));
                    // 重力を打ち消す
                    cpBodySetForce(body, cpvmult(cpSpaceGetGravity(space), -cpBodyGetMass(body)));
                }
            }
            if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
                draggedDebrisIdx = -1;
            }
        } else if (actionMode == 3) {
            // モード3: 水配置
            if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                placeWater(worldX, worldY);
            }
        } else if (actionMode == 4) {
            // モード4: コンベア配置
            if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                placeConveyor(worldX, worldY);
            }
        } else if (actionMode == 5) {
            // モード5: Output配置
            if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                placeOutput(worldX, worldY);
            }
        } else if (actionMode == 6) {
            // モード6: ふるい配置
            if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                placeUserSieve(worldX, worldY);
            }
        }

        if (spawning) {
            spawnTimer += dt;
            if (spawnTimer > 0.128f) {
                // 3個生成: 60%石、25%木、15%粘土
                for (int i = 0; i < 3; i++) {
                    float r = rand01();
                    if (r < 0.15f) {
                        spawnClay();
                    } else if (r < 0.40f) {
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
        processPendingSticky();
        applyBuoyancy();
        applyConveyorForce();
        checkAndClearOutputs();

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Camera2Dを設定（ズームとスクロール）
        Camera2D camera = {0};
        camera.target = (Vector2){cameraX + SCREEN_WIDTH / 2.0f, cameraY + SCREEN_HEIGHT / 2.0f};
        camera.offset = (Vector2){SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
        camera.rotation = 0.0f;
        camera.zoom = zoom;

        BeginMode2D(camera);

        drawWater();
        drawGround();
        drawSieve();
        drawUserSieves();

        for (int i = 0; i < debrisCount; i++) {
            drawDebris(i);
        }
        drawStickyJoints();
        drawConveyors();
        drawOutputs();

        EndMode2D();

        int activeCount = 0;
        for (int i = 0; i < debrisCount; i++) {
            if (debris[i].active) activeCount++;
        }

        DrawRectangle(5, 5, 220, 135, (Color){255, 255, 255, 200});
        DrawFPS(10, 10);
        DrawText(TextFormat("Debris: %d", activeCount), 10, 35, 20, BLACK);
        DrawText(TextFormat("Physics: %.2f ms", physicsTimeMs), 10, 55, 20, BLACK);
        DrawText(TextFormat("Mouse: %.0f, %.0f", worldX, worldY), 10, 75, 20, BLACK);
        DrawText(TextFormat("Mode[1-9]: %d %s", actionMode, actionModeNames[actionMode]), 10, 95, 20, DARKBLUE);
        DrawText(TextFormat("Zoom: %.1fx", zoom), 10, 115, 20, BLACK);

        // キー操作一覧（右端）
        int helpX = SCREEN_WIDTH - 180;
        int helpY = 5;
        DrawRectangle(helpX - 5, helpY, 180, 260, (Color){255, 255, 255, 200});
        DrawText("-- Controls --", helpX, helpY + 5, 16, DARKGRAY);
        DrawText("Space: Spawn ON/OFF", helpX, helpY + 25, 14, BLACK);
        DrawText("1: Break mode", helpX, helpY + 45, 14, BLACK);
        DrawText("2: Drag mode", helpX, helpY + 65, 14, BLACK);
        DrawText("3: Water mode", helpX, helpY + 85, 14, BLACK);
        DrawText("4: Conveyor mode", helpX, helpY + 105, 14, BLACK);
        DrawText("5: Output mode", helpX, helpY + 125, 14, BLACK);
        DrawText("6: Sieve mode", helpX, helpY + 145, 14, BLACK);
        DrawText("A/D: Scroll L/R", helpX, helpY + 170, 14, BLACK);
        DrawText("W/S: Scroll U/D", helpX, helpY + 190, 14, BLACK);
        DrawText("Wheel: Zoom", helpX, helpY + 210, 14, BLACK);
        DrawText("L-Drag: Pan", helpX, helpY + 230, 14, BLACK);

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
    for (int i = 0; i < userSieveCount; i++) {
        if (!userSieves[i].active) continue;
        cpSpaceRemoveShape(space, userSieves[i].shape);
        cpSpaceRemoveBody(space, userSieves[i].body);
        cpShapeFree(userSieves[i].shape);
        cpBodyFree(userSieves[i].body);
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
