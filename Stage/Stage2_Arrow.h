#pragma once
#include "Common.h"

enum Direction { LEFT_TO_RIGHT, RIGHT_TO_LEFT, TOP_TO_BOTTOM, BOTTOM_TO_TOP };

// ── 화살 오브젝트 ─────────────────────────────
class Arrow {
public:
    int       x, y, speed;
    Direction dir;
    int       screenWidth, screenHeight;

    Arrow(int width, int height);
    void move();
    void draw();
    bool isOutOfScreen();
};

// ── 스테이지 2 실행 함수 ──────────────────────
// 반환값: true = 15초 생존 클리어, false = 게임오버
bool runStage2();
