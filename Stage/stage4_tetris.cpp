#include "Stage4_Tetris.h"
#include <conio.h>

template<class T> T clamp(T v, T lo, T hi) { return v < lo ? lo : (v > hi ? hi : v); }

// =====================================================================
//  버퍼 시스템 (테트리스 전용 - 80x30)
// =====================================================================
const int BW = 80, BH = 30;
static HANDLE   g_hOut4;
static CHAR_INFO g_buf4[BW * BH];

static void bSet(int x, int y, wchar_t ch, WORD attr) {
    if (x < 0 || x >= BW || y < 0 || y >= BH) return;
    g_buf4[y * BW + x].Char.UnicodeChar = ch;
    g_buf4[y * BW + x].Attributes = attr;
}
static bool isFullWidth(wchar_t c) {
    return (c >= 0xAC00 && c <= 0xD7A3) ||
        (c >= 0x3130 && c <= 0x318F) ||
        (c >= 0x1100 && c <= 0x11FF);
}
static int getStrWidth(const wstring& s) {
    int w = 0;
    for (wchar_t c : s) w += isFullWidth(c) ? 2 : 1;
    return w;
}
static void bStr(int x, int y, const wchar_t* s, WORD attr) {
    int cx = x;
    for (int i = 0; s[i]; i++) {
        if (isFullWidth(s[i])) {
            bSet(cx, y, s[i], attr | COMMON_LVB_LEADING_BYTE);
            bSet(cx + 1, y, s[i], attr | COMMON_LVB_TRAILING_BYTE);
            cx += 2;
        }
        else { bSet(cx, y, s[i], attr); cx += 1; }
    }
}
static void bStr(int x, int y, const wstring& s, WORD attr) {
    int cx = x;
    for (wchar_t c : s) {
        if (isFullWidth(c)) {
            bSet(cx, y, c, attr | COMMON_LVB_LEADING_BYTE);
            bSet(cx + 1, y, c, attr | COMMON_LVB_TRAILING_BYTE);
            cx += 2;
        }
        else { bSet(cx, y, c, attr); cx += 1; }
    }
}
static void bCenter(int y, const wstring& s, WORD attr) {
    int width = getStrWidth(s);
    int x = (BW - width) / 2;
    if (x < 0) x = 0;
    bStr(x, y, s, attr);
}
static void bFillRow(int y, wchar_t ch, WORD attr) {
    for (int x = 0; x < BW; x++) bSet(x, y, ch, attr);
}
static void bClear4(wchar_t ch = L' ', WORD attr = 0x0000) {
    for (int i = 0; i < BW * BH; i++) {
        g_buf4[i].Char.UnicodeChar = ch;
        g_buf4[i].Attributes = attr;
    }
}
static void bFlush4() {
    COORD sz = { (SHORT)BW, (SHORT)BH };
    COORD origin = { 0, 0 };
    SMALL_RECT rgn = { 0, 0, (SHORT)(BW - 1), (SHORT)(BH - 1) };
    WriteConsoleOutputW(g_hOut4, g_buf4, sz, origin, &rgn);
}
static void bBox4(int x, int y, int w, int h, WORD attr) {
    bSet(x, y, L'\u2554', attr);
    bSet(x + w - 1, y, L'\u2557', attr);
    bSet(x, y + h - 1, L'\u255A', attr);
    bSet(x + w - 1, y + h - 1, L'\u255D', attr);
    for (int i = 1; i < w - 1; i++) {
        bSet(x + i, y, L'\u2550', attr);
        bSet(x + i, y + h - 1, L'\u2550', attr);
    }
    for (int j = 1; j < h - 1; j++) {
        bSet(x, y + j, L'\u2551', attr);
        bSet(x + w - 1, y + j, L'\u2551', attr);
    }
}
static void bBoxThin4(int x, int y, int w, int h, WORD attr) {
    bSet(x, y, L'\u250C', attr);
    bSet(x + w - 1, y, L'\u2510', attr);
    bSet(x, y + h - 1, L'\u2514', attr);
    bSet(x + w - 1, y + h - 1, L'\u2518', attr);
    for (int i = 1; i < w - 1; i++) {
        bSet(x + i, y, L'\u2500', attr);
        bSet(x + i, y + h - 1, L'\u2500', attr);
    }
    for (int j = 1; j < h - 1; j++) {
        bSet(x, y + j, L'\u2502', attr);
        bSet(x + w - 1, y + j, L'\u2502', attr);
    }
}

// =====================================================================
//  입력 유틸
// =====================================================================
static bool keyHeld4(int vk) { return (GetAsyncKeyState(vk) & 0x8000) != 0; }
struct Edge4 {
    bool prev = false;
    bool once(int vk) {
        bool c = keyHeld4(vk), h = c && !prev; prev = c; return h;
    }
};
static void flushKeys4() { while (_kbhit()) _getch(); }
static void hideCursor4() {
    CONSOLE_CURSOR_INFO ci = { 1, FALSE };
    SetConsoleCursorInfo(g_hOut4, &ci);
}

// =====================================================================
//  파티클: 우주 별
// =====================================================================
struct StarP4 { float x, y, vx, vy; int col; wchar_t ch; };
static vector<StarP4> g_stars4;

static void initStars4() {
    g_stars4.clear();
    const wchar_t chs[] = { L'.', L'+', L'*', L'\u00B7', L'\u2022', L'.', L'.' };
    const int cols[] = { 0x000D,0x000B,0x000C,0x000A,0x000E,0x0009,0x0005 };
    srand(77);
    for (int i = 0; i < 80; i++) {
        StarP4 s;
        s.x = (float)(rand() % BW);
        s.y = (float)(rand() % BH);
        int tier = rand() % 3;
        s.vx = -(0.01f + tier * 0.025f + (rand() % 4) * 0.008f);
        s.vy = 0.0f;
        int t = (tier == 0) ? (rand() % 2 + 4) :
            (tier == 1) ? (rand() % 2 + 2) : (rand() % 2);
        s.ch = chs[t]; s.col = cols[t];
        g_stars4.push_back(s);
    }
    srand((unsigned)time(nullptr));
}
static void updateStars4() {
    for (auto& s : g_stars4) {
        s.x += s.vx;
        if (s.x < 0) { s.x = (float)(BW - 2); s.y = (float)(1 + rand() % (BH - 2)); }
        int ix = (int)s.x, iy = (int)s.y;
        if (ix >= 0 && ix < BW - 1 && iy >= 1 && iy < BH - 1)
            bSet(ix, iy, s.ch, (WORD)s.col);
    }
}

// =====================================================================
//  하이스코어
// =====================================================================
const char* SCORE_FILE4 = "tetris_highscore.dat";
static int loadHighScore4() {
    ifstream f(SCORE_FILE4); int s = 0;
    if (f.is_open()) f >> s; return s;
}
static void saveHighScore4(int score) {
    if (score > loadHighScore4()) {
        ofstream f(SCORE_FILE4);
        if (f.is_open()) f << score;
    }
}

// =====================================================================
//  게임 데이터
// =====================================================================
static const int PIECES[7][4][4][2] = {
    {{{0,1},{1,1},{2,1},{3,1}},{{2,0},{2,1},{2,2},{2,3}},{{0,2},{1,2},{2,2},{3,2}},{{1,0},{1,1},{1,2},{1,3}}},
    {{{1,0},{2,0},{1,1},{2,1}},{{1,0},{2,0},{1,1},{2,1}},{{1,0},{2,0},{1,1},{2,1}},{{1,0},{2,0},{1,1},{2,1}}},
    {{{1,0},{0,1},{1,1},{2,1}},{{1,0},{1,1},{2,1},{1,2}},{{0,1},{1,1},{2,1},{1,2}},{{1,0},{0,1},{1,1},{1,2}}},
    {{{1,0},{2,0},{0,1},{1,1}},{{1,0},{1,1},{2,1},{2,2}},{{1,1},{2,1},{0,2},{1,2}},{{0,0},{0,1},{1,1},{1,2}}},
    {{{0,0},{1,0},{1,1},{2,1}},{{2,0},{1,1},{2,1},{1,2}},{{0,1},{1,1},{1,2},{2,2}},{{1,0},{0,1},{1,1},{0,2}}},
    {{{0,0},{0,1},{1,1},{2,1}},{{1,0},{2,0},{1,1},{1,2}},{{0,1},{1,1},{2,1},{2,2}},{{1,0},{1,1},{0,2},{1,2}}},
    {{{2,0},{0,1},{1,1},{2,1}},{{1,0},{1,1},{1,2},{2,2}},{{0,1},{1,1},{2,1},{0,2}},{{0,0},{1,0},{1,1},{1,2}}},
};
static const WORD PIECE_ATTR[7] = {
    0x000B,0x0009,0x000E,0x000E,0x000A,0x000D,0x000C
};

// =====================================================================
//  테트리스 레이아웃 상수
// =====================================================================
const int TW = 10, TH = 20;
const int TBX = 29, TBY = 5;
const int TSX = TBX + TW * 2 + 3;
const int THX = TBX - 13;

static void tetDrawBorder4() {
    bBox4(TBX - 1, TBY - 1, TW * 2 + 2, TH + 2, 0x000D);
}

static void drawMiniPiece4(int bx, int by, int p, WORD attr) {
    for (int y = 0; y < 3; y++)
        for (int x = 0; x < 8; x++)
            bSet(bx + x, by + y, L' ', 0x0000);
    if (p < 0) return;
    for (int i = 0; i < 4; i++) {
        int nx = PIECES[p][0][i][0];
        int ny = PIECES[p][0][i][1];
        bSet(bx + nx * 2, by + ny, L'[', attr);
        bSet(bx + nx * 2 + 1, by + ny, L']', attr);
    }
}

static void tetDrawHUD4(int score, int lines, int nexP, int holdP, bool holdUsed, int holdLeft, int hiScore) {
    bFillRow(0, L'\u2580', 0x0050);
    bStr(1, 0, L"[ TETRIS : SPACE EDITION ]", 0x000D);
    bStr(50, 0, L"GOAL: 700pt", 0x000E);
    bFillRow(BH - 1, L'\u2584', 0x0050);
    bStr(2, BH - 1, L"ARROW:move  UP:rotate  SPC:drop  C:delete  ESC:title", 0x0008);

    // 점수
    bStr(TSX, TBY, L"SCORE", 0x000B);
    bBoxThin4(TSX, TBY + 1, 12, 3, 0x000B);
    bStr(TSX + 1, TBY + 2, to_wstring(score), 0x000E);

    // 하이스코어
    bStr(TSX, TBY + 5, L"BEST", 0x0008);
    bBoxThin4(TSX, TBY + 6, 12, 3, 0x0008);
    bStr(TSX + 1, TBY + 7, to_wstring(hiScore),
        score >= hiScore ? (WORD)0x000E : (WORD)0x0008);

    // 줄 수
    bStr(TSX, TBY + 10, L"LINES", 0x000A);
    bBoxThin4(TSX, TBY + 11, 12, 3, 0x000A);
    bStr(TSX + 1, TBY + 12, to_wstring(lines), 0x000A);

    // NEXT
    bStr(TSX, TBY + 15, L"NEXT", 0x000B);
    bBoxThin4(TSX, TBY + 16, 12, 5, 0x000B);
    drawMiniPiece4(TSX + 1, TBY + 17, nexP, PIECE_ATTR[nexP]);

    // HOLD (블록 삭제)
    WORD holdCol = (holdLeft > 0) ? (WORD)0x000D : (WORD)0x0008;
    bStr(THX, TBY, L"Delete", holdCol);
    bStr(THX + 7, TBY, L"(" + to_wstring(holdLeft) + L"/3)", holdLeft > 0 ? (WORD)0x000D : (WORD)0x0008);
    bBoxThin4(THX, TBY + 1, 12, 5, holdCol);
    if (holdP >= 0)
        drawMiniPiece4(THX + 1, TBY + 2, holdP, holdLeft > 0 ? PIECE_ATTR[holdP] : (WORD)0x0008);
    bStr(THX, TBY + 7, L"C : Delete", holdLeft > 0 ? (WORD)0x0005 : (WORD)0x0008);
    if (holdLeft == 0)
        bStr(THX, TBY + 8, L"USED UP!", 0x000C);
}

// =====================================================================
//  콘솔 크기 설정 (테트리스 전용)
// =====================================================================
static void setTetrisConsole() {
    SMALL_RECT wr = { 0, 0, (SHORT)(BW - 1), (SHORT)(BH - 1) };
    SetConsoleWindowInfo(g_hOut4, TRUE, &wr);
    COORD sz = { (SHORT)BW, (SHORT)(BH + 1) };
    SetConsoleScreenBufferSize(g_hOut4, sz);
    SetConsoleWindowInfo(g_hOut4, TRUE, &wr);
}

// =====================================================================
//  스테이지 4 메인 루프
// =====================================================================
bool runStage4() {
    g_hOut4 = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    setTetrisConsole();
    hideCursor4();
    initStars4();

    // 시작 안내
    bClear4();
    bCenter(4, L"=== STAGE 4: TETRIS SPACE EDITION ===", 0x000E);
    bCenter(7, L"블록을 쌓아 줄을 지워 700점을 달성하세요!", 0x000B);
    bCenter(9, L"이동: 방향키   회전: UP   하드드롭: SPACE", 0x0007);
    bCenter(10, L"블록삭제: C (3회)   ESC: 타이틀", 0x0007);
    bCenter(13, L">> ENTER를 누르면 시작합니다 <<", 0x000F);
    bFlush4();
    flushKeys4();
    while (!keyHeld4(VK_RETURN)) {
        if (keyHeld4(VK_ESCAPE)) return false;
        Sleep(20);
    }
    Sleep(100);

RESTART4:
    int board[TH][TW] = {};
    int curP = rand() % 7, curR = 0, curX = 3, curY = 0;
    int nexP = rand() % 7;
    int holdP = -1;
    bool holdUsed = false;
    int  holdLeft = 3;
    int  score = 0, lines = 0, level = 1;
    bool gameOver = false;
    int  hiScore = loadHighScore4();
    int  dropMs = 500;

    DWORD lastDrop = GetTickCount();
    DWORD lastMoveX = 0, lastMoveY = 0;
    const DWORD dX = 100, dY = 50;

    auto valid = [&](int p, int r, int x, int y)->bool {
        for (int i = 0; i < 4; i++) {
            int nx = x + PIECES[p][r][i][0], ny = y + PIECES[p][r][i][1];
            if (nx < 0 || nx >= TW || ny >= TH) return false;
            if (ny >= 0 && board[ny][nx]) return false;
        }
        return true;
        };

    // 줄 지우기 이펙트
    auto clearLineEffect = [&](vector<int>& fullRows) {
        for (int flash = 0; flash < 4; flash++) {
            WORD col = (flash % 2 == 0) ? 0x000F : 0x000E;
            for (int fy : fullRows)
                for (int x = 0; x < TW; x++) {
                    bSet(TBX + x * 2, TBY + fy, L'\u2588', col);
                    bSet(TBX + x * 2 + 1, TBY + fy, L'\u2588', col);
                }
            bFlush4(); Sleep(55);
        }
        for (int step = 0; step <= TW / 2; step++) {
            for (int fy : fullRows) {
                int left = TW / 2 - step, right = TW / 2 + step;
                if (left >= 0 && left < TW) { bSet(TBX + left * 2, TBY + fy, L' ', 0x0000); bSet(TBX + left * 2 + 1, TBY + fy, L' ', 0x0000); }
                if (right >= 0 && right < TW) { bSet(TBX + right * 2, TBY + fy, L' ', 0x0000); bSet(TBX + right * 2 + 1, TBY + fy, L' ', 0x0000); }
            }
            bFlush4(); Sleep(25);
        }
        };

    auto place = [&]() {
        for (int i = 0; i < 4; i++) {
            int nx = curX + PIECES[curP][curR][i][0];
            int ny = curY + PIECES[curP][curR][i][1];
            if (ny >= 0 && ny < TH && nx >= 0 && nx < TW) board[ny][nx] = curP + 1;
        }
        vector<int> fullRows;
        for (int y = 0; y < TH; y++) {
            bool full = true;
            for (int x = 0; x < TW; x++) if (!board[y][x]) { full = false; break; }
            if (full) fullRows.push_back(y);
        }
        if (!fullRows.empty()) {
            clearLineEffect(fullRows);
            for (int fy : fullRows) {
                for (int yy = fy; yy > 0; yy--)
                    for (int x = 0; x < TW; x++) board[yy][x] = board[yy - 1][x];
                for (int x = 0; x < TW; x++) board[0][x] = 0;
            }
            int cleared = (int)fullRows.size();
            int pts[] = { 0,100,300,500,800 };
            score += pts[min(cleared, 4)] * level;
            lines += cleared;
            level = lines / 10 + 1;
            dropMs = max(80, 500 - (level - 1) * 50);
        }
        curP = nexP; nexP = rand() % 7; curR = 0; curX = 3; curY = 0;
        holdUsed = false;
        if (!valid(curP, curR, curX, curY)) gameOver = true;
        };

    auto drawBoard4 = [&]() {
        for (int y = 0; y < TH; y++)
            for (int x = 0; x < TW; x++) {
                bSet(TBX + x * 2, TBY + y, L'\u00B7', 0x0008);
                bSet(TBX + x * 2 + 1, TBY + y, L'\u00B7', 0x0008);
            }
        for (int y = 0; y < TH; y++)
            for (int x = 0; x < TW; x++)
                if (board[y][x]) {
                    WORD a = PIECE_ATTR[board[y][x] - 1];
                    bSet(TBX + x * 2, TBY + y, L'[', a);
                    bSet(TBX + x * 2 + 1, TBY + y, L']', a);
                }
        for (int i = 0; i < 4; i++) {
            int nx = curX + PIECES[curP][curR][i][0];
            int ny = curY + PIECES[curP][curR][i][1];
            if (ny >= 0 && ny < TH) {
                bSet(TBX + nx * 2, TBY + ny, L'[', PIECE_ATTR[curP]);
                bSet(TBX + nx * 2 + 1, TBY + ny, L']', PIECE_ATTR[curP]);
            }
        }
        // 고스트 블록
        int gy = curY;
        while (valid(curP, curR, curX, gy + 1)) gy++;
        if (gy != curY)
            for (int i = 0; i < 4; i++) {
                int nx = curX + PIECES[curP][curR][i][0];
                int ny = gy + PIECES[curP][curR][i][1];
                if (ny >= 0 && ny < TH && !board[ny][nx]) {
                    bSet(TBX + nx * 2, TBY + ny, L'[', 0x0008);
                    bSet(TBX + nx * 2 + 1, TBY + ny, L']', 0x0008);
                }
            }
        };

    Edge4 eUp, eSpc, eEsc, eHold;

    while (!gameOver) {
        DWORD now = GetTickCount();
        if (keyHeld4(VK_TAB)) { saveHighScore4(score); return true; }
        // 좌우 이동
        if (keyHeld4(VK_LEFT)) {
            if (now - lastMoveX >= dX) { if (valid(curP, curR, curX - 1, curY)) curX--; lastMoveX = now; }
        }
        else if (keyHeld4(VK_RIGHT)) {
            if (now - lastMoveX >= dX) { if (valid(curP, curR, curX + 1, curY)) curX++; lastMoveX = now; }
        }
        else lastMoveX = 0;

        // 소프트 드롭
        if (keyHeld4(VK_DOWN)) {
            if (now - lastMoveY >= dY) { if (valid(curP, curR, curX, curY + 1)) { curY++; score++; } lastMoveY = now; }
        }
        else lastMoveY = 0;

        // 회전
        if (eUp.once(VK_UP)) {
            int nr = (curR + 1) % 4;
            if (valid(curP, nr, curX, curY)) curR = nr;
        }

        // 하드 드롭
        if (eSpc.once(VK_SPACE)) {
            while (valid(curP, curR, curX, curY + 1)) { curY++; score += 2; }
            place();
        }

        // 블록 삭제 (C키, 3회)
        if (eHold.once('C') && !holdUsed && holdLeft > 0) {
            if (holdP < 0) {
                holdP = curP; curP = nexP; nexP = rand() % 7;
            }
            else {
                int tmp = holdP; holdP = curP; curP = tmp;
            }
            curR = 0; curX = 3; curY = 0;
            holdUsed = true;
            holdLeft--;
            if (!valid(curP, curR, curX, curY)) gameOver = true;
        }

        // ESC → 타이틀(false 반환)
        if (eEsc.once(VK_ESCAPE)) { saveHighScore4(score); return false; }

        // 자동 낙하
        if (now - lastDrop >= (DWORD)dropMs) {
            lastDrop = now;
            if (valid(curP, curR, curX, curY + 1)) curY++;
            else place();
        }

        if (score > hiScore) hiScore = score;

        // 그리기
        bClear4(L' ', 0x0000);
        updateStars4();
        tetDrawBorder4();
        drawBoard4();
        tetDrawHUD4(score, lines, nexP, holdP, holdUsed, holdLeft, hiScore);
        bFlush4();

        // ── 700점 클리어 ──────────────────────────────
        if (score >= 700) {
            saveHighScore4(score);
            // 클리어 연출
            for (int cy = 0; cy < TH; cy++) {
                for (int x = 0; x < TW; x++) {
                    bSet(TBX + x * 2, TBY + cy, L'\u2588', 0x000E);
                    bSet(TBX + x * 2 + 1, TBY + cy, L'\u2588', 0x000E);
                }
                bFlush4(); Sleep(18);
            }
            Sleep(200);
            bClear4(L' ', 0x0000); updateStars4();
            bCenter(8, L"** STAGE 4 CLEAR!! **", 0x000E);
            bCenter(10, L"Score: " + to_wstring(score) + L"   Lines: " + to_wstring(lines), 0x000B);
            bCenter(12, score >= loadHighScore4() ? L"NEW BEST SCORE!" : L"BEST: " + to_wstring(loadHighScore4()) + L" pt",
                score >= loadHighScore4() ? (WORD)0x000E : (WORD)0x0008);
            bCenter(15, L"ENTER: continue    ESC: quit", 0x0008);
            bFlush4(); flushKeys4();
            while (true) {
                if (keyHeld4(VK_RETURN)) { Sleep(60); return true; }
                if (keyHeld4(VK_ESCAPE)) { Sleep(60); return false; }
                Sleep(20);
            }
        }
        Sleep(16);
    }

    // 게임오버
    saveHighScore4(score);
    bClear4(L' ', 0x0000); updateStars4();
    bCenter(10, L"GAME OVER!", 0x000C);
    bCenter(12, L"Score: " + to_wstring(score), 0x0007);
    bCenter(13, score >= loadHighScore4() ? L"NEW BEST SCORE!" : L"BEST: " + to_wstring(loadHighScore4()) + L" pt",
        score >= loadHighScore4() ? (WORD)0x000E : (WORD)0x0008);
    bCenter(15, L"ENTER: retry    ESC: title", 0x0008);
    bFlush4(); flushKeys4();
    while (true) {
        if (keyHeld4(VK_RETURN)) { Sleep(60); return true; }
        if (keyHeld4(VK_TAB)) { Sleep(60); return true; }  // TAB 추가
        if (keyHeld4(VK_ESCAPE)) { Sleep(60); return false; }
        Sleep(20);
    }
}