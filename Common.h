#pragma once
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <conio.h>

// ── 화면 크기 (드래곤 게임 기준) ──────────────
const int WIDTH = 40;
const int HEIGHT = 27;

// ── 콘솔 유틸 ─────────────────────────────────
void hideCursor();
void gotoxy(int x, int y);
void setConsoleSize(int cols, int lines);

// ── 버퍼 렌더링 (드래곤 / 화살 게임 공용) ─────
extern char buf[HEIGHT][WIDTH + 1];
extern WORD colorBuf[HEIGHT][WIDTH];
extern HANDLE hOut;

void clearBuffer();
void drawChar(int x, int y, char ch, WORD color = 0x0F);
void drawString(int x, int y, const std::string& s, WORD color = 0x0F);
void render();

// ── 더블버퍼 (계단 게임 전용) ─────────────────
extern HANDLE hDBuffer[2];
extern int    nBufferIdx;

void   initDoubleBuffer();
void   printToBuffer(int x, int y, const char* str, WORD color);
void   flipBuffer();
