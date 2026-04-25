#include "Common.h"
#include "Stage1_Dragon.h"
#include "Stage2_Arrow.h"
#include "Stage3_Stair.h"
#include "Stage4_Tetris.h"
#include "Stage5_Snake.h"

static void showTitle() {
    system("cls");
    setConsoleSize(44, 26);
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(h, 0x0E);
    gotoxy(3, 2); std::cout << "╔══════════════════════════════════╗";
    gotoxy(3, 3); std::cout << "║        PENTA STAGE GAME          ║";
    gotoxy(3, 4); std::cout << "╚══════════════════════════════════╝";
    SetConsoleTextAttribute(h, 0x0B);
    gotoxy(5, 6); std::cout << "STAGE 1: Dragon Flight";
    gotoxy(8, 7); std::cout << "- Defeat the boss (300HP)!";
    gotoxy(5, 9); std::cout << "STAGE 2: Arrow Dodge";
    gotoxy(8, 10); std::cout << "- Survive 15 seconds!";
    gotoxy(5, 12); std::cout << "STAGE 3: Infinity Stairs";
    gotoxy(8, 13); std::cout << "- Collect 100 points!";
    gotoxy(5, 15); std::cout << "STAGE 4: Tetris Space Edition";
    gotoxy(8, 16); std::cout << "- Score 700 points!";
    gotoxy(5, 18); std::cout << "STAGE 5: Snake Hard Mode";
    gotoxy(8, 19); std::cout << "- Score 200 points!";
    SetConsoleTextAttribute(h, 0x0F);
    gotoxy(12, 23); std::cout << "Press any key to start";
    _getch();
}

static void showGameOver() {
    system("cls");
    setConsoleSize(42, 18);
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(h, 0x0C);
    gotoxy(10, 6);  std::cout << "╔══════════════════╗";
    gotoxy(10, 7);  std::cout << "║   GAME  OVER     ║";
    gotoxy(10, 8);  std::cout << "╚══════════════════╝";
    SetConsoleTextAttribute(h, 0x0F);
    gotoxy(10, 12); std::cout << "R: Retry  /  Q: Quit";
    while (true) {
        if (GetAsyncKeyState('R') & 0x8000) return;
        if (GetAsyncKeyState('Q') & 0x8000) exit(0);
        Sleep(100);
    }
}

static void showAllClear() {
    system("cls");
    setConsoleSize(44, 20);
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(h, 0x0A);
    gotoxy(5, 4); std::cout << "╔══════════════════════════════════╗";
    gotoxy(5, 5); std::cout << "║          ALL STAGES CLEAR!       ║";
    gotoxy(5, 6); std::cout << "╚══════════════════════════════════╝";
    SetConsoleTextAttribute(h, 0x0E);
    gotoxy(8, 10); std::cout << "Congratulations!";
    gotoxy(8, 11); std::cout << "All 5 stages cleared!";
    SetConsoleTextAttribute(h, 0x0F);
    gotoxy(10, 15); std::cout << "R: Retry  /  Q: Quit";
    while (true) {
        if (GetAsyncKeyState('R') & 0x8000) return;
        if (GetAsyncKeyState('Q') & 0x8000) exit(0);
        Sleep(100);
    }
}

int main() {
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    hideCursor();
    srand((unsigned)time(nullptr));
    while (true) {
        showTitle();
        if (!runStage1()) { showGameOver(); continue; }
        if (!runStage2()) { showGameOver(); continue; }
        if (!runStage3()) { showGameOver(); continue; }
        if (!runStage4()) { showGameOver(); continue; }
        if (!runStage5()) { showGameOver(); continue; }
        showAllClear();
    }
    return 0;
}