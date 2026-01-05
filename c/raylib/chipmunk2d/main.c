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

// 衝突カテゴリ（表面と裏面で完全分離）
#define CATEGORY_DEBRIS_FRONT 1
#define CATEGORY_DEBRIS_BACK 2
#define CATEGORY_WATER_FRONT 4
#define CATEGORY_WATER_BACK 8
#define CATEGORY_STRUCTURE_FRONT 16
#define CATEGORY_STRUCTURE_BACK 32
#define CATEGORY_GROUND 64  // 地面と左右壁（両面共通）

// 便利マクロ
#define MASK_FRONT (CATEGORY_DEBRIS_FRONT | CATEGORY_WATER_FRONT | CATEGORY_STRUCTURE_FRONT | CATEGORY_GROUND)
#define MASK_BACK (CATEGORY_DEBRIS_BACK | CATEGORY_WATER_BACK | CATEGORY_STRUCTURE_BACK | CATEGORY_GROUND)

#define WAKE_UP_IMPULSE_THRESHOLD 150000.0f  // この衝撃を超えたら起こす（大きい=起きにくい）
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
    bool isBack;        // 裏面にいるか
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

// 操作モード (1: 破壊, 2: ドラッグ移動, 3: 水配置, 4: コンベア, 5: Output, 6: ふるい, 7: Input, 8: 削除, 9: 壁)
static int actionMode = 1;
static const char* actionModeNames[] = {"", "Break", "Drag", "Water", "Conveyor", "Output", "Sieve", "Input", "Delete", "Wall", "Transfer"};
static const int modeCount = sizeof(actionModeNames) / sizeof(actionModeNames[0]);
static bool showModeMenu = false;  // Eキーで表示するメニュー

// ドラッグ移動用
static int draggedDebrisIdx = -1;
static cpVect dragOffset = {0, 0};

// 水パーティクル
#define MAX_WATER_PARTICLES 2000
#define WATER_PARTICLE_RADIUS 5.0f
#define COLLISION_TYPE_WATER 2
#define WATER_MERGE_DISTANCE 8.0f   // この距離以内でマージ
#define WATER_SPLIT_DISTANCE 60.0f  // この距離以上離れたら分割
typedef struct {
    cpBody *body;
    cpShape *shape;
    bool active;
    bool isMud;  // 泥水かどうか
    int valence; // 1, 2, 4, 8, 16（含む水量）
    int mergeCooldown;  // マージ禁止カウンタ（0なら合体可能）
    bool isBack;  // 裏面にいるか
} WaterParticle;
static WaterParticle waterParticles[MAX_WATER_PARTICLES];
static int waterParticleCount = 0;

// 前方宣言
static float getWaterRadius(int valence);

// 配置用グリッド
#define GRID_UNIT 10.0f
static float snapToGrid(float v) {
    return roundf(v / GRID_UNIT) * GRID_UNIT;
}

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
    bool isBack;    // 裏面にあるか
} Conveyor;
static Conveyor conveyors[MAX_CONVEYOR];
static int conveyorCount = 0;
static float lastConveyorAngle = CONVEYOR_DEFAULT_ANGLE;  // 最後に置いたコンベアの角度

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
    bool isBack;  // 裏面にあるか
} Output;
static Output outputs[MAX_OUTPUT];
static int outputCount = 0;

// Input（投入口）
#define MAX_INPUT_BOX 20
#define INPUT_SIZE 200.0f
#define INPUT_WALL_THICKNESS 10.0f
typedef struct {
    float x, y;
    cpShape *leftWall;
    cpShape *rightWall;
    cpShape *topWall;
    bool active;
    bool isBack;  // 裏面にあるか
} Input;
static Input inputs[MAX_INPUT_BOX];
static int inputCount = 0;

// ユーザー配置ふるい
#define MAX_USER_SIEVE 50
#define SIEVE_HANDLE_RADIUS 5.0f  // ふるいのハンドル半径（コンベアの1/3）
typedef struct {
    cpBody *body;
    cpShape *shape;
    cpVect basePos;
    bool active;
    bool isBack;  // 裏面にあるか
} UserSieve;
static UserSieve userSieves[MAX_USER_SIEVE];
static int userSieveCount = 0;
static float lastSieveAngle = SIEVE_ANGLE;  // 最後に置いたふるいの角度

// 壁（静的な障害物）
#define MAX_WALL 100
#define WALL_LENGTH 60.0f
#define WALL_THICKNESS 5.0f
#define WALL_HANDLE_RADIUS 8.0f
typedef struct {
    cpBody *body;
    cpShape *shape;
    bool active;
    bool isBack;  // 裏面にあるか
} Wall;
static Wall walls[MAX_WALL];
static int wallCount = 0;
static float lastWallAngle = 0.0f;  // 最後に置いた壁の角度

// Transfer（立体交差パーツ）
#define MAX_TRANSFER 50
#define TRANSFER_SIZE 80.0f  // 正方形のサイズ
typedef struct {
    float x, y;      // 中心座標
    bool active;
    bool toBack;     // true: 表→裏, false: 裏→表
} Transfer;
static Transfer transfers[MAX_TRANSFER];
static int transferCount = 0;

// 表面/裏面の表示切替
static bool showBackSide = false;  // Fキーで切替

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
    float elasticity = 0.1f;  // 石は跳ねない
    cpShapeSetElasticity(shape, elasticity);
    cpShapeSetCollisionType(shape, COLLISION_TYPE_DEBRIS);
    cpShapeSetFilter(shape, cpShapeFilterNew(0, CATEGORY_DEBRIS_FRONT, MASK_FRONT));
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
    debris[slot].isBack = false;  // 常に表面に生成
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
    cpShapeSetFilter(shape, cpShapeFilterNew(0, CATEGORY_DEBRIS_FRONT, MASK_FRONT));
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
    debris[slot].isBack = false;  // 常に表面に生成
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
    cpShapeSetFilter(shape, cpShapeFilterNew(0, CATEGORY_DEBRIS_FRONT, MASK_FRONT));
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
    debris[slot].isBack = false;  // 常に表面に生成
    if (slot >= debrisCount) debrisCount = slot + 1;
}

// 指定位置に指定サイズ・密度・タイプの破片を生成（isBack指定可能）
static void spawnDebrisAt(float x, float y, float size, float density, DebrisType type, bool isBack) {
    int slot = findFreeSlot();
    if (slot < 0) return;
    if (size < 2.0f) size = 2.0f;  // 最小サイズ保証

    int vertexCount;
    cpVect points[MAX_VERTICES];
    float area;

    if (type == DEBRIS_WOOD) {
        // 木は長方形（縦横比 3:1 〜 6:1）
        float aspectRatio = 3.0f + rand01() * 3.0f;
        // size は「半径」的な値なので、面積 = 2 * size^2 程度と想定
        // 長方形の面積 = length * width = length * (length / aspectRatio)
        // 面積を保持するため: length = sqrt(targetArea * aspectRatio)
        float targetArea = 2.0f * size * size;
        float length = sqrtf(targetArea * aspectRatio);
        float width = length / aspectRatio;
        if (width < 1.5f) {
            width = 1.5f;
            length = targetArea / width;
        }
        float hw = length / 2.0f;
        float hh = width / 2.0f;
        points[0] = cpv(-hw, -hh);
        points[1] = cpv(-hw, hh);
        points[2] = cpv(hw, hh);
        points[3] = cpv(hw, -hh);
        vertexCount = 4;
        area = length * width;
    } else {
        // 石・粘土はランダム多角形
        vertexCount = 3 + rand() % 5;
        float angles[MAX_VERTICES];
        for (int i = 0; i < vertexCount; i++) {
            angles[i] = ((float)i / vertexCount + rand01() * 0.1f) * 2.0f * PI;
        }
        qsort(angles, vertexCount, sizeof(float), compareFloats);
        for (int i = 0; i < vertexCount; i++) {
            float r = size * (0.7f + rand01() * 0.3f);
            points[i] = cpv(r * cosf(angles[i]), r * sinf(angles[i]));
        }
        area = calcPolygonArea(points, vertexCount);
    }

    // 質量を計算
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
    cpShapeSetFriction(shape, type == DEBRIS_WOOD ? 0.6f : 0.5f);
    float elasticity = type == DEBRIS_WOOD ? 0.2f : 0.1f;  // 固い物は跳ねない
    cpShapeSetElasticity(shape, elasticity);
    cpShapeSetCollisionType(shape, COLLISION_TYPE_DEBRIS);
    if (isBack) {
        cpShapeSetFilter(shape, cpShapeFilterNew(0, CATEGORY_DEBRIS_BACK, MASK_BACK));
    } else {
        cpShapeSetFilter(shape, cpShapeFilterNew(0, CATEGORY_DEBRIS_FRONT, MASK_FRONT));
    }
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
    debris[slot].type = type;
    debris[slot].active = true;
    debris[slot].isStatic = false;
    debris[slot].wakeUp = false;
    debris[slot].shouldBreak = false;
    debris[slot].breakFlash = 0;  // 破片は元の色で表示
    debris[slot].idleTime = 0.0f;
    debris[slot].lastPos = cpv(x, y);
    debris[slot].isBack = isBack;  // 面を継承
    if (slot >= debrisCount) debrisCount = slot + 1;
}

// 石・木を砕く（面積を保存）
static void breakDebris(int index) {
    if (!debris[index].active) return;
    if (debris[index].type == DEBRIS_CLAY) return;  // 粘土は割れない
    if (debris[index].size < MIN_BREAK_SIZE) return;

    float originalSize = debris[index].size;
    float density = debris[index].density;  // 削除前に保存
    DebrisType type = debris[index].type;   // 削除前に保存
    bool isBack = debris[index].isBack;     // 削除前に保存
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

    // 破片を生成（元の密度とタイプと面を継承）
    for (int i = 0; i < numPieces; i++) {
        float offsetX = (rand01() - 0.5f) * originalSize;
        float offsetY = (rand01() - 0.5f) * originalSize;
        spawnDebrisAt((float)pos.x + offsetX, (float)pos.y + offsetY, sizes[i], density, type, isBack);
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

// 画面外の水パーティクルを削除
static void cleanupWaterParticles(void) {
    for (int i = 0; i < waterParticleCount; i++) {
        if (!waterParticles[i].active) continue;
        cpVect pos = cpBodyGetPosition(waterParticles[i].body);
        if (pos.x > FIELD_WIDTH + 100 || pos.x < -100.0f || pos.y > SCREEN_HEIGHT + 100) {
            cpSpaceRemoveShape(space, waterParticles[i].shape);
            cpSpaceRemoveBody(space, waterParticles[i].body);
            cpShapeFree(waterParticles[i].shape);
            cpBodyFree(waterParticles[i].body);
            waterParticles[i].active = false;
        }
    }
}

// 位置が水中かチェック（水パーティクルとの距離で判定）
static bool isInWater(cpVect pos) {
    for (int w = 0; w < waterParticleCount; w++) {
        if (!waterParticles[w].active) continue;
        cpVect waterPos = cpBodyGetPosition(waterParticles[w].body);
        float dx = pos.x - waterPos.x;
        float dy = pos.y - waterPos.y;
        float dist = sqrtf(dx * dx + dy * dy);
        // 水パーティクルの半径 + デブリが接触する距離
        if (dist < WATER_PARTICLE_RADIUS * 2.0f) {
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

        // 回転中はスタティック化しない
        cpFloat angVel = cpBodyGetAngularVelocity(debris[i].body);
        if (fabs(angVel) > 0.2f) {
            debris[i].idleTime = 0.0f;
            debris[i].lastPos = pos;
            continue;
        }

        // 速度が十分低いかチェック
        cpVect vel = cpBodyGetVelocity(debris[i].body);
        if (cpvlength(vel) > 5.0f) {
            debris[i].idleTime = 0.0f;
            debris[i].lastPos = pos;
            continue;
        }

        cpVect diff = cpvsub(pos, debris[i].lastPos);
        float moved = cpvlength(diff);

        // ほとんど動いていない場合
        if (moved < 1.5f) {
            debris[i].idleTime += dt;
        } else {
            debris[i].idleTime = 0.0f;
        }
        debris[i].lastPos = pos;

        // 0.5秒以上静止していたらスタティック化
        if (debris[i].idleTime > 0.5f) {
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
            if (debris[i].isBack) {
                cpShapeSetFilter(debris[i].shape, cpShapeFilterNew(0, CATEGORY_DEBRIS_BACK, MASK_BACK));
            } else {
                cpShapeSetFilter(debris[i].shape, cpShapeFilterNew(0, CATEGORY_DEBRIS_FRONT, MASK_FRONT));
            }
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

        // 衝撃で割れる判定（サイズ3より大きい石と木）
        float size = debris[idx].size;
        if (size > 3.0f && !debris[idx].shouldBreak &&
            (debris[idx].type == DEBRIS_STONE || debris[idx].type == DEBRIS_WOOD)) {
            // 質量を考慮した閾値（木は石の1/4の力で砕ける）
            float breakThreshold = size * size * size * 500.0f;
            if (debris[idx].type == DEBRIS_WOOD) {
                breakThreshold *= 0.25f;  // 木は弱い力で砕ける
            }
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

    // 水パーティクルが壁（構造物）に触れたらマージ禁止
    for (int s = 0; s < 2; s++) {
        cpShape *waterShape = shapes[s];
        cpShape *otherShape = shapes[1 - s];
        if (cpShapeGetCollisionType(waterShape) != COLLISION_TYPE_WATER) continue;
        // 相手が水以外（壁や構造物）ならクールダウン設定
        if (cpShapeGetCollisionType(otherShape) == COLLISION_TYPE_WATER) continue;

        int idx = (int)(intptr_t)cpShapeGetUserData(waterShape);
        if (idx >= 0 && idx < waterParticleCount && waterParticles[idx].active) {
            waterParticles[idx].mergeCooldown = 5;  // 壁接触中はマージ禁止
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

        // ピボットジョイントを作成（完全固定、力で切れる）
        cpVect point = pendingSticky[i].point;
        cpConstraint *joint = cpPivotJointNew(debris[idxA].body, debris[idxB].body, point);
        cpConstraintSetMaxForce(joint, INFINITY);  // 完全固定（伸びない）
        cpConstraintSetErrorBias(joint, cpfpow(0.3f, 60.0f));  // 振動を抑える
        cpConstraintSetCollideBodies(joint, cpTrue);
        cpSpaceAddConstraint(space, joint);

        stickyJoints[slot].joint = joint;
        stickyJoints[slot].debrisA = idxA;
        stickyJoints[slot].debrisB = idxB;
        stickyJoints[slot].active = true;
    }
    pendingStickyCount = 0;

    // 力が閾値を超えたジョイントを切る
    cpFloat maxImpulse = 50000.0f;  // この力を超えたら切れる
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
        // ジョイントにかかった力をチェック
        cpFloat impulse = cpConstraintGetImpulse(stickyJoints[i].joint);
        if (fabs(impulse) > maxImpulse) {
            cpSpaceRemoveConstraint(space, stickyJoints[i].joint);
            cpConstraintFree(stickyJoints[i].joint);
            stickyJoints[i].active = false;
        }
    }
}

// スタティック化した石をダイナミックに戻す
// 指定位置の近くにあるstaticデブリを起こす
static void wakeUpStaticDebrisNearPoint(float x, float y, float radius) {
    for (int i = 0; i < debrisCount; i++) {
        if (!debris[i].active || !debris[i].isStatic) continue;
        float dx = debris[i].staticPos.x - x;
        float dy = debris[i].staticPos.y - y;
        float dist = sqrtf(dx * dx + dy * dy);
        if (dist < radius + debris[i].size) {
            debris[i].wakeUp = true;
        }
    }
}

static void wakeUpStaticDebris(void) {
    bool anyWokeUp = true;
    // 連鎖的に起こす（起きたデブリの近くのstaticデブリも起こす）
    while (anyWokeUp) {
        anyWokeUp = false;
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
            float elasticity = debris[i].type == DEBRIS_WOOD ? 0.2f : 0.1f;  // 固い物は跳ねない
            cpShapeSetElasticity(shape, elasticity);
            cpShapeSetCollisionType(shape, COLLISION_TYPE_DEBRIS);
            if (debris[i].isBack) {
                cpShapeSetFilter(shape, cpShapeFilterNew(0, CATEGORY_DEBRIS_BACK, MASK_BACK));
            } else {
                cpShapeSetFilter(shape, cpShapeFilterNew(0, CATEGORY_DEBRIS_FRONT, MASK_FRONT));
            }
            cpShapeSetUserData(shape, (void*)(intptr_t)i);
            cpSpaceAddShape(space, shape);

            debris[i].body = body;
            debris[i].shape = shape;
            debris[i].isStatic = false;
            debris[i].wakeUp = false;
            debris[i].idleTime = 0.0f;
            debris[i].lastPos = debris[i].staticPos;

            // このデブリの近くのstaticデブリを起こす（連鎖）
            wakeUpStaticDebrisNearPoint(debris[i].staticPos.x, debris[i].staticPos.y, debris[i].size + 10.0f);
            anyWokeUp = true;
        }
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
    // 現在表示中の面のみ描画
    if (debris[index].isBack != showBackSide) return;

    // カリング: カメラ範囲外ならスキップ
    cpVect pos;
    if (debris[index].isStatic) {
        pos = debris[index].staticPos;
    } else {
        pos = cpBodyGetPosition(debris[index].body);
    }
    float margin = debris[index].size + 20.0f;  // 余裕を持たせる
    float viewLeft = cameraX + (SCREEN_WIDTH / 2.0f) - (SCREEN_WIDTH / 2.0f) / zoom;
    float viewRight = cameraX + (SCREEN_WIDTH / 2.0f) + (SCREEN_WIDTH / 2.0f) / zoom;
    float viewTop = cameraY + (SCREEN_HEIGHT / 2.0f) - (SCREEN_HEIGHT / 2.0f) / zoom;
    float viewBottom = cameraY + (SCREEN_HEIGHT / 2.0f) + (SCREEN_HEIGHT / 2.0f) / zoom;
    if (pos.x < viewLeft - margin || pos.x > viewRight + margin ||
        pos.y < viewTop - margin || pos.y > viewBottom + margin) {
        return;
    }

    Color color;
    if (debris[index].isStatic) {
        color = (Color){80, 80, 180, 255};  // スタティックは青
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
        // 現在表示中の面のみ描画
        if (debris[idxA].isBack != showBackSide) continue;

        cpVect posA = cpBodyGetPosition(debris[idxA].body);
        cpVect posB = cpBodyGetPosition(debris[idxB].body);
        Vector2 screenA = {(float)posA.x, (float)posA.y};
        Vector2 screenB = {(float)posB.x, (float)posB.y};
        DrawLineEx(screenA, screenB, 2.0f, (Color){255, 100, 100, 200});
    }
}

// 水パーティクルを描画
static void drawWaterParticles(void) {
    for (int i = 0; i < waterParticleCount; i++) {
        if (!waterParticles[i].active) continue;
        // 現在表示中の面のみ描画
        if (waterParticles[i].isBack != showBackSide) continue;
        cpVect pos = cpBodyGetPosition(waterParticles[i].body);
        float radius = getWaterRadius(waterParticles[i].valence);
        Color color;
        if (waterParticles[i].isMud) {
            color = (Color){180, 140, 100, 200};  // 泥水：明るい茶色
        } else {
            color = (Color){100, 180, 255, 200};  // 水：水色
        }
        DrawCircle((int)pos.x, (int)pos.y, radius, color);
    }
}

// 水パーティクルを振動させて平らにする
static void updateWaterParticles(void) {
    for (int i = 0; i < waterParticleCount; i++) {
        if (!waterParticles[i].active) continue;

        cpVect pos = cpBodyGetPosition(waterParticles[i].body);
        cpVect vel = cpBodyGetVelocity(waterParticles[i].body);

        // 横方向に広がる力（左右ランダム）
        float spreadX = (rand01() - 0.5f) * 300.0f;
        cpBodyApplyForceAtWorldPoint(waterParticles[i].body, cpv(spreadX, 0), pos);

        // 速度が遅い場合は横にランダムに動く
        if (fabs(vel.x) < 10.0f) {
            float kick = (rand01() > 0.5f ? 1.0f : -1.0f) * 20.0f;
            cpBodySetVelocity(waterParticles[i].body, cpv(vel.x + kick, vel.y));
        }
    }
}

// 水パーティクルとの相互作用（浮力・粘土→石変換）
static void applyBuoyancy(void) {
    for (int i = 0; i < debrisCount; i++) {
        if (!debris[i].active || debris[i].isStatic || !debris[i].body) continue;

        cpVect pos = cpBodyGetPosition(debris[i].body);
        float debrisRadius = debris[i].size * 0.5f;  // 半径（サイズの半分）

        // 全ての水パーティクルをチェック
        bool inWater = false;
        for (int w = 0; w < waterParticleCount; w++) {
            if (!waterParticles[w].active) continue;

            cpVect waterPos = cpBodyGetPosition(waterParticles[w].body);
            float dx = pos.x - waterPos.x;
            float dy = pos.y - waterPos.y;
            float dist = sqrtf(dx * dx + dy * dy);

            // デブリと水パーティクルが実際に重なっているか（厳密に判定）
            if (dist < WATER_PARTICLE_RADIUS + debrisRadius * 0.5f) {
                inWater = true;

                // 粘土は水に触れると石に変化し、水は泥水になる
                if (debris[i].type == DEBRIS_CLAY) {
                    debris[i].type = DEBRIS_STONE;
                    removeJointsForDebris(i);
                    waterParticles[w].isMud = true;
                }
            }
        }

        // 水中にいる場合、浮力と抵抗を適用
        if (inWater) {
            // 密度1未満なら浮力を適用
            if (debris[i].density < 1.0f) {
                float area = calcPolygonArea(debris[i].vertices, debris[i].vertexCount);
                // 浮力 = (1 - 密度) * 面積 * 重力（上向き）
                float buoyancy = (1.0f - debris[i].density) * area * 9.8f * SCALE * 5.0f;
                cpVect currentForce = cpBodyGetForce(debris[i].body);
                cpBodySetForce(debris[i].body, cpv(currentForce.x, currentForce.y - buoyancy));
            }
            // 水の抵抗（強め）
            cpVect vel = cpBodyGetVelocity(debris[i].body);
            cpBodySetVelocity(debris[i].body, cpvmult(vel, 0.85f));
            // 回転も減衰
            cpBodySetAngularVelocity(debris[i].body, cpBodyGetAngularVelocity(debris[i].body) * 0.85f);
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

    // 水パーティクルにもコンベアの力を適用
    for (int i = 0; i < waterParticleCount; i++) {
        if (!waterParticles[i].active) continue;

        for (int c = 0; c < conveyorCount; c++) {
            if (!conveyors[c].active) continue;

            cpContactPointSet contacts = cpShapesCollide(waterParticles[i].shape, conveyors[c].shape);
            if (contacts.count > 0) {
                float angle = (float)cpBodyGetAngle(conveyors[c].body);
                cpVect conveyorDir = cpv(cosf(angle), sinf(angle));
                if (conveyors[c].reversed) {
                    conveyorDir = cpvneg(conveyorDir);
                }

                cpVect vel = cpBodyGetVelocity(waterParticles[i].body);
                float speedInDir = (float)cpvdot(vel, conveyorDir);

                // 目標速度との差に応じて力を調整（水は軽いので弱めに）
                if (speedInDir < CONVEYOR_MAX_SPEED * 0.5f) {
                    float forceMag = (CONVEYOR_MAX_SPEED * 0.5f - speedInDir) * 50.0f;
                    if (forceMag > 5000.0f) forceMag = 5000.0f;
                    cpVect force = cpvmult(conveyorDir, forceMag);
                    cpVect pos = cpBodyGetPosition(waterParticles[i].body);
                    cpBodyApplyForceAtWorldPoint(waterParticles[i].body, force, pos);
                }
                break;
            }
        }
    }
}

// valenceに応じた半径を計算（充填効率を考慮）
static float getWaterRadius(int valence) {
    return WATER_PARTICLE_RADIUS * powf((float)valence, 0.57f);
}

// 水パーティクルのシェイプを更新（valence変更後に呼ぶ）
static void updateWaterParticleShape(int idx) {
    WaterParticle *wp = &waterParticles[idx];
    float newRadius = getWaterRadius(wp->valence);
    cpFloat newMass = (cpFloat)wp->valence;
    cpFloat newMoment = cpMomentForCircle(newMass, 0, newRadius, cpvzero);

    cpBodySetMass(wp->body, newMass);
    cpBodySetMoment(wp->body, newMoment);

    // シェイプを再作成
    cpSpaceRemoveShape(space, wp->shape);
    cpShapeFree(wp->shape);

    wp->shape = cpCircleShapeNew(wp->body, newRadius, cpvzero);
    cpShapeSetFriction(wp->shape, 0.0f);
    cpShapeSetElasticity(wp->shape, 0.1f);
    cpShapeSetCollisionType(wp->shape, COLLISION_TYPE_WATER);
    cpShapeSetFilter(wp->shape, cpShapeFilterNew(0, CATEGORY_WATER_FRONT, MASK_FRONT));
    cpShapeSetUserData(wp->shape, (void*)(intptr_t)idx);
    cpSpaceAddShape(space, wp->shape);
}

// 水パーティクルのマージ処理（同じvalence同士のみ）
static void mergeWaterParticles(void) {
    for (int i = 0; i < waterParticleCount; i++) {
        if (!waterParticles[i].active) continue;
        if (waterParticles[i].valence >= 16) continue;  // 最大valence
        // クールダウン中はマージ禁止（減算も行う）
        if (waterParticles[i].mergeCooldown > 0) {
            waterParticles[i].mergeCooldown--;
            continue;
        }

        cpVect posI = cpBodyGetPosition(waterParticles[i].body);
        float radiusI = getWaterRadius(waterParticles[i].valence);

        for (int j = i + 1; j < waterParticleCount; j++) {
            if (!waterParticles[j].active) continue;
            if (waterParticles[j].mergeCooldown > 0) continue;  // jもクールダウン中はスキップ
            if (waterParticles[i].isMud != waterParticles[j].isMud) continue;
            // 同じvalence同士のみマージ
            if (waterParticles[j].valence != waterParticles[i].valence) continue;

            cpVect posJ = cpBodyGetPosition(waterParticles[j].body);
            float radiusJ = getWaterRadius(waterParticles[j].valence);
            float dist = cpvlength(cpvsub(posI, posJ));
            float mergeThresh = WATER_MERGE_DISTANCE + radiusI + radiusJ;

            if (dist < mergeThresh) {
                // jをiに吸収
                waterParticles[i].valence *= 2;

                // jを削除
                cpSpaceRemoveShape(space, waterParticles[j].shape);
                cpSpaceRemoveBody(space, waterParticles[j].body);
                cpShapeFree(waterParticles[j].shape);
                cpBodyFree(waterParticles[j].body);
                waterParticles[j].active = false;

                // iのシェイプを更新
                updateWaterParticleShape(i);
                break;
            }
        }
    }
}

// 孤立した水パーティクルを分割
static void splitWaterParticles(void) {
    for (int i = 0; i < waterParticleCount; i++) {
        if (!waterParticles[i].active) continue;
        if (waterParticles[i].valence <= 1) continue;  // 最小valence

        cpVect posI = cpBodyGetPosition(waterParticles[i].body);
        float radiusI = getWaterRadius(waterParticles[i].valence);

        // 近くに他の水パーティクルがいるかチェック
        bool hasNeighbor = false;
        for (int j = 0; j < waterParticleCount; j++) {
            if (i == j || !waterParticles[j].active) continue;

            cpVect posJ = cpBodyGetPosition(waterParticles[j].body);
            float dist = cpvlength(cpvsub(posI, posJ));

            if (dist < WATER_SPLIT_DISTANCE + radiusI) {
                hasNeighbor = true;
                break;
            }
        }

        // 孤立時、または壁接触中は強制分裂（隙間を通れるように）
        bool touchingWall = (waterParticles[i].mergeCooldown > 0);
        bool forceSplit = touchingWall && (rand() % 100 < 20);  // 壁接触中は20%で強制分裂
        if (!hasNeighbor || forceSplit) {
            // 分割：valenceを半分にして、新しい粒子を生成
            int newValence = waterParticles[i].valence / 2;
            waterParticles[i].valence = newValence;
            updateWaterParticleShape(i);
            // 強制分裂時はクールダウン設定（約3秒間マージ禁止）
            if (forceSplit) {
                waterParticles[i].mergeCooldown = 30;
            }

            // 新しい粒子を少しずらして生成
            cpVect vel = cpBodyGetVelocity(waterParticles[i].body);
            float offsetX = (rand() % 20 - 10);
            float offsetY = (rand() % 20 - 10);

            // 空きスロットを探す
            int slot = -1;
            for (int k = 0; k < waterParticleCount; k++) {
                if (!waterParticles[k].active) { slot = k; break; }
            }
            if (slot < 0 && waterParticleCount < MAX_WATER_PARTICLES) {
                slot = waterParticleCount++;
            }
            if (slot >= 0) {
                float radius = getWaterRadius(newValence);
                cpFloat mass = (cpFloat)newValence;
                cpFloat moment = cpMomentForCircle(mass, 0, radius, cpvzero);
                cpBody *body = cpBodyNew(mass, moment);
                cpBodySetPosition(body, cpv(posI.x + offsetX, posI.y + offsetY));
                cpBodySetVelocity(body, vel);
                cpSpaceAddBody(space, body);

                cpShape *shape = cpCircleShapeNew(body, radius, cpvzero);
                cpShapeSetFriction(shape, 0.0f);
                cpShapeSetElasticity(shape, 0.1f);
                cpShapeSetCollisionType(shape, COLLISION_TYPE_WATER);
                cpShapeSetFilter(shape, cpShapeFilterNew(0, CATEGORY_WATER_FRONT, MASK_FRONT));
                cpShapeSetUserData(shape, (void*)(intptr_t)slot);
                cpSpaceAddShape(space, shape);

                waterParticles[slot].body = body;
                waterParticles[slot].shape = shape;
                waterParticles[slot].active = true;
                waterParticles[slot].isMud = waterParticles[i].isMud;
                waterParticles[slot].valence = newValence;
                waterParticles[slot].mergeCooldown = forceSplit ? 30 : 0;
            }
        }
    }
}

// 水パーティクルを生成
static void spawnWaterParticle(float x, float y) {
    // 空きスロットを探す
    int slot = -1;
    for (int i = 0; i < waterParticleCount; i++) {
        if (!waterParticles[i].active) { slot = i; break; }
    }
    if (slot < 0 && waterParticleCount < MAX_WATER_PARTICLES) {
        slot = waterParticleCount++;
    }
    if (slot < 0) return;

    // ダイナミックボディを作成
    cpFloat mass = 1.0f;
    cpFloat moment = cpMomentForCircle(mass, 0, WATER_PARTICLE_RADIUS, cpvzero);
    cpBody *body = cpBodyNew(mass, moment);
    cpBodySetPosition(body, cpv(x, y));
    cpSpaceAddBody(space, body);

    // 円形シェイプ
    cpShape *shape = cpCircleShapeNew(body, WATER_PARTICLE_RADIUS, cpvzero);
    cpShapeSetFriction(shape, 0.0f);  // 摩擦ゼロで滑り落ちる
    cpShapeSetElasticity(shape, 0.1f);  // 少し弾んで広がる
    cpShapeSetCollisionType(shape, COLLISION_TYPE_WATER);
    cpShapeSetFilter(shape, cpShapeFilterNew(0, CATEGORY_WATER_FRONT, MASK_FRONT));
    cpShapeSetUserData(shape, (void*)(intptr_t)slot);  // インデックスを保存
    cpSpaceAddShape(space, shape);

    waterParticles[slot].body = body;
    waterParticles[slot].shape = shape;
    waterParticles[slot].active = true;
    waterParticles[slot].isMud = false;
    waterParticles[slot].valence = 1;
    waterParticles[slot].mergeCooldown = 0;
}

// コンベアを配置
static void placeConveyor(float x, float y) {
    x = snapToGrid(x);
    y = snapToGrid(y);

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
    cpBodySetAngle(body, lastConveyorAngle);
    cpSpaceAddBody(space, body);

    // コンベアの形状
    cpFloat hw = CONVEYOR_LENGTH / 2.0f;
    cpFloat hh = CONVEYOR_THICKNESS / 2.0f;
    cpVect verts[4] = {
        cpv(-hw, -hh), cpv(-hw, hh), cpv(hw, hh), cpv(hw, -hh)
    };

    cpShape *shape = cpPolyShapeNew(body, 4, verts, cpTransformIdentity, 0.0f);
    cpShapeSetFriction(shape, 1.0f);  // 高摩擦
    if (showBackSide) {
        cpShapeSetFilter(shape, cpShapeFilterNew(0, CATEGORY_STRUCTURE_BACK, MASK_BACK));
    } else {
        cpShapeSetFilter(shape, cpShapeFilterNew(0, CATEGORY_STRUCTURE_FRONT, MASK_FRONT));
    }
    // 表面速度は使わず、力だけで動かす（回転に正しく追従させるため）

    cpSpaceAddShape(space, shape);

    conveyors[slot].body = body;
    conveyors[slot].shape = shape;
    conveyors[slot].x = x;
    conveyors[slot].y = y;
    conveyors[slot].angle = lastConveyorAngle;
    conveyors[slot].active = true;
    conveyors[slot].reversed = false;
    conveyors[slot].isBack = showBackSide;
}

// コンベアを描画
static void drawConveyors(void) {
    for (int i = 0; i < conveyorCount; i++) {
        if (!conveyors[i].active) continue;
        // 現在表示中の面のみ描画
        if (conveyors[i].isBack != showBackSide) continue;

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

// 壁を配置
static void placeWall(float x, float y) {
    x = snapToGrid(x);
    y = snapToGrid(y);

    int slot = -1;
    for (int i = 0; i < wallCount; i++) {
        if (!walls[i].active) { slot = i; break; }
    }
    if (slot < 0 && wallCount < MAX_WALL) {
        slot = wallCount++;
    }
    if (slot < 0) return;

    // キネマティックボディ（回転できるように）
    cpBody *body = cpBodyNewKinematic();
    cpBodySetPosition(body, cpv(x, y));
    cpBodySetAngle(body, lastWallAngle);
    cpSpaceAddBody(space, body);

    // 壁の形状
    cpFloat hw = WALL_LENGTH / 2.0f;
    cpFloat hh = WALL_THICKNESS / 2.0f;
    cpVect verts[4] = {
        cpv(-hw, -hh), cpv(-hw, hh), cpv(hw, hh), cpv(hw, -hh)
    };

    cpShape *shape = cpPolyShapeNew(body, 4, verts, cpTransformIdentity, 0.0f);
    cpShapeSetFriction(shape, 0.8f);
    if (showBackSide) {
        cpShapeSetFilter(shape, cpShapeFilterNew(0, CATEGORY_STRUCTURE_BACK, MASK_BACK));
    } else {
        cpShapeSetFilter(shape, cpShapeFilterNew(0, CATEGORY_STRUCTURE_FRONT, MASK_FRONT));
    }
    cpSpaceAddShape(space, shape);

    walls[slot].body = body;
    walls[slot].shape = shape;
    walls[slot].active = true;
    walls[slot].isBack = showBackSide;
}

// 壁を描画
static void drawWalls(void) {
    for (int i = 0; i < wallCount; i++) {
        if (!walls[i].active) continue;
        // 現在表示中の面のみ描画
        if (walls[i].isBack != showBackSide) continue;

        cpVect pos = cpBodyGetPosition(walls[i].body);
        float angle = (float)cpBodyGetAngle(walls[i].body);

        // 壁本体
        Rectangle rect = {(float)pos.x, (float)pos.y, WALL_LENGTH, WALL_THICKNESS};
        Vector2 origin = {WALL_LENGTH / 2.0f, WALL_THICKNESS / 2.0f};
        DrawRectanglePro(rect, origin, angle * 180.0f / PI, DARKGRAY);

        // 中心に回転用の丸を描画
        DrawCircle((int)pos.x, (int)pos.y, WALL_HANDLE_RADIUS, GRAY);
        DrawCircleLines((int)pos.x, (int)pos.y, WALL_HANDLE_RADIUS, WHITE);
    }
}

// Transferを配置
static void placeTransfer(float x, float y) {
    x = snapToGrid(x);
    y = snapToGrid(y);

    int slot = -1;
    for (int i = 0; i < transferCount; i++) {
        if (!transfers[i].active) { slot = i; break; }
    }
    if (slot < 0 && transferCount < MAX_TRANSFER) {
        slot = transferCount++;
    }
    if (slot < 0) return;

    transfers[slot].x = x;
    transfers[slot].y = y;
    transfers[slot].active = true;
    // 表示中の面から反対の面へ送る
    transfers[slot].toBack = !showBackSide;  // Front表示中なら表→裏、Back表示中なら裏→表
}

// Transferを描画（両面で表示）
static void drawTransfers(void) {
    for (int i = 0; i < transferCount; i++) {
        if (!transfers[i].active) continue;

        float x = transfers[i].x;
        float y = transfers[i].y;
        float halfSize = TRANSFER_SIZE / 2.0f;
        bool toBack = transfers[i].toBack;

        // 入口側（送り出し側）か出口側（受け入れ側）か
        bool isEntrance = (toBack && !showBackSide) || (!toBack && showBackSide);

        // 入口は濃く、出口は薄く表示
        Color color;
        if (isEntrance) {
            color = toBack ? (Color){50, 150, 255, 150} : (Color){255, 150, 50, 150};
        } else {
            color = toBack ? (Color){50, 150, 255, 50} : (Color){255, 150, 50, 50};
        }
        DrawRectangle((int)(x - halfSize), (int)(y - halfSize),
            (int)TRANSFER_SIZE, (int)TRANSFER_SIZE, color);

        // 枠線
        DrawRectangleLines((int)(x - halfSize), (int)(y - halfSize),
            (int)TRANSFER_SIZE, (int)TRANSFER_SIZE, WHITE);

        // 方向テキスト
        const char *label = toBack ? "F>B" : "B>F";
        DrawText(label, (int)(x - 12), (int)(y - 8), 16, WHITE);
    }
}

// Transfer内のデブリ/水を反対側の面に転送
static void updateTransfers(void) {
    float halfSize = TRANSFER_SIZE / 2.0f;

    for (int t = 0; t < transferCount; t++) {
        if (!transfers[t].active) continue;

        float tx = transfers[t].x;
        float ty = transfers[t].y;
        float left = tx - halfSize;
        float right = tx + halfSize;
        float top = ty - halfSize;
        float bottom = ty + halfSize;
        bool toBack = transfers[t].toBack;

        // デブリをチェック
        for (int i = 0; i < debrisCount; i++) {
            if (!debris[i].active) continue;
            // 入口側の面にいるオブジェクトのみ転送（出口からは入らない）
            // toBack=true(F→B): Frontにいる(isBack=false)オブジェクトのみ
            // toBack=false(B→F): Backにいる(isBack=true)オブジェクトのみ
            if (debris[i].isBack == toBack) continue;

            cpVect pos;
            if (debris[i].isStatic) {
                pos = debris[i].staticPos;
            } else {
                pos = cpBodyGetPosition(debris[i].body);
            }

            // デブリがTransfer領域内に完全に入っているか
            float margin = debris[i].size * 0.5f;
            if (pos.x - margin >= left && pos.x + margin <= right &&
                pos.y - margin >= top && pos.y + margin <= bottom) {

                // 面を切り替え
                debris[i].isBack = toBack;

                // 衝突フィルタを更新
                if (debris[i].shape) {
                    if (debris[i].isBack) {
                        cpShapeSetFilter(debris[i].shape, cpShapeFilterNew(0, CATEGORY_DEBRIS_BACK, MASK_BACK));
                    } else {
                        cpShapeSetFilter(debris[i].shape, cpShapeFilterNew(0, CATEGORY_DEBRIS_FRONT, MASK_FRONT));
                    }
                }
            }
        }

        // 水パーティクルをチェック
        for (int i = 0; i < waterParticleCount; i++) {
            if (!waterParticles[i].active) continue;
            // 入口側の面にいるオブジェクトのみ転送
            if (waterParticles[i].isBack == toBack) continue;

            cpVect pos = cpBodyGetPosition(waterParticles[i].body);
            float radius = getWaterRadius(waterParticles[i].valence);

            // 水がTransfer領域内に完全に入っているか
            if (pos.x - radius >= left && pos.x + radius <= right &&
                pos.y - radius >= top && pos.y + radius <= bottom) {

                // 面を切り替え
                waterParticles[i].isBack = toBack;

                // 衝突フィルタを更新
                if (waterParticles[i].isBack) {
                    cpShapeSetFilter(waterParticles[i].shape, cpShapeFilterNew(0, CATEGORY_WATER_BACK, MASK_BACK));
                } else {
                    cpShapeSetFilter(waterParticles[i].shape, cpShapeFilterNew(0, CATEGORY_WATER_FRONT, MASK_FRONT));
                }
            }
        }
    }
}

// Outputを配置
static void placeOutput(float x, float y) {
    x = snapToGrid(x);
    y = snapToGrid(y);

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
    if (showBackSide) {
        cpShapeSetFilter(outputs[slot].leftWall, cpShapeFilterNew(0, CATEGORY_STRUCTURE_BACK, MASK_BACK));
    } else {
        cpShapeSetFilter(outputs[slot].leftWall, cpShapeFilterNew(0, CATEGORY_STRUCTURE_FRONT, MASK_FRONT));
    }
    cpSpaceAddShape(space, outputs[slot].leftWall);

    // 右壁
    cpTransform rightT = cpTransformTranslate(cpv(ox + OUTPUT_SIZE, oy + OUTPUT_SIZE / 2.0f));
    outputs[slot].rightWall = cpPolyShapeNew(staticBody, 4, leftVerts, rightT, 0.0f);
    cpShapeSetFriction(outputs[slot].rightWall, 0.5f);
    if (showBackSide) {
        cpShapeSetFilter(outputs[slot].rightWall, cpShapeFilterNew(0, CATEGORY_STRUCTURE_BACK, MASK_BACK));
    } else {
        cpShapeSetFilter(outputs[slot].rightWall, cpShapeFilterNew(0, CATEGORY_STRUCTURE_FRONT, MASK_FRONT));
    }
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
    if (showBackSide) {
        cpShapeSetFilter(outputs[slot].bottomWall, cpShapeFilterNew(0, CATEGORY_STRUCTURE_BACK, MASK_BACK));
    } else {
        cpShapeSetFilter(outputs[slot].bottomWall, cpShapeFilterNew(0, CATEGORY_STRUCTURE_FRONT, MASK_FRONT));
    }
    cpSpaceAddShape(space, outputs[slot].bottomWall);

    outputs[slot].active = true;
    outputs[slot].isBack = showBackSide;
}

// Outputを描画
static void drawOutputs(void) {
    for (int i = 0; i < outputCount; i++) {
        if (!outputs[i].active) continue;
        // 現在表示中の面のみ描画
        if (outputs[i].isBack != showBackSide) continue;
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

// Inputを配置
static void placeInput(float x, float y) {
    x = snapToGrid(x);
    y = snapToGrid(y);

    int slot = -1;
    for (int i = 0; i < inputCount; i++) {
        if (!inputs[i].active) { slot = i; break; }
    }
    if (slot < 0 && inputCount < MAX_INPUT_BOX) {
        slot = inputCount++;
    }
    if (slot < 0) return;

    float ix = x - INPUT_SIZE / 2.0f;
    float iy = y - INPUT_SIZE / 2.0f;
    inputs[slot].x = ix;
    inputs[slot].y = iy;

    cpBody *staticBody = cpSpaceGetStaticBody(space);
    cpFloat hw = INPUT_WALL_THICKNESS / 2.0f;
    cpFloat hh = INPUT_SIZE / 2.0f;

    // 左壁
    cpVect leftVerts[4] = {
        cpv(-hw, -hh), cpv(-hw, hh), cpv(hw, hh), cpv(hw, -hh)
    };
    cpTransform leftT = cpTransformTranslate(cpv(ix, iy + INPUT_SIZE / 2.0f));
    inputs[slot].leftWall = cpPolyShapeNew(staticBody, 4, leftVerts, leftT, 0.0f);
    cpShapeSetFriction(inputs[slot].leftWall, 0.5f);
    if (showBackSide) {
        cpShapeSetFilter(inputs[slot].leftWall, cpShapeFilterNew(0, CATEGORY_STRUCTURE_BACK, MASK_BACK));
    } else {
        cpShapeSetFilter(inputs[slot].leftWall, cpShapeFilterNew(0, CATEGORY_STRUCTURE_FRONT, MASK_FRONT));
    }
    cpSpaceAddShape(space, inputs[slot].leftWall);

    // 右壁
    cpTransform rightT = cpTransformTranslate(cpv(ix + INPUT_SIZE, iy + INPUT_SIZE / 2.0f));
    inputs[slot].rightWall = cpPolyShapeNew(staticBody, 4, leftVerts, rightT, 0.0f);
    cpShapeSetFriction(inputs[slot].rightWall, 0.5f);
    if (showBackSide) {
        cpShapeSetFilter(inputs[slot].rightWall, cpShapeFilterNew(0, CATEGORY_STRUCTURE_BACK, MASK_BACK));
    } else {
        cpShapeSetFilter(inputs[slot].rightWall, cpShapeFilterNew(0, CATEGORY_STRUCTURE_FRONT, MASK_FRONT));
    }
    cpSpaceAddShape(space, inputs[slot].rightWall);

    // 上壁
    cpFloat thw = INPUT_SIZE / 2.0f;
    cpFloat thh = INPUT_WALL_THICKNESS / 2.0f;
    cpVect topVerts[4] = {
        cpv(-thw, -thh), cpv(-thw, thh), cpv(thw, thh), cpv(thw, -thh)
    };
    cpTransform topT = cpTransformTranslate(cpv(ix + INPUT_SIZE / 2.0f, iy));
    inputs[slot].topWall = cpPolyShapeNew(staticBody, 4, topVerts, topT, 0.0f);
    cpShapeSetFriction(inputs[slot].topWall, 0.5f);
    if (showBackSide) {
        cpShapeSetFilter(inputs[slot].topWall, cpShapeFilterNew(0, CATEGORY_STRUCTURE_BACK, MASK_BACK));
    } else {
        cpShapeSetFilter(inputs[slot].topWall, cpShapeFilterNew(0, CATEGORY_STRUCTURE_FRONT, MASK_FRONT));
    }
    cpSpaceAddShape(space, inputs[slot].topWall);

    inputs[slot].active = true;
    inputs[slot].isBack = showBackSide;
}

// Inputを描画
static void drawInputs(void) {
    for (int i = 0; i < inputCount; i++) {
        if (!inputs[i].active) continue;
        // 現在表示中の面のみ描画
        if (inputs[i].isBack != showBackSide) continue;
        float ix = inputs[i].x;
        float iy = inputs[i].y;

        // 背景（半透明）
        DrawRectangle((int)ix, (int)iy, (int)INPUT_SIZE, (int)INPUT_SIZE,
            (Color){100, 128, 100, 50});

        // 左壁
        DrawRectangle((int)(ix - INPUT_WALL_THICKNESS / 2.0f), (int)iy,
            (int)INPUT_WALL_THICKNESS, (int)INPUT_SIZE, DARKGRAY);
        // 右壁
        DrawRectangle((int)(ix + INPUT_SIZE - INPUT_WALL_THICKNESS / 2.0f), (int)iy,
            (int)INPUT_WALL_THICKNESS, (int)INPUT_SIZE, DARKGRAY);
        // 上壁
        DrawRectangle((int)ix, (int)(iy - INPUT_WALL_THICKNESS / 2.0f),
            (int)INPUT_SIZE, (int)INPUT_WALL_THICKNESS, DARKGRAY);

        DrawText("INPUT", (int)ix + 70, (int)iy + 90, 20, (Color){60, 80, 60, 200});
    }
}

// Input内が空なら土砂を生成
static void checkAndFillInputs(void) {
    for (int inp = 0; inp < inputCount; inp++) {
        if (!inputs[inp].active) continue;

        float ix = inputs[inp].x;
        float iy = inputs[inp].y;

        // このInput内にデブリがあるかチェック（同じ面のみ）
        bool hasDebris = false;
        for (int i = 0; i < debrisCount; i++) {
            if (!debris[i].active) continue;
            if (debris[i].isBack != inputs[inp].isBack) continue;  // 同じ面のみ

            cpVect pos;
            if (debris[i].isStatic) {
                pos = debris[i].staticPos;
            } else if (debris[i].body) {
                pos = cpBodyGetPosition(debris[i].body);
            } else {
                continue;
            }

            // Input内にいるか
            if (pos.x >= ix && pos.x <= ix + INPUT_SIZE &&
                pos.y >= iy && pos.y <= iy + INPUT_SIZE) {
                hasDebris = true;
                break;
            }
        }

        // 空なら土砂を生成（3個）
        if (!hasDebris) {
            for (int s = 0; s < 3; s++) {
                float r = rand01();
                float spawnX = ix + 20.0f + rand01() * (INPUT_SIZE - 40.0f);
                float spawnY = iy + 20.0f + rand01() * (INPUT_SIZE - 40.0f);

                // spawnDebris系の関数は固定位置に生成するので、ここで直接生成
                int slot = findFreeSlot();
                if (slot < 0) continue;

                // サイズと種類を決定
                float size;
                DebrisType type;
                float density;
                if (r < 0.15f) {
                    // 粘土
                    size = 5.0f + rand01() * 15.0f;
                    type = DEBRIS_CLAY;
                    density = 1.8f;
                } else if (r < 0.40f) {
                    // 木
                    size = 10.0f + rand01() * 50.0f;
                    type = DEBRIS_WOOD;
                    density = 0.7f;
                } else {
                    // 石
                    size = 2.0f + powf(rand01(), 2.0f) * 28.0f;
                    type = DEBRIS_STONE;
                    density = 0.7f + rand01() * 2.0f;
                }

                int vertexCount;
                cpVect points[MAX_VERTICES];

                if (type == DEBRIS_WOOD) {
                    // 木は長方形
                    float aspectRatio = 3.0f + rand01() * 5.0f;
                    float length = size;
                    float width = size / aspectRatio;
                    if (width < 1.5f) width = 1.5f;
                    float hw = length / 2.0f;
                    float hh = width / 2.0f;
                    points[0] = cpv(-hw, -hh);
                    points[1] = cpv(-hw, hh);
                    points[2] = cpv(hw, hh);
                    points[3] = cpv(hw, -hh);
                    vertexCount = 4;
                } else {
                    // 石・粘土は多角形
                    vertexCount = 3 + rand() % 5;
                    float angles[MAX_VERTICES];
                    for (int j = 0; j < vertexCount; j++) {
                        angles[j] = ((float)j / vertexCount + rand01() * 0.1f) * 2.0f * PI;
                    }
                    qsort(angles, vertexCount, sizeof(float), compareFloats);
                    for (int j = 0; j < vertexCount; j++) {
                        float rad = size * (0.7f + rand01() * 0.3f);
                        points[j] = cpv(rad * cosf(angles[j]), rad * sinf(angles[j]));
                    }
                }

                float area = calcPolygonArea(points, vertexCount);
                cpFloat mass = area * density;
                cpFloat moment = cpMomentForPoly(mass, vertexCount, points, cpvzero, 0.0f);
                cpBody *body = cpBodyNew(mass, moment);
                cpBodySetPosition(body, cpv(spawnX, spawnY));
                cpBodySetAngle(body, rand01() * 2.0f * PI);
                cpSpaceAddBody(space, body);

                cpShape *shape = cpPolyShapeNew(body, vertexCount, points, cpTransformIdentity, 0.0f);
                cpShapeSetFriction(shape, type == DEBRIS_CLAY ? 0.9f : (type == DEBRIS_WOOD ? 0.6f : 0.5f));
                float elasticity = type == DEBRIS_CLAY ? 0.05f : (type == DEBRIS_WOOD ? 0.2f : 0.1f);
                cpShapeSetElasticity(shape, elasticity);
                cpShapeSetCollisionType(shape, COLLISION_TYPE_DEBRIS);
                if (inputs[inp].isBack) {
                    cpShapeSetFilter(shape, cpShapeFilterNew(0, CATEGORY_DEBRIS_BACK, MASK_BACK));
                } else {
                    cpShapeSetFilter(shape, cpShapeFilterNew(0, CATEGORY_DEBRIS_FRONT, MASK_FRONT));
                }
                cpShapeSetUserData(shape, (void*)(intptr_t)slot);
                cpSpaceAddShape(space, shape);

                debris[slot].body = body;
                debris[slot].shape = shape;
                debris[slot].vertexCount = vertexCount;
                for (int j = 0; j < vertexCount; j++) {
                    debris[slot].vertices[j] = points[j];
                }
                debris[slot].size = size;
                debris[slot].density = density;
                debris[slot].type = type;
                debris[slot].active = true;
                debris[slot].isStatic = false;
                debris[slot].wakeUp = false;
                debris[slot].shouldBreak = false;
                debris[slot].breakFlash = 0;
                debris[slot].idleTime = 0.0f;
                debris[slot].lastPos = cpv(spawnX, spawnY);
                debris[slot].isBack = inputs[inp].isBack;  // Inputと同じ面に生成
                if (slot >= debrisCount) debrisCount = slot + 1;
            }
        }
    }
}

// ユーザーふるいを配置
static void placeUserSieve(float x, float y) {
    x = snapToGrid(x);
    y = snapToGrid(y);

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
    cpBodySetAngle(body, lastSieveAngle);  // 最後に置いたふるいと同じ角度
    cpSpaceAddBody(space, body);

    cpFloat hw = SIEVE_LENGTH / 2.0f;
    cpFloat hh = SIEVE_THICKNESS / 2.0f;
    cpVect verts[4] = {
        cpv(-hw, -hh), cpv(-hw, hh), cpv(hw, hh), cpv(hw, -hh)
    };
    cpShape *shape = cpPolyShapeNew(body, 4, verts, cpTransformIdentity, 0.0f);
    cpShapeSetFriction(shape, 0.5f);
    if (showBackSide) {
        cpShapeSetFilter(shape, cpShapeFilterNew(0, CATEGORY_STRUCTURE_BACK, MASK_BACK));
    } else {
        cpShapeSetFilter(shape, cpShapeFilterNew(0, CATEGORY_STRUCTURE_FRONT, MASK_FRONT));
    }
    cpSpaceAddShape(space, shape);

    userSieves[slot].body = body;
    userSieves[slot].shape = shape;
    userSieves[slot].active = true;
    userSieves[slot].isBack = showBackSide;
}

// ユーザーふるいを描画
static void drawUserSieves(void) {
    for (int i = 0; i < userSieveCount; i++) {
        if (!userSieves[i].active) continue;
        // 現在表示中の面のみ描画
        if (userSieves[i].isBack != showBackSide) continue;

        cpVect pos = cpBodyGetPosition(userSieves[i].body);
        cpFloat angle = cpBodyGetAngle(userSieves[i].body);

        Rectangle rect = {(float)pos.x, (float)pos.y, SIEVE_LENGTH, SIEVE_THICKNESS};
        Vector2 origin = {SIEVE_LENGTH / 2.0f, SIEVE_THICKNESS / 2.0f};
        DrawRectanglePro(rect, origin, angle * 180.0f / PI, DARKGRAY);

        // 中心ハンドル
        DrawCircle((int)pos.x, (int)pos.y, SIEVE_HANDLE_RADIUS, DARKGRAY);
        DrawCircleLines((int)pos.x, (int)pos.y, SIEVE_HANDLE_RADIUS, WHITE);
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
        cpShapeSetFilter(shape, cpShapeFilterNew(0, CATEGORY_STRUCTURE_FRONT, MASK_FRONT));
        cpSpaceAddShape(space, shape);

        sieveBodies[i] = body;
        sieveShapes[i] = shape;
    }
}

static void updateSieveVibration(float time) {
    float omega = 2.0f * PI * SIEVE_FREQ;
    float velocityPhase = cosf(time * omega);
    float speed = SIEVE_AMP * omega * velocityPhase;

    // ユーザー配置ふるいを振動（各ふるいの面に垂直な方向）
    for (int i = 0; i < userSieveCount; i++) {
        if (!userSieves[i].active) continue;
        float sieveAngle = (float)cpBodyGetAngle(userSieves[i].body);
        float normalAngle = sieveAngle + PI / 2.0f;  // 面に垂直な方向
        float vx = speed * cosf(normalAngle);
        float vy = speed * sinf(normalAngle);
        cpBodySetVelocity(userSieves[i].body, cpv(vx, vy));
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
    cpShapeSetFriction(groundShape, 3.0f);  // 強い静止摩擦
    cpShapeSetFilter(groundShape, cpShapeFilterNew(0, CATEGORY_GROUND, CP_ALL_CATEGORIES));
    cpSpaceAddShape(space, groundShape);

    // 左壁
    cpFloat wallHW = 10.0f;
    cpFloat wallHH = SCREEN_HEIGHT / 2.0f;
    cpVect leftVerts[4] = {
        cpv(-wallHW, -wallHH), cpv(-wallHW, wallHH), cpv(wallHW, wallHH), cpv(wallHW, -wallHH)
    };
    leftWallShape = cpPolyShapeNew(staticBody, 4, leftVerts,
        cpTransformTranslate(cpv(-10.0f, SCREEN_HEIGHT / 2.0f)), 0.0f);
    cpShapeSetFriction(leftWallShape, 3.0f);  // 強い静止摩擦
    cpShapeSetFilter(leftWallShape, cpShapeFilterNew(0, CATEGORY_GROUND, CP_ALL_CATEGORIES));
    cpSpaceAddShape(space, leftWallShape);

    // 右壁（フィールド右端）
    cpVect rightVerts[4] = {
        cpv(-wallHW, -wallHH), cpv(-wallHW, wallHH), cpv(wallHW, wallHH), cpv(wallHW, -wallHH)
    };
    rightWallShape = cpPolyShapeNew(staticBody, 4, rightVerts,
        cpTransformTranslate(cpv(FIELD_WIDTH + 10.0f, SCREEN_HEIGHT / 2.0f)), 0.0f);
    cpShapeSetFriction(rightWallShape, 3.0f);  // 強い静止摩擦
    cpShapeSetFilter(rightWallShape, cpShapeFilterNew(0, CATEGORY_GROUND, CP_ALL_CATEGORIES));
    cpSpaceAddShape(space, rightWallShape);
}

static void drawGround(void) {
    DrawRectangle(0, SCREEN_HEIGHT - 20, FIELD_WIDTH, 20, DARKGRAY);
}

// 背景グリッドを描画
static void drawGrid(float camX, float camY, float camZoom) {
    Color gridColor = (Color){200, 200, 200, 40};

    // 可視範囲を計算
    float halfW = (SCREEN_WIDTH / 2.0f) / camZoom;
    float halfH = (SCREEN_HEIGHT / 2.0f) / camZoom;
    float centerX = camX + SCREEN_WIDTH / 2.0f;
    float centerY = camY + SCREEN_HEIGHT / 2.0f;

    int minX = (int)((centerX - halfW) / GRID_UNIT) - 1;
    int maxX = (int)((centerX + halfW) / GRID_UNIT) + 1;
    int minY = (int)((centerY - halfH) / GRID_UNIT) - 1;
    int maxY = (int)((centerY + halfH) / GRID_UNIT) + 1;

    // 縦線
    for (int x = minX; x <= maxX; x++) {
        float wx = x * GRID_UNIT;
        DrawLine((int)wx, minY * (int)GRID_UNIT, (int)wx, maxY * (int)GRID_UNIT, gridColor);
    }
    // 横線
    for (int y = minY; y <= maxY; y++) {
        float wy = y * GRID_UNIT;
        DrawLine(minX * (int)GRID_UNIT, (int)wy, maxX * (int)GRID_UNIT, (int)wy, gridColor);
    }
}

// モード選択メニューを描画
static void drawModeMenu(void) {
    // 背景（半透明グレー）
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){128, 128, 128, 200});

    // ボタン配置（2列レイアウト）
    int buttonW = 180, buttonH = 40;
    int startX = 50, startY = 50;
    int gap = 10;

    for (int i = 1; i < modeCount; i++) {
        int col = (i - 1) / 10;
        int row = (i - 1) % 10;
        int x = startX + col * (buttonW + 30);
        int y = startY + row * (buttonH + gap);

        // ボタン描画
        Rectangle btn = {x, y, buttonW, buttonH};
        Vector2 mouse = GetMousePosition();
        bool hover = CheckCollisionPointRec(mouse, btn);
        DrawRectangleRec(btn, hover ? BLUE : (Color){70, 130, 180, 255});
        DrawRectangleLinesEx(btn, 2, DARKBLUE);
        DrawText(actionModeNames[i], x + 10, y + 10, 20, WHITE);

        // クリック検出
        if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            actionMode = i;
            showModeMenu = false;
        }
    }

    // ESCで閉じる
    if (IsKeyPressed(KEY_ESCAPE)) {
        showModeMenu = false;
    }
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sieve Simulation (Chipmunk2D)");
    SetTargetFPS(60);
    srand(42);

    space = cpHastySpaceNew();
    cpHastySpaceSetThreads(space, 0);  // 0 = 自動検出
    cpSpaceSetGravity(space, cpv(0.0f, 9.8f * SCALE));
    cpSpaceSetIterations(space, 50);  // 増加: より正確な拘束解決
    cpSpaceSetCollisionSlop(space, 0.1f);  // 減少: めり込み許容を小さく
    cpSpaceSetCollisionBias(space, cpfpow(0.9f, 60.0f));  // 増加: 素早く位置補正
    cpSpaceSetDamping(space, 0.9f);  // ダンピング: 振動を抑える
    // スリープ設定：閾値を小さくして傾いた状態で止まらないように
    cpSpaceSetSleepTimeThreshold(space, 0.5f);
    cpSpaceSetIdleSpeedThreshold(space, 5.0f);

    createGround();

    // 衝突ハンドラを登録
    cpCollisionHandler *handler = cpSpaceAddDefaultCollisionHandler(space);
    handler->postSolveFunc = postSolveHandler;

    float physicsTimeMs = 0.0f;

    // カメラをフィールド中央に初期化
    cameraX = FIELD_WIDTH / 2.0f - SCREEN_WIDTH / 2.0f;

    // マウスドラッグ用
    bool dragging = false;
    float dragStartX = 0.0f;
    float dragStartY = 0.0f;
    float cameraStartX = 0.0f;
    float cameraStartY = 0.0f;

    // コンベア操作用
    int clickedConveyor = -1;
    int draggingConveyor = -1;
    cpVect conveyorDragOffset = cpvzero;
    // ふるい操作用
    int clickedSieve = -1;
    int draggingSieve = -1;
    cpVect sieveDragOffset = cpvzero;
    // 壁操作用
    int clickedWall = -1;
    int draggingWall = -1;
    cpVect wallDragOffset = cpvzero;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // カメラ用のワールド座標計算（回転判定用）
        Camera2D camForRotate = {0};
        camForRotate.target = (Vector2){cameraX + SCREEN_WIDTH / 2.0f, cameraY + SCREEN_HEIGHT / 2.0f};
        camForRotate.offset = (Vector2){SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
        camForRotate.zoom = zoom;
        Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camForRotate);

        // マウスドラッグでスクロール または 回転/移動
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            clickedConveyor = -1;
            clickedSieve = -1;
            clickedWall = -1;
            draggingConveyor = -1;
            draggingSieve = -1;
            draggingWall = -1;

            // モード4, 6, 9の場合、コンベア/ふるい/壁の中心ハンドルをクリックしたか確認
            if (actionMode == 4) {
                for (int i = 0; i < conveyorCount; i++) {
                    if (!conveyors[i].active) continue;
                    cpVect pos = cpBodyGetPosition(conveyors[i].body);
                    float dist = sqrtf(powf(mouseWorldPos.x - (float)pos.x, 2) + powf(mouseWorldPos.y - (float)pos.y, 2));
                    if (dist < CONVEYOR_HANDLE_RADIUS) {
                        clickedConveyor = i;
                        conveyorDragOffset = cpvsub(pos, cpv(mouseWorldPos.x, mouseWorldPos.y));
                        break;
                    }
                }
            } else if (actionMode == 6) {
                for (int i = 0; i < userSieveCount; i++) {
                    if (!userSieves[i].active) continue;
                    cpVect pos = cpBodyGetPosition(userSieves[i].body);
                    float dist = sqrtf(powf(mouseWorldPos.x - (float)pos.x, 2) + powf(mouseWorldPos.y - (float)pos.y, 2));
                    if (dist < SIEVE_HANDLE_RADIUS) {
                        clickedSieve = i;
                        sieveDragOffset = cpvsub(pos, cpv(mouseWorldPos.x, mouseWorldPos.y));
                        break;
                    }
                }
            } else if (actionMode == 9) {
                for (int i = 0; i < wallCount; i++) {
                    if (!walls[i].active) continue;
                    cpVect pos = cpBodyGetPosition(walls[i].body);
                    float dist = sqrtf(powf(mouseWorldPos.x - (float)pos.x, 2) + powf(mouseWorldPos.y - (float)pos.y, 2));
                    if (dist < WALL_HANDLE_RADIUS) {
                        clickedWall = i;
                        wallDragOffset = cpvsub(pos, cpv(mouseWorldPos.x, mouseWorldPos.y));
                        break;
                    }
                }
            }

            // 操作対象がなければカメラドラッグモード
            if (clickedConveyor < 0 && clickedSieve < 0 && clickedWall < 0) {
                dragging = true;
                dragStartX = GetMouseX();
                dragStartY = GetMouseY();
                cameraStartX = cameraX;
                cameraStartY = cameraY;
            }
        }
        // コンベアのドラッグ移動（動かしたらドラッグ開始）
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && clickedConveyor >= 0) {
            cpVect currentConvPos = cpBodyGetPosition(conveyors[clickedConveyor].body);
            cpVect targetPos = cpvadd(cpv(mouseWorldPos.x, mouseWorldPos.y), conveyorDragOffset);
            float moveDist = (float)cpvlength(cpvsub(targetPos, currentConvPos));
            if (moveDist > 5.0f) {  // 5ピクセル以上動いたらドラッグ開始
                draggingConveyor = clickedConveyor;
            }
            if (draggingConveyor >= 0) {
                cpBodySetPosition(conveyors[draggingConveyor].body, targetPos);
            }
        }
        // ふるいのドラッグ移動（動かしたらドラッグ開始）
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && clickedSieve >= 0) {
            cpVect currentSievePos = cpBodyGetPosition(userSieves[clickedSieve].body);
            cpVect targetPos = cpvadd(cpv(mouseWorldPos.x, mouseWorldPos.y), sieveDragOffset);
            float moveDist = (float)cpvlength(cpvsub(targetPos, currentSievePos));
            if (moveDist > 5.0f) {  // 5ピクセル以上動いたらドラッグ開始
                draggingSieve = clickedSieve;
            }
            if (draggingSieve >= 0) {
                cpBodySetPosition(userSieves[draggingSieve].body, targetPos);
            }
        }
        // 壁のドラッグ移動（動かしたらドラッグ開始）
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && clickedWall >= 0) {
            cpVect currentWallPos = cpBodyGetPosition(walls[clickedWall].body);
            cpVect targetPos = cpvadd(cpv(mouseWorldPos.x, mouseWorldPos.y), wallDragOffset);
            float moveDist = (float)cpvlength(cpvsub(targetPos, currentWallPos));
            if (moveDist > 5.0f) {  // 5ピクセル以上動いたらドラッグ開始
                draggingWall = clickedWall;
            }
            if (draggingWall >= 0) {
                cpBodySetPosition(walls[draggingWall].body, targetPos);
            }
        }
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            // ドラッグしなかった場合のみ15度回転
            if (clickedConveyor >= 0 && draggingConveyor < 0) {
                cpFloat currentAngle = cpBodyGetAngle(conveyors[clickedConveyor].body);
                cpFloat newAngle = currentAngle + 15.0f * PI / 180.0f;
                cpBodySetAngle(conveyors[clickedConveyor].body, newAngle);
                lastConveyorAngle = (float)newAngle;  // 次に置くコンベアも同じ角度にする
            }
            if (clickedSieve >= 0 && draggingSieve < 0) {
                cpFloat currentAngle = cpBodyGetAngle(userSieves[clickedSieve].body);
                cpFloat newAngle = currentAngle + 15.0f * PI / 180.0f;
                cpBodySetAngle(userSieves[clickedSieve].body, newAngle);
                lastSieveAngle = (float)newAngle;  // 次に置くふるいも同じ角度にする
            }
            if (clickedWall >= 0 && draggingWall < 0) {
                cpFloat currentAngle = cpBodyGetAngle(walls[clickedWall].body);
                cpFloat newAngle = currentAngle + 15.0f * PI / 180.0f;
                cpBodySetAngle(walls[clickedWall].body, newAngle);
                lastWallAngle = (float)newAngle;  // 次に置く壁も同じ角度にする
            }
            dragging = false;
            clickedConveyor = -1;
            draggingConveyor = -1;
            clickedSieve = -1;
            draggingSieve = -1;
            clickedWall = -1;
            draggingWall = -1;
        }
        if (dragging) {
            float dx = GetMouseX() - dragStartX;
            float dy = GetMouseY() - dragStartY;
            cameraX = cameraStartX - dx / zoom;
            cameraY = cameraStartY - dy / zoom;
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

        // マウスホイールでズーム（ポインタ位置を中心に）
        float wheel = GetMouseWheelMove();
        if (wheel != 0) {
            Vector2 mousePos = GetMousePosition();
            // ズーム前のワールド座標
            float worldXBefore = cameraX + (mousePos.x - SCREEN_WIDTH / 2.0f) / zoom + SCREEN_WIDTH / 2.0f;
            float worldYBefore = cameraY + (mousePos.y - SCREEN_HEIGHT / 2.0f) / zoom + SCREEN_HEIGHT / 2.0f;

            float zoomDelta = wheel * 0.1f;
            zoom += zoomDelta;
            if (zoom < 0.1f) zoom = 0.1f;
            if (zoom > 3.0f) zoom = 3.0f;

            // ズーム後に同じワールド座標がマウス位置に来るようにカメラ調整
            cameraX = worldXBefore - SCREEN_WIDTH / 2.0f - (mousePos.x - SCREEN_WIDTH / 2.0f) / zoom;
            cameraY = worldYBefore - SCREEN_HEIGHT / 2.0f - (mousePos.y - SCREEN_HEIGHT / 2.0f) / zoom;
        }

        // Eキーでモード選択メニュー表示
        if (IsKeyPressed(KEY_E)) {
            showModeMenu = !showModeMenu;
        }

        // Fキーで表面/裏面切替
        if (IsKeyPressed(KEY_F)) {
            showBackSide = !showBackSide;
        }

        // ワールド座標を取得（描画やプレビューで使用）
        Camera2D cam = {0};
        cam.target = (Vector2){cameraX + SCREEN_WIDTH / 2.0f, cameraY + SCREEN_HEIGHT / 2.0f};
        cam.offset = (Vector2){SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
        cam.rotation = 0.0f;
        cam.zoom = zoom;
        Vector2 mouseWorld = GetScreenToWorld2D(GetMousePosition(), cam);
        float worldX = mouseWorld.x;
        float worldY = mouseWorld.y;

        // 1-9キーで操作モード切替（メニュー非表示時のみ）
        if (!showModeMenu) {
        if (IsKeyPressed(KEY_ONE)) actionMode = 1;
        if (IsKeyPressed(KEY_TWO)) actionMode = 2;
        if (IsKeyPressed(KEY_THREE)) actionMode = 3;
        if (IsKeyPressed(KEY_FOUR)) actionMode = 4;
        if (IsKeyPressed(KEY_FIVE)) actionMode = 5;
        if (IsKeyPressed(KEY_SIX)) actionMode = 6;
        if (IsKeyPressed(KEY_SEVEN)) actionMode = 7;
        if (IsKeyPressed(KEY_EIGHT)) actionMode = 8;
        if (IsKeyPressed(KEY_NINE)) actionMode = 9;

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
            // モード3: 水配置（右クリック押し続けで連続生成）
            if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
                spawnWaterParticle(worldX, worldY);
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
        } else if (actionMode == 7) {
            // モード7: Input配置
            if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                placeInput(worldX, worldY);
            }
        } else if (actionMode == 8) {
            // モード8: 削除
            if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                // まずdebrisをチェック
                cpPointQueryInfo info;
                cpShape *shape = cpSpacePointQueryNearest(space, cpv(worldX, worldY), 10.0f, CP_SHAPE_FILTER_ALL, &info);
                bool deleted = false;
                if (shape && cpShapeGetCollisionType(shape) == COLLISION_TYPE_DEBRIS) {
                    int idx = (int)(intptr_t)cpShapeGetUserData(shape);
                    if (idx >= 0 && idx < debrisCount && debris[idx].active) {
                        cpSpaceRemoveShape(space, debris[idx].shape);
                        cpShapeFree(debris[idx].shape);
                        if (!debris[idx].isStatic && debris[idx].body) {
                            cpSpaceRemoveBody(space, debris[idx].body);
                            cpBodyFree(debris[idx].body);
                        }
                        debris[idx].active = false;
                        debris[idx].body = NULL;
                        debris[idx].shape = NULL;
                        deleted = true;
                    }
                }
                if (!deleted) {
                    // コンベアをチェック
                    for (int i = 0; i < conveyorCount && !deleted; i++) {
                        if (!conveyors[i].active) continue;
                        cpVect pos = cpBodyGetPosition(conveyors[i].body);
                        float dist = sqrtf(powf(worldX - (float)pos.x, 2) + powf(worldY - (float)pos.y, 2));
                        if (dist < CONVEYOR_LENGTH / 2.0f) {
                            cpSpaceRemoveShape(space, conveyors[i].shape);
                            cpSpaceRemoveBody(space, conveyors[i].body);
                            cpShapeFree(conveyors[i].shape);
                            cpBodyFree(conveyors[i].body);
                            conveyors[i].active = false;
                            deleted = true;
                        }
                    }
                    // ふるいをチェック
                    for (int i = 0; i < userSieveCount && !deleted; i++) {
                        if (!userSieves[i].active) continue;
                        cpVect pos = cpBodyGetPosition(userSieves[i].body);
                        float dist = sqrtf(powf(worldX - (float)pos.x, 2) + powf(worldY - (float)pos.y, 2));
                        if (dist < SIEVE_LENGTH / 2.0f) {
                            cpSpaceRemoveShape(space, userSieves[i].shape);
                            cpSpaceRemoveBody(space, userSieves[i].body);
                            cpShapeFree(userSieves[i].shape);
                            cpBodyFree(userSieves[i].body);
                            userSieves[i].active = false;
                            deleted = true;
                        }
                    }
                    // Outputをチェック
                    for (int i = 0; i < outputCount && !deleted; i++) {
                        if (!outputs[i].active) continue;
                        float ox = outputs[i].x;
                        float oy = outputs[i].y;
                        if (worldX >= ox && worldX <= ox + OUTPUT_SIZE &&
                            worldY >= oy && worldY <= oy + OUTPUT_SIZE) {
                            cpSpaceRemoveShape(space, outputs[i].leftWall);
                            cpSpaceRemoveShape(space, outputs[i].rightWall);
                            cpSpaceRemoveShape(space, outputs[i].bottomWall);
                            cpShapeFree(outputs[i].leftWall);
                            cpShapeFree(outputs[i].rightWall);
                            cpShapeFree(outputs[i].bottomWall);
                            outputs[i].active = false;
                            deleted = true;
                        }
                    }
                    // Inputをチェック
                    for (int i = 0; i < inputCount && !deleted; i++) {
                        if (!inputs[i].active) continue;
                        float ix = inputs[i].x;
                        float iy = inputs[i].y;
                        if (worldX >= ix && worldX <= ix + INPUT_SIZE &&
                            worldY >= iy && worldY <= iy + INPUT_SIZE) {
                            cpSpaceRemoveShape(space, inputs[i].leftWall);
                            cpSpaceRemoveShape(space, inputs[i].rightWall);
                            cpSpaceRemoveShape(space, inputs[i].topWall);
                            cpShapeFree(inputs[i].leftWall);
                            cpShapeFree(inputs[i].rightWall);
                            cpShapeFree(inputs[i].topWall);
                            inputs[i].active = false;
                            deleted = true;
                        }
                    }
                    // 水パーティクルをチェック
                    for (int i = 0; i < waterParticleCount && !deleted; i++) {
                        if (!waterParticles[i].active) continue;
                        cpVect wpos = cpBodyGetPosition(waterParticles[i].body);
                        float dist = sqrtf(powf(worldX - (float)wpos.x, 2) + powf(worldY - (float)wpos.y, 2));
                        if (dist < WATER_PARTICLE_RADIUS * 2.0f) {
                            cpSpaceRemoveShape(space, waterParticles[i].shape);
                            cpSpaceRemoveBody(space, waterParticles[i].body);
                            cpShapeFree(waterParticles[i].shape);
                            cpBodyFree(waterParticles[i].body);
                            waterParticles[i].active = false;
                            deleted = true;
                        }
                    }
                    // 壁をチェック
                    for (int i = 0; i < wallCount && !deleted; i++) {
                        if (!walls[i].active) continue;
                        cpVect pos = cpBodyGetPosition(walls[i].body);
                        float dist = sqrtf(powf(worldX - (float)pos.x, 2) + powf(worldY - (float)pos.y, 2));
                        if (dist < WALL_LENGTH / 2.0f) {
                            cpSpaceRemoveShape(space, walls[i].shape);
                            cpSpaceRemoveBody(space, walls[i].body);
                            cpShapeFree(walls[i].shape);
                            cpBodyFree(walls[i].body);
                            walls[i].active = false;
                            // 壁の近くのstaticデブリを起こす
                            wakeUpStaticDebrisNearPoint((float)pos.x, (float)pos.y, WALL_LENGTH);
                            wakeUpStaticDebris();
                            deleted = true;
                        }
                    }
                    // Transferをチェック
                    for (int i = 0; i < transferCount && !deleted; i++) {
                        if (!transfers[i].active) continue;
                        float dist = sqrtf(powf(worldX - transfers[i].x, 2) + powf(worldY - transfers[i].y, 2));
                        if (dist < TRANSFER_SIZE / 2.0f) {
                            transfers[i].active = false;
                            deleted = true;
                        }
                    }
                }
            }
        } else if (actionMode == 9) {
            // モード9: 壁配置
            if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                placeWall(worldX, worldY);
            }
        } else if (actionMode == 10) {
            // モード10: Transfer配置
            if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                placeTransfer(worldX, worldY);
            }
        }
        } // if (!showModeMenu)

        // Input箱が空なら土砂を生成
        checkAndFillInputs();

        updateSieveVibration((float)GetTime());

        double startTime = GetTime();
        cpHastySpaceStep(space, 1.0f / 60.0f);
        double endTime = GetTime();
        physicsTimeMs = (float)((endTime - startTime) * 1000.0);

        cleanupDebris();
        cleanupWaterParticles();
        staticizeDebris(dt);
        wakeUpStaticDebris();
        processBreakingDebris();
        processPendingSticky();
        updateWaterParticles();
        updateTransfers();  // Transfer内のオブジェクトを反対側に転送
        // 10フレームごとにマージ/分割処理
        static int waterFrameCount = 0;
        if (++waterFrameCount >= 10) {
            waterFrameCount = 0;
            mergeWaterParticles();
            splitWaterParticles();
        }
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

        drawGrid(cameraX, cameraY, zoom);
        drawGround();
        drawSieve();
        drawUserSieves();

        for (int i = 0; i < debrisCount; i++) {
            drawDebris(i);
        }
        drawStickyJoints();
        drawWaterParticles();  // デブリより前面に描画
        drawConveyors();
        drawWalls();
        drawOutputs();
        drawInputs();
        drawTransfers();  // 両面で表示

        // 配置プレビュー（ワイヤフレーム）
        float previewX = snapToGrid(worldX);
        float previewY = snapToGrid(worldY);
        Color previewColor = (Color){100, 200, 100, 180};

        if (actionMode == 4) {
            // コンベア プレビュー（回転を考慮）
            float angle = lastConveyorAngle;
            float hw = CONVEYOR_LENGTH / 2.0f;
            float hh = CONVEYOR_THICKNESS / 2.0f;
            float cosA = cosf(angle), sinA = sinf(angle);
            Vector2 corners[4] = {
                {previewX + (-hw)*cosA - (-hh)*sinA, previewY + (-hw)*sinA + (-hh)*cosA},
                {previewX + ( hw)*cosA - (-hh)*sinA, previewY + ( hw)*sinA + (-hh)*cosA},
                {previewX + ( hw)*cosA - ( hh)*sinA, previewY + ( hw)*sinA + ( hh)*cosA},
                {previewX + (-hw)*cosA - ( hh)*sinA, previewY + (-hw)*sinA + ( hh)*cosA}
            };
            for (int i = 0; i < 4; i++) {
                DrawLineEx(corners[i], corners[(i+1)%4], 2, previewColor);
            }
        } else if (actionMode == 5) {
            // Output プレビュー
            float ox = previewX - OUTPUT_SIZE / 2.0f;
            float oy = previewY - OUTPUT_SIZE / 2.0f;
            DrawRectangleLines((int)ox, (int)oy, (int)OUTPUT_SIZE, (int)OUTPUT_SIZE, previewColor);
        } else if (actionMode == 6) {
            // ふるい プレビュー
            float angle = lastSieveAngle;
            float hw = SIEVE_LENGTH / 2.0f;
            float hh = SIEVE_THICKNESS / 2.0f;
            float cosA = cosf(angle), sinA = sinf(angle);
            Vector2 corners[4] = {
                {previewX + (-hw)*cosA - (-hh)*sinA, previewY + (-hw)*sinA + (-hh)*cosA},
                {previewX + ( hw)*cosA - (-hh)*sinA, previewY + ( hw)*sinA + (-hh)*cosA},
                {previewX + ( hw)*cosA - ( hh)*sinA, previewY + ( hw)*sinA + ( hh)*cosA},
                {previewX + (-hw)*cosA - ( hh)*sinA, previewY + (-hw)*sinA + ( hh)*cosA}
            };
            for (int i = 0; i < 4; i++) {
                DrawLineEx(corners[i], corners[(i+1)%4], 2, previewColor);
            }
        } else if (actionMode == 7) {
            // Input プレビュー
            float ix = previewX - INPUT_SIZE / 2.0f;
            float iy = previewY - INPUT_SIZE / 2.0f;
            DrawRectangleLines((int)ix, (int)iy, (int)INPUT_SIZE, (int)INPUT_SIZE, previewColor);
        } else if (actionMode == 9) {
            // 壁 プレビュー
            float angle = lastWallAngle;
            float hw = WALL_LENGTH / 2.0f;
            float hh = WALL_THICKNESS / 2.0f;
            float cosA = cosf(angle), sinA = sinf(angle);
            Vector2 corners[4] = {
                {previewX + (-hw)*cosA - (-hh)*sinA, previewY + (-hw)*sinA + (-hh)*cosA},
                {previewX + ( hw)*cosA - (-hh)*sinA, previewY + ( hw)*sinA + (-hh)*cosA},
                {previewX + ( hw)*cosA - ( hh)*sinA, previewY + ( hw)*sinA + ( hh)*cosA},
                {previewX + (-hw)*cosA - ( hh)*sinA, previewY + (-hw)*sinA + ( hh)*cosA}
            };
            for (int i = 0; i < 4; i++) {
                DrawLineEx(corners[i], corners[(i+1)%4], 2, previewColor);
            }
        } else if (actionMode == 10) {
            // Transfer プレビュー
            float halfSize = TRANSFER_SIZE / 2.0f;
            DrawRectangleLines((int)(previewX - halfSize), (int)(previewY - halfSize),
                (int)TRANSFER_SIZE, (int)TRANSFER_SIZE, previewColor);
        }

        EndMode2D();

        int activeCount = 0;
        for (int i = 0; i < debrisCount; i++) {
            if (debris[i].active) activeCount++;
        }

        int activeWaterCount = 0;
        int maxValenceCount = 0;
        int totalWaterVolume = 0;
        for (int i = 0; i < waterParticleCount; i++) {
            if (waterParticles[i].active) {
                activeWaterCount++;
                totalWaterVolume += waterParticles[i].valence;
                if (waterParticles[i].valence >= 16) maxValenceCount++;
            }
        }

        DrawRectangle(5, 5, 220, 155, (Color){255, 255, 255, 200});
        DrawFPS(10, 10);
        DrawText(TextFormat("Debris: %d", activeCount), 10, 35, 20, BLACK);
        DrawText(TextFormat("Water: %d (vol:%d)", activeWaterCount, totalWaterVolume), 10, 55, 20, BLUE);
        DrawText(TextFormat("Physics: %.2f ms", physicsTimeMs), 10, 75, 20, BLACK);
        DrawText(TextFormat("Mouse: %.0f, %.0f", worldX, worldY), 10, 95, 20, BLACK);
        DrawText(TextFormat("Mode[E]: %s", actionModeNames[actionMode]), 10, 115, 20, DARKBLUE);
        DrawText(TextFormat("Zoom: %.1fx", zoom), 10, 135, 20, BLACK);
        DrawText(showBackSide ? "Side: Back [F]" : "Side: Front [F]", 10, 155, 20, showBackSide ? ORANGE : DARKGREEN);

        // キー操作一覧（右端）
        int helpX = SCREEN_WIDTH - 180;
        int helpY = 5;
        DrawRectangle(helpX - 5, helpY, 180, 150, (Color){255, 255, 255, 200});
        DrawText("-- Controls --", helpX, helpY + 5, 16, DARKGRAY);
        DrawText("E: Mode menu", helpX, helpY + 25, 14, BLACK);
        DrawText("F: Front/Back", helpX, helpY + 45, 14, BLACK);
        DrawText("A/D: Scroll L/R", helpX, helpY + 65, 14, BLACK);
        DrawText("W/S: Scroll U/D", helpX, helpY + 85, 14, BLACK);
        DrawText("Wheel: Zoom", helpX, helpY + 105, 14, BLACK);
        DrawText("L-Drag: Pan", helpX, helpY + 125, 14, BLACK);

        // モード選択メニュー
        if (showModeMenu) {
            drawModeMenu();
        }

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
