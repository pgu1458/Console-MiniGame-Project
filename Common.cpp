#define _CRT_SECURE_NO_WARNINGS
#include "Stage3_Stair.h"
#include "Common.h"

char   buf[HEIGHT][WIDTH + 1];
WORD   colorBuf[HEIGHT][WIDTH];
HANDLE hOut = nullptr;

HANDLE hDBuffer[2] = { nullptr, nullptr };
int    nBufferIdx = 0;

void hideCursor() {
    CONSOLE_CURSOR_INFO ci = { 1, FALSE };
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ci);
}

void gotoxy(int x, int y) {
    COORD pos = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void setConsoleSize(int cols, int lines) {
    // 버퍼 먼저 크게 → 창 줄이기 → 버퍼 맞추기 순으로 해야 오류 없음
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD  bigBuf = { (SHORT)(cols + 1), (SHORT)(lines + 1) };
    SetConsoleScreenBufferSize(h, bigBuf);
    SMALL_RECT win = { 0, 0, (SHORT)(cols - 1), (SHORT)(lines - 1) };
    SetConsoleWindowInfo(h, TRUE, &win);
    COORD  fitBuf = { (SHORT)cols, (SHORT)lines };
    SetConsoleScreenBufferSize(h, fitBuf);
}

// ── 단순 버퍼 렌더링 ──────────────────────────
void clearBuffer() {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            buf[y][x] = ' ';
            colorBuf[y][x] = 0x08;
        }
        buf[y][WIDTH] = '\0';
    }
}

void drawChar(int x, int y, char ch, WORD color) {
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return;
    buf[y][x] = ch;
    colorBuf[y][x] = color;
}

void drawString(int x, int y, const std::string& s, WORD color) {
    for (int i = 0; i < (int)s.size(); i++)
        drawChar(x + i, y, s[i], color);
}

void render() {
    gotoxy(0, 0);
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            SetConsoleTextAttribute(hOut, colorBuf[y][x]);
            std::cout << buf[y][x];
        }
        std::cout << '\n';
    }
    SetConsoleTextAttribute(hOut, 0x0F);
}

// ── 더블 버퍼 (계단 게임) ────────────────────
void initDoubleBuffer() {
    const WORD C_DEF = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    for (int i = 0; i < 2; i++) {
        hDBuffer[i] = CreateConsoleScreenBuffer(
            GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
        CONSOLE_CURSOR_INFO ci = { 1, FALSE };
        SetConsoleCursorInfo(hDBuffer[i], &ci);
    }
}

void printToBuffer(int x, int y, const char* str, WORD color) {
    if (x < 0 || x >= 50 || y < 0 || y >= 25) return;
    DWORD dw;
    COORD pos = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(hDBuffer[nBufferIdx], pos);
    SetConsoleTextAttribute(hDBuffer[nBufferIdx], color);
    WriteConsoleA(hDBuffer[nBufferIdx], str, (DWORD)strlen(str), &dw, NULL);
}

void flipBuffer() {
    SetConsoleActiveScreenBuffer(hDBuffer[nBufferIdx]);
    nBufferIdx = !nBufferIdx;
    COORD pos = { 0, 0 };
    DWORD dw;
    const WORD C_DEF = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    FillConsoleOutputCharacterA(hDBuffer[nBufferIdx], ' ', 50 * 25, pos, &dw);
    FillConsoleOutputAttribute(hDBuffer[nBufferIdx], C_DEF, 50 * 25, pos, &dw);
}
