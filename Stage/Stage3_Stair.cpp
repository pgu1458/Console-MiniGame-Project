#define _CRT_SECURE_NO_WARNINGS
#include "Stage3_Stair.h"
#include <cstdio>

// 계단 게임 전용 상수
static const int SW = 50;
static const int SH = 25;

static const WORD C_HAIR = 0;
static const WORD C_SKIN = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY;
static const WORD C_WHITE = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY;
static const WORD C_BROWN = BACKGROUND_RED | BACKGROUND_GREEN;
static const WORD C_DEF = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;

// ── 테두리 ────────────────────────────────────
void drawBorderStair() {
    for (int i = 0; i < SW; i += 2) {
        printToBuffer(i, 0, "=", C_DEF);
        printToBuffer(i, SH - 1, "=", C_DEF);
    }
    for (int i = 1; i < SH - 1; i++) {
        printToBuffer(0, i, "|", C_DEF);
        printToBuffer(SW - 2, i, "|", C_DEF);
    }
}

// ── 캐릭터 ────────────────────────────────────
void drawCharacterStair(int x, int y, bool isJumping) {
    DWORD dw;
    // 얼굴
    printToBuffer(x + 1, y - 2, " ", C_SKIN);
    printToBuffer(x + 2, y - 2, " ", C_SKIN);
    COORD eyeL = { (SHORT)(x + 1), (SHORT)(y - 2) };
    COORD eyeR = { (SHORT)(x + 2), (SHORT)(y - 2) };
    FillConsoleOutputCharacterA(hDBuffer[nBufferIdx], '.', 1, eyeL, &dw);
    FillConsoleOutputCharacterA(hDBuffer[nBufferIdx], '.', 1, eyeR, &dw);
    // 몸통
    printToBuffer(x + 1, y - 1, " ", C_WHITE);
    printToBuffer(x + 2, y - 1, " ", C_WHITE);
    printToBuffer(x, y - 1, " ", C_SKIN);
    printToBuffer(x + 3, y - 1, " ", C_SKIN);
    // 다리
    printToBuffer(x + 1, y, " ", C_BROWN);
    printToBuffer(x + 2, y, " ", C_BROWN);
}

// ── 다음 계단 X 계산 ──────────────────────────
int getValidNextX(int currentX) {
    int nextX = currentX + (rand() % 2 == 1 ? 5 : -5);
    if (nextX < 2)  return currentX + 5;
    if (nextX > 40) return currentX - 5;
    return nextX;
}

// ── 게임 화면 그리기 ──────────────────────────
void drawGame3(int score, int stage,
    const std::vector<StairPoint>& stairs,
    int charX, int charY, bool isJumping)
{
    drawBorderStair();

    // 구분선
    for (int i = 1; i < SW - 1; i++) printToBuffer(i, 4, "-", C_DEF);

    char scoreBuf[64];
    sprintf(scoreBuf, "Stage: %d | Score: %d / 100", stage, score);
    printToBuffer(3, 2, scoreBuf, C_DEF);

    int cx = charX + 1;
    int cy = charY;

    // 계단 그리기
    for (const auto& s : stairs) {
        if (s.y <= 4 || s.y >= SH - 1 || s.x <= 0 || s.x >= SW - 2) continue;
        for (int i = 0; i < 5; i++) {
            int sx = s.x + i, sy = s.y;
            // 캐릭터 몸통/다리 위치 겹침 제외
            bool overlap = false;
            if (sy == cy && sx >= cx + 1 && sx <= cx + 2) overlap = true;
            if (sy == cy - 1 && sx >= cx && sx <= cx + 3) overlap = true;
            if (overlap) continue;

            if (i == 0) printToBuffer(sx, sy, "[", C_DEF);
            else if (i == 4) printToBuffer(sx, sy, "]", C_DEF);
            else             printToBuffer(sx, sy, "-", C_DEF);
        }
    }

    drawCharacterStair(cx, cy, isJumping);
    printToBuffer(3, SH - 2, "A: Left | S: Right | Q: Quit", C_DEF);
}

// ─────────────────────────────────────────────
//  스테이지 3 메인 루프
// ─────────────────────────────────────────────
bool runStage3() {
    setConsoleSize(SW + 1, SH + 1);
    initDoubleBuffer();
    system("cls");

    // 시작 안내 (더블버퍼 활성화 전 기본 콘솔에 출력)
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleActiveScreenBuffer(h);
    SetConsoleTextAttribute(h, 0x0D);
    gotoxy(12, SH / 2 - 1); std::cout << "=== STAGE 3: INFINITY STAIRS ===";
    SetConsoleTextAttribute(h, 0x0F);
    gotoxy(14, SH / 2 + 1); std::cout << "100점을 모으면 최종 클리어!";
    gotoxy(18, SH / 2 + 3); std::cout << "아무 키나 눌러 시작";
    _getch();

    bool cleared = false;

    // 외부 루프: 재시작 처리
    while (true) {
        int score = 0, stage = 1;
        int charX = 20, charY = 19;

        std::vector<StairPoint> stairs;
        stairs.push_back({ charX + 1, charY + 1 });
        for (int i = 0; i < 15; i++)
            stairs.push_back({ getValidNextX(stairs.back().x), stairs.back().y - 2 });

        bool alive = true;

        while (alive) {
            drawGame3(score, stage, stairs, charX, charY, false);
            flipBuffer();

            if (score >= 100) {
                // 클리어!
                printToBuffer(18, SH / 2, "=== FINAL CLEAR! ===", FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                flipBuffer();
                Sleep(2000);
                cleared = true;
                alive = false;
                break;
            }

            if (_kbhit()) {
                char key = _getch();
                if (key == 'q' || key == 'Q') { alive = false; break; }
                if (key == '\t') { cleared = true; alive = false; break; }

                int inputDir = (key == 'a' || key == 'A') ? 0
                    : (key == 's' || key == 'S') ? 1 : -1;

                if (inputDir != -1) {
                    bool correctDir = (inputDir == (stairs[1].x > stairs[0].x ? 1 : 0));
                    if (correctDir) {
                        score++;
                        int diffX = (inputDir == 1 ? 5 : -5);
                        // 점프 연출
                        drawGame3(score, stage, stairs, charX + diffX / 2, charY - 1, true);
                        flipBuffer();
                        Sleep(60);
                        // 계단 갱신
                        stairs.erase(stairs.begin());
                        for (auto& s : stairs) { s.y += 2; s.x -= diffX; }
                        stairs.push_back({ getValidNextX(stairs.back().x), stairs.back().y - 2 });
                    }
                    else {
                        alive = false;
                    }
                }
            }
            Sleep(10);
        }

        if (cleared) break;

        // 게임오버 화면
        printToBuffer(15, 8, " G A M E   O V E R ", FOREGROUND_RED | FOREGROUND_INTENSITY);
        char fs[32]; sprintf(fs, "Final Score: %d", score);
        printToBuffer(17, 10, fs, C_DEF);
        printToBuffer(13, 14, "[ R ] : REPLAY", FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        printToBuffer(13, 16, "[ Q ] : QUIT", FOREGROUND_RED | FOREGROUND_INTENSITY);
        flipBuffer();

        bool restart = false;
        while (true) {
            if (_kbhit()) {
                char c = _getch();
                if (c == 'r' || c == 'R') { restart = true;  break; }
                if (c == 'q' || c == 'Q') { restart = false; break; }
            }
            Sleep(100);
        }
        if (!restart) break;
    }

    // 더블버퍼 → 기본 콘솔로 복귀
    SetConsoleActiveScreenBuffer(GetStdHandle(STD_OUTPUT_HANDLE));
    system("cls");

    if (cleared) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0E);
        gotoxy(14, SH / 2 - 1); std::cout << "=== ALL STAGES CLEAR! ===";
        gotoxy(12, SH / 2 + 1);
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0F);
        std::cout << "축하합니다! 모든 스테이지를 클리어했습니다!";
        gotoxy(18, SH / 2 + 3); std::cout << "아무 키나 눌러 종료";
        _getch();
        return true;
    }
    return false;
}
