#include "Common.h"
#include "Stage1_Dragon.h"
#include "Stage2_Arrow.h"
#include "Stage3_Stair.h"
#include "Stage4_Tetris.h"
#include "Stage5_Snake.h"

static void showTitle() {
    system("cls");
    setConsoleSize(52, 34);
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

    // 배경 별 장식
    SetConsoleTextAttribute(h, 0x08);
    gotoxy(2, 1); std::cout << "*       .       *       .    *     .      *";
    gotoxy(2, 2); std::cout << "   .        *       .      *       .    *  ";
    gotoxy(2, 30); std::cout << "   .        *       .      *       .    *  ";
    gotoxy(2, 31); std::cout << "*       .       *       .    *     .      *";

    // 메인 타이틀 박스
    SetConsoleTextAttribute(h, 0x0E);
    gotoxy(4, 4); std::cout << "+===========================================+";
    gotoxy(4, 5); std::cout << "|                                           |";
    gotoxy(4, 6); std::cout << "|        P E N T A  S T A G E               |";
    gotoxy(4, 7); std::cout << "|              G A M E                      |";
    gotoxy(4, 8); std::cout << "|                                           |";
    gotoxy(4, 9); std::cout << "+===========================================+";

    // 부제목
    SetConsoleTextAttribute(h, 0x07);
    gotoxy(13, 11); std::cout << "- 5 Stages  |  Clear All to Win -";

    // 구분선
    SetConsoleTextAttribute(h, 0x08);
    gotoxy(4, 13); std::cout << "-------------------------------------------";

    // Stage 1
    SetConsoleTextAttribute(h, 0x0C);
    gotoxy(6, 15); std::cout << "[ STAGE 1 ]";
    SetConsoleTextAttribute(h, 0x07);
    gotoxy(19, 15); std::cout << "Dragon Flight";
    SetConsoleTextAttribute(h, 0x08);
    gotoxy(19, 16); std::cout << "Defeat the boss (300HP)";

    // Stage 2
    SetConsoleTextAttribute(h, 0x0B);
    gotoxy(6, 18); std::cout << "[ STAGE 2 ]";
    SetConsoleTextAttribute(h, 0x07);
    gotoxy(19, 18); std::cout << "Arrow Dodge";
    SetConsoleTextAttribute(h, 0x08);
    gotoxy(19, 19); std::cout << "Survive 15 seconds";

    // Stage 3
    SetConsoleTextAttribute(h, 0x0A);
    gotoxy(6, 21); std::cout << "[ STAGE 3 ]";
    SetConsoleTextAttribute(h, 0x07);
    gotoxy(19, 21); std::cout << "Infinity Stairs";
    SetConsoleTextAttribute(h, 0x08);
    gotoxy(19, 22); std::cout << "Collect 100 points";

    // Stage 4
    SetConsoleTextAttribute(h, 0x0E);
    gotoxy(6, 24); std::cout << "[ STAGE 4 ]";
    SetConsoleTextAttribute(h, 0x07);
    gotoxy(19, 24); std::cout << "Tetris Space Edition";
    SetConsoleTextAttribute(h, 0x08);
    gotoxy(19, 25); std::cout << "Score 700 points";

    // Stage 5
    SetConsoleTextAttribute(h, 0x0D);
    gotoxy(6, 27); std::cout << "[ STAGE 5 ]";
    SetConsoleTextAttribute(h, 0x07);
    gotoxy(19, 27); std::cout << "Snake Hard Mode";
    SetConsoleTextAttribute(h, 0x08);
    gotoxy(19, 28); std::cout << "Score 200 points";

    // 구분선
    SetConsoleTextAttribute(h, 0x08);
    gotoxy(4, 30); std::cout << "-------------------------------------------";

    // 시작 안내
    SetConsoleTextAttribute(h, 0x0F);
    gotoxy(14, 32); std::cout << ">> Press any key to START <<";

    SetConsoleTextAttribute(h, 0x07);
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