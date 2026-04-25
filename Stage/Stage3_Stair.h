#pragma once
#include "Common.h"

struct StairPoint { int x, y; };

// ── 계단 게임 내부 함수 ───────────────────────
void   drawBorderStair();
void   drawCharacterStair(int x, int y, bool isJumping);
int    getValidNextX(int currentX);
void   drawGame3(int score, int stage,
    const std::vector<StairPoint>& stairs,
    int charX, int charY, bool isJumping);

// ── 스테이지 3 실행 함수 ──────────────────────
// 반환값: true = 클리어, false = 게임오버(종료)
bool runStage3();
