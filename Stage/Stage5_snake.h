#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <iostream>
#include <deque>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <thread>
#include <conio.h>
#include <algorithm>
#include <cmath>

// ── 스테이지 5 실행 함수 ──────────────────────
// 반환값: true = 200점 클리어, false = 포기/종료
bool runStage5();
