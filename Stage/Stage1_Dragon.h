#pragma once
#include "Common.h"

// ── 오브젝트 구조체 ───────────────────────────

struct Player {
    float x = WIDTH / 2.0f;
    float y = HEIGHT - 3.0f;
    int   hp = 5;
    int   score = 0;
    int   bulletLevel = 1;
    void  draw();
};

struct Bullet {
    float x, y;
    bool  alive = true;
    void  update();
    void  draw();
};

struct Enemy {
    float x, y;
    bool  alive = true;
    float speed;
    int   type;
    int   hp, maxHp;
    Enemy(float _x, float _y, int _type);
    void update();
    void draw();
};

struct BossBullet {
    float x, y, dx, dy;
    bool  alive = true;
    void  update();
    void  draw();
};

struct Boss {
    float x = WIDTH / 2.0f;
    float y = 3.0f;
    int   hp = 300;
    bool  alive = true;
    int   shootTimer = 0;
    float moveDir = 0.5f;
    void  update(std::vector<BossBullet>& bBullets);
    void  draw();
};

struct Item {
    float x, y;
    bool  alive = true;
    void  update();
    void  draw();
};

// ── 스테이지 1 실행 함수 ──────────────────────
// 반환값: true = 보스 클리어(다음 스테이지), false = 게임오버
bool runStage1();
