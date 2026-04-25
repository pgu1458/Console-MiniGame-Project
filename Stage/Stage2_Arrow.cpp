#include "Stage2_Arrow.h"

// ── Arrow ─────────────────────────────────────
Arrow::Arrow(int width, int height) {
    screenWidth = width;
    screenHeight = height;
    speed = 1;

    int r = rand() % 10;
    if (r < 4) dir = LEFT_TO_RIGHT;
    else if (r < 8) dir = RIGHT_TO_LEFT;
    else if (r == 8) dir = TOP_TO_BOTTOM;
    else             dir = BOTTOM_TO_TOP;

    switch (dir) {
    case LEFT_TO_RIGHT: x = 1;           y = (rand() % (height - 2)) + 1; break;
    case RIGHT_TO_LEFT: x = width - 3;   y = (rand() % (height - 2)) + 1; break;
    case TOP_TO_BOTTOM: x = (rand() % (width - 3)) + 1; y = 1;            break;
    case BOTTOM_TO_TOP: x = (rand() % (width - 3)) + 1; y = height - 2;   break;
    }
}

void Arrow::move() {
    switch (dir) {
    case LEFT_TO_RIGHT: x += speed; break;
    case RIGHT_TO_LEFT: x -= speed; break;
    case TOP_TO_BOTTOM: y += speed; break;
    case BOTTOM_TO_TOP: y -= speed; break;
    }
}

void Arrow::draw() {
    if (isOutOfScreen()) return;
    // 화살 피하기 게임은 직접 콘솔에 출력 (버퍼 방식 미사용)
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    if (dir == LEFT_TO_RIGHT || dir == RIGHT_TO_LEFT) {
        SetConsoleTextAttribute(h, 11);
        gotoxy(x, y);
        std::cout << (dir == LEFT_TO_RIGHT ? "->" : "<-");
    }
    else {
        SetConsoleTextAttribute(h, 14);
        if (dir == TOP_TO_BOTTOM) {
            if (y > 1) { gotoxy(x, y - 1); std::cout << "|"; }
            gotoxy(x, y); std::cout << "v";
        }
        else {
            if (y < screenHeight - 2) { gotoxy(x, y + 1); std::cout << "|"; }
            gotoxy(x, y); std::cout << "^";
        }
    }
    SetConsoleTextAttribute(h, 0x0F);
}

bool Arrow::isOutOfScreen() {
    return (x <= 0 || x >= screenWidth - 2 || y <= 0 || y >= screenHeight - 1);
}

// ── 내부 헬퍼 ────────────────────────────────
static void drawWall2(int width, int height) {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(h, 15);
    for (int i = 0; i < width; i++) { gotoxy(i, 0);          std::cout << "#"; }
    for (int i = 0; i < width; i++) { gotoxy(i, height - 1); std::cout << "#"; }
    for (int i = 1; i < height - 1; i++) { gotoxy(0, i);          std::cout << "#"; }
    for (int i = 1; i < height - 1; i++) { gotoxy(width - 1, i);  std::cout << "#"; }
    SetConsoleTextAttribute(h, 0x0F);
}

static void drawPlayer2(int x, int y, bool erase = false) {
    gotoxy(x, y);
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(h, 0x0F);
    std::cout << (erase ? "  " : "\xF0\x9F\x90\xB1"); // 🐱 UTF-8
    SetConsoleTextAttribute(h, 0x0F);
}
// ─────────────────────────────────────────────
//  스테이지 2 메인 루프
// ─────────────────────────────────────────────
bool runStage2() {
    system("chcp 65001 > nul");
    const int W = 80, H = 25;
    setConsoleSize(W + 1, H + 2);
    system("cls");

    // 시작 안내
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(h, 0x0B);
    gotoxy(28, H / 2 - 1); std::cout << "=== STAGE 2: ARROW DODGE ===";
    SetConsoleTextAttribute(h, 0x0F);
    gotoxy(30, H / 2 + 1); std::cout << "15초 동안 화살을 피하세요!";
    gotoxy(34, H / 2 + 3); std::cout << "아무 키나 눌러 시작";
    _getch();

    // 카운트다운
    for (int i = 3; i > 0; i--) {
        system("cls");
        gotoxy(38, H / 2);
        SetConsoleTextAttribute(h, 0x0E);
        std::cout << "READY... " << i;
        Sleep(1000);
    }
    system("cls");

    drawWall2(W, H);

    double px = W / 2.0, py = H / 2.0;
    double moveSpeed = 1.5;
    int    oldX = (int)px, oldY = (int)py;

    std::vector<Arrow> arrows;
    time_t startTime = time(nullptr);
    bool   cleared = false;

    while (true) {
        time_t now = time(nullptr);
        int    elapsed = (int)difftime(now, startTime);

        if (elapsed >= 15) { cleared = true; break; }

        // 입력
        if (GetAsyncKeyState(VK_UP) & 0x8000 && py > 1.2)          py -= moveSpeed * 0.6;
        if (GetAsyncKeyState(VK_DOWN) & 0x8000 && py < H - 2.2)      py += moveSpeed * 0.6;
        if (GetAsyncKeyState(VK_LEFT) & 0x8000 && px > 1.2)          px -= moveSpeed;
        if (GetAsyncKeyState(VK_RIGHT) & 0x8000 && px < W - 3.2)      px += moveSpeed;
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) break;
        if (GetAsyncKeyState(VK_TAB) & 0x8000) { cleared = true; break; }

        // 플레이어 이동 처리
        if (oldX != (int)px || oldY != (int)py) {
            drawPlayer2(oldX, oldY, true);
            oldX = (int)px; oldY = (int)py;
        }
        drawPlayer2(oldX, oldY);

        // 화살 스폰
        if (rand() % 2 == 0) arrows.push_back(Arrow(W, H));

        // 화살 업데이트
        bool dead = false;
        for (int i = 0; i < (int)arrows.size(); i++) {
            // 이전 위치 지우기
            gotoxy(arrows[i].x, arrows[i].y); std::cout << "  ";
            if (arrows[i].dir == TOP_TO_BOTTOM && arrows[i].y > 1)
            {
                gotoxy(arrows[i].x, arrows[i].y - 1); std::cout << " ";
            }
            else if (arrows[i].dir == BOTTOM_TO_TOP && arrows[i].y < H - 2)
            {
                gotoxy(arrows[i].x, arrows[i].y + 1); std::cout << " ";
            }

            arrows[i].move();

            // 충돌
            if (abs(arrows[i].x - oldX) <= 1 && arrows[i].y == oldY) {
                dead = true; break;
            }

            if (arrows[i].isOutOfScreen()) {
                arrows.erase(arrows.begin() + i); i--; continue;
            }
            arrows[i].draw();
        }

        if (dead) { cleared = false; break; }

        // HUD
        gotoxy(2, 0);
        SetConsoleTextAttribute(h, 0x0F);
        std::cout << " TIME: " << elapsed << " / 15s   ";

        drawWall2(W, H); // 테두리 유지
        Sleep(30);
    }

    system("cls");
    if (cleared) {
        SetConsoleTextAttribute(h, 0x0A);
        gotoxy(30, H / 2 - 1); std::cout << "=== STAGE 2 CLEAR! ===";
        gotoxy(28, H / 2 + 1); std::cout << "다음 스테이지: 계단 올라가기!";
        gotoxy(34, H / 2 + 3);
        SetConsoleTextAttribute(h, 0x0F);
        std::cout << "아무 키나 눌러 계속";
        _getch();
        return true;
    }
    return false;
}
