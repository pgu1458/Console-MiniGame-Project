#include "Stage5_Snake.h"

// ═══════════════════════════════════════
//  색상
// ═══════════════════════════════════════
#define RST5        "\033[0m"
#define BOLD5       "\033[1m"
#define FG_RED5     "\033[91m"
#define FG_GREEN5   "\033[92m"
#define FG_YELLOW5  "\033[93m"
#define FG_MAGENTA5 "\033[95m"
#define FG_CYAN5    "\033[96m"
#define FG_WHITE5   "\033[97m"
#define FG_GRAY5    "\033[90m"

// ═══════════════════════════════════════
//  레이아웃 상수
// ═══════════════════════════════════════
static const int SW5 = 20, SH5 = 20, CELL5 = 2;
static const int MX5 = 1, MY5 = 1;
static const int BX5 = MX5 + 1, BY5 = MY5 + 1;
static const int PX5 = MX5 + 1 + SW5 * CELL5 + 1 + 2;

// ═══════════════════════════════════════
//  콘솔 유틸 (Snake 전용)
// ═══════════════════════════════════════
static HANDLE hSnakeOut = nullptr;

static void sGotoxy(int x, int y) {
    SetConsoleCursorPosition(hSnakeOut, { (SHORT)x, (SHORT)y });
}

static void sCls() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD w;
    GetConsoleScreenBufferInfo(hSnakeOut, &csbi);
    DWORD n = (DWORD)csbi.dwSize.X * csbi.dwSize.Y;
    COORD o = { 0,0 };
    FillConsoleOutputCharacterA(hSnakeOut, ' ', n, o, &w);
    FillConsoleOutputAttribute(hSnakeOut, csbi.wAttributes, n, o, &w);
    SetConsoleCursorPosition(hSnakeOut, o);
}

static void initSnakeConsole() {
    hSnakeOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    DWORD mode = 0;
    GetConsoleMode(hSnakeOut, &mode);
    SetConsoleMode(hSnakeOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    CONSOLE_CURSOR_INFO ci = { 1, FALSE };
    SetConsoleCursorInfo(hSnakeOut, &ci);
    COORD buf = { 90, 32 };
    SetConsoleScreenBufferSize(hSnakeOut, buf);
    SMALL_RECT win = { 0, 0, 89, 31 };
    SetConsoleWindowInfo(hSnakeOut, TRUE, &win);
}

// ═══════════════════════════════════════
//  그리기 기초
// ═══════════════════════════════════════
struct Pt5 { int x, y; bool operator==(const Pt5& o)const { return x == o.x && y == o.y; } };
enum class Dir5 { UP, DOWN, LEFT, RIGHT };

static void drawCell5(int cx, int cy, const char* color, const char* ch = "  ") {
    sGotoxy(BX5 + cx * CELL5, BY5 + cy);
    std::cout << color << ch << RST5;
}

static void pline5(int dy, const std::string& s) {
    sGotoxy(PX5, MY5 + dy);
    std::cout << "                      ";
    sGotoxy(PX5, MY5 + dy);
    std::cout << s;
}

// ═══════════════════════════════════════
//  맵 테두리
// ═══════════════════════════════════════
static void drawBorder5() {
    sGotoxy(MX5, MY5);
    std::cout << FG_RED5 BOLD5 "+";
    for (int i = 0; i < SW5 * CELL5; i++) std::cout << "-";
    std::cout << "+" RST5;
    sGotoxy(MX5, MY5 + SH5 + 1);
    std::cout << FG_RED5 BOLD5 "+";
    for (int i = 0; i < SW5 * CELL5; i++) std::cout << "-";
    std::cout << "+" RST5;
    for (int y = 0; y < SH5; y++) {
        sGotoxy(MX5, MY5 + 1 + y); std::cout << FG_RED5 BOLD5 "|" RST5;
        sGotoxy(MX5 + 1 + SW5 * CELL5, MY5 + 1 + y); std::cout << FG_RED5 BOLD5 "|" RST5;
    }
}

// ═══════════════════════════════════════
//  패널
// ═══════════════════════════════════════
static void drawPanel5(int score, int hi, int level, int len, bool paused, bool started) {
    pline5(0, FG_RED5 BOLD5 "+-----------------------+" RST5);
    pline5(1, FG_RED5 BOLD5 "|   SNAKE  HARD  MODE   |" RST5);
    pline5(2, FG_RED5 BOLD5 "+-----------------------+" RST5);
    pline5(3, "");
    pline5(4, FG_GRAY5 "  SCORE" RST5);
    pline5(5, std::string(FG_YELLOW5 BOLD5 "  ") + std::to_string(score) + RST5);
    pline5(6, "");
    pline5(7, FG_GRAY5 "  BEST" RST5);
    pline5(8, std::string(FG_WHITE5 "  ") + std::to_string(hi) + RST5);
    pline5(9, "");
    pline5(10, FG_GRAY5 "  LEVEL" RST5);
    std::string bar = "  ";
    for (int i = 1; i <= 10; i++)
        bar += (i <= level) ? "\033[91m\xe2\x96\x88\033[0m" : "\033[90m\xe2\x96\x91\033[0m";
    pline5(11, bar);
    pline5(12, "");
    pline5(13, FG_GRAY5 "  LENGTH" RST5);
    pline5(14, std::string(FG_GREEN5 "  ") + std::to_string(len) + RST5);
    pline5(15, "");
    pline5(16, FG_GRAY5 "  --------------------" RST5);
    pline5(17, "");
    pline5(18, FG_YELLOW5 "  W/UP  A/LEFT" RST5);
    pline5(19, FG_YELLOW5 "  S/DOWN  D/RIGHT" RST5);
    pline5(20, FG_YELLOW5 "  P:pause  Q:quit" RST5);
    pline5(21, "");
    pline5(22, FG_GRAY5 "  ** normal +10" RST5);
    pline5(23, FG_YELLOW5 "  ** gold   +30" RST5);
    pline5(24, FG_MAGENTA5 "  xx poison -20" RST5);
    if (paused)
        pline5(25, FG_RED5 BOLD5 "  !! PAUSED !!" RST5);
    else if (!started)
        pline5(25, FG_RED5 BOLD5 "  STAGE 5 START!" RST5);
    else
        pline5(25, "");
    std::cout.flush();
}

// ═══════════════════════════════════════
//  먹이 / 게임
// ═══════════════════════════════════════
struct Food5 { Pt5 pos; int type, timer; };

struct Game5 {
    std::deque<Pt5>    snake;
    Dir5               dir, nextDir;
    std::vector<Food5> foods;
    int                score, hi, level, tick, speed;
    bool               alive, paused, started, cleared;
    std::mt19937       rng;

    static int speedFor(int lv) {
        const int spd[] = { 350,280,220,180,150,120,100,80,65,55 };
        return spd[std::min(lv - 1, 9)];
    }

    Game5(int hiScore) : score(0), hi(hiScore), level(6), tick(0), speed(speedFor(6)),
        alive(true), paused(false), started(false), cleared(false),
        rng(std::random_device{}())
    {
        snake.push_back({ SW5 / 2, SH5 / 2 });
        snake.push_back({ SW5 / 2, SH5 / 2 + 1 });
        snake.push_back({ SW5 / 2, SH5 / 2 + 2 });
        dir = nextDir = Dir5::UP;
        spawnFood();
    }

    bool occupied(Pt5 p) {
        for (auto& s : snake) if (s == p) return true;
        for (auto& f : foods) if (f.pos == p) return true;
        return false;
    }

    void spawnFood() {
        if (foods.size() >= 3) return;
        std::uniform_int_distribution<int> rx(0, SW5 - 1), ry(0, SH5 - 1);
        Pt5 p;
        int tries = 0;
        do { p = { rx(rng), ry(rng) }; tries++; } while (occupied(p) && tries < 200);
        std::uniform_int_distribution<int> rt(0, 9);
        int t = rt(rng);
        int type = (t < 6) ? 0 : (t < 9) ? 1 : 2;
        foods.push_back({ p, type, (type == 2) ? 30 : 0 });
    }

    void drawSnake() {
        for (int i = 0; i < (int)snake.size(); i++) {
            if (i == 0)
                drawCell5(snake[i].x, snake[i].y, "\033[42m", "  ");
            else
                drawCell5(snake[i].x, snake[i].y, "\033[102m", "  ");
        }
    }

    void drawFood(const Food5& f) {
        if (f.type == 0)
            drawCell5(f.pos.x, f.pos.y, "\033[41m", "  ");
        else if (f.type == 1)
            drawCell5(f.pos.x, f.pos.y, "\033[43m", "  ");
        else
            drawCell5(f.pos.x, f.pos.y, "\033[45m", "  ");
    }

    bool update() {
        if (paused || !started) return true;
        tick++;

        // 독 타이머
        for (int i = (int)foods.size() - 1; i >= 0; i--) {
            if (foods[i].type == 2) {
                foods[i].timer--;
                if (foods[i].timer <= 0) {
                    drawCell5(foods[i].pos.x, foods[i].pos.y,
                        ((foods[i].pos.x + foods[i].pos.y) % 2 == 0) ? "\033[48;5;232m" : "\033[48;5;233m", "  ");
                    foods.erase(foods.begin() + i);
                    spawnFood();
                }
            }
        }

        dir = nextDir;
        Pt5 head = snake.front();
        if (dir == Dir5::UP)    head.y--;
        else if (dir == Dir5::DOWN)  head.y++;
        else if (dir == Dir5::LEFT)  head.x--;
        else if (dir == Dir5::RIGHT) head.x++;

        // 벽 충돌
        if (head.x < 0 || head.x >= SW5 || head.y < 0 || head.y >= SH5) { alive = false; return false; }
        // 자기 충돌
        for (auto& s : snake) if (s == head) { alive = false; return false; }

        snake.push_front(head);

        bool ate = false;
        for (int i = (int)foods.size() - 1; i >= 0; i--) {
            if (foods[i].pos == head) {
                if (foods[i].type == 0) { score += 10; }
                else if (foods[i].type == 1) { score += 30; }
                else {
                    score = std::max(0, score - 20);
                    // 몸 단축
                    if (snake.size() > 3) {
                        drawCell5(snake.back().x, snake.back().y,
                            ((snake.back().x + snake.back().y) % 2 == 0) ? "\033[48;5;232m" : "\033[48;5;233m", "  ");
                        snake.pop_back();
                    }
                }
                if (score > hi) hi = score;
                // 레벨업
                level = std::min(10, 6 + score / 50);
                speed = speedFor(level);
                drawCell5(foods[i].pos.x, foods[i].pos.y,
                    ((foods[i].pos.x + foods[i].pos.y) % 2 == 0) ? "\033[48;5;232m" : "\033[48;5;233m", "  ");
                foods.erase(foods.begin() + i);
                spawnFood();
                ate = true;
                break;
            }
        }
        if (!ate) {
            Pt5 tail = snake.back();
            snake.pop_back();
            drawCell5(tail.x, tail.y,
                ((tail.x + tail.y) % 2 == 0) ? "\033[48;5;232m" : "\033[48;5;233m", "  ");
        }

        if (score >= 200) { alive = false; cleared = true; }
        return true;
    }

    void draw() {
        drawSnake();
        for (auto& f : foods) drawFood(f);
    }
};

// ═══════════════════════════════════════
//  키 처리
// ═══════════════════════════════════════
static void handleKey5(Game5& g) {
    while (_kbhit()) {
        int k = _getch();
        if (k == 224 || k == 0) {
            k = _getch();
            if (k == 72) { g.nextDir = Dir5::UP;    g.started = true; }
            else if (k == 80) { g.nextDir = Dir5::DOWN;  g.started = true; }
            else if (k == 75) { g.nextDir = Dir5::LEFT;  g.started = true; }
            else if (k == 77) { g.nextDir = Dir5::RIGHT; g.started = true; }
        }
        else {
            char c = (char)tolower(k);
            if (c == 'w') { g.nextDir = Dir5::UP;    g.started = true; }
            else if (c == 's') { g.nextDir = Dir5::DOWN;  g.started = true; }
            else if (c == 'a') { g.nextDir = Dir5::LEFT;  g.started = true; }
            else if (c == 'd') { g.nextDir = Dir5::RIGHT; g.started = true; }
            else if (c == 'p') g.paused = !g.paused;
            else if (c == 'q') g.alive = false;
            else if (c == '\t') { g.cleared = true; g.alive = false; }
        }
    }
}

// ═══════════════════════════════════════
//  폭죽 파티클
// ═══════════════════════════════════════
struct Spark5 { float x, y, vx, vy; int color, life; };
static const char* SPARK_COLORS5[] = { "", "\033[91m","\033[93m","\033[96m","\033[92m","\033[95m" };
static const char* SPARK_CHARS5[] = { "*","+",".","o","*","+" };

static void drawClear5(int score, int hi) {
    sCls();
    int cx = 22, cy = 8;
    auto box = [&](int dx, int dy, const char* s) {
        sGotoxy(cx + dx, cy + dy); std::cout << s;
        };
    box(0, 0, "\033[93m\033[1m╔══════════════════════════════╗\033[0m");
    box(0, 1, "\033[93m\033[1m║                              ║\033[0m");
    box(0, 2, "\033[92m\033[1m║    ★  STAGE 5 CLEAR! ★      ║\033[0m");
    box(0, 3, "\033[93m\033[1m║                              ║\033[0m");
    box(0, 4, "\033[93m\033[1m╚══════════════════════════════╝\033[0m");
    sGotoxy(cx + 4, cy + 6); std::cout << "\033[97m SCORE : \033[93m\033[1m" << score << "\033[0m";
    sGotoxy(cx + 4, cy + 7); std::cout << "\033[97m BEST  : \033[97m\033[1m" << hi << "\033[0m";
    sGotoxy(cx + 4, cy + 9); std::cout << "\033[92m\033[1mENTER\033[0m\033[90m - next stage\033[0m";
    std::cout.flush();

    std::mt19937 rng{ std::random_device{}() };
    std::uniform_real_distribution<float> rAngle(0.f, 6.28f);
    std::uniform_real_distribution<float> rSpeed(0.3f, 1.4f);
    std::uniform_int_distribution<int>    rColor(1, 5);
    std::uniform_int_distribution<int>    rX(5, 70), rY(1, 25);
    std::uniform_int_distribution<int>    rSparks(8, 16);
    std::vector<Spark5> sparks;

    auto burst = [&](int bx, int by) {
        int n = rSparks(rng), col = rColor(rng);
        for (int i = 0; i < n; i++) {
            float a = rAngle(rng), s = rSpeed(rng);
            sparks.push_back({ (float)bx,(float)by, s * std::cos(a) * 2.f, s * std::sin(a), col, 12 + i % 5 });
        }
        };

    for (int frame = 0; frame < 40; frame++) {
        if (frame % 5 == 0) { burst(rX(rng), rY(rng)); burst(rX(rng), rY(rng)); }
        for (auto& sp : sparks) {
            int ox = (int)sp.x, oy = (int)sp.y;
            if (ox >= 0 && ox < 89 && oy >= 0 && oy < 30) { sGotoxy(ox, oy); std::cout << ' '; }
            sp.x += sp.vx; sp.y += sp.vy * 0.5f; sp.vy += 0.15f; sp.life--;
            int nx = (int)sp.x, ny = (int)sp.y;
            if (sp.life > 0 && nx >= 0 && nx < 89 && ny >= 0 && ny < 30) {
                sGotoxy(nx, ny);
                std::cout << SPARK_COLORS5[sp.color] << SPARK_CHARS5[sp.life % 6] << "\033[0m";
            }
        }
        sparks.erase(std::remove_if(sparks.begin(), sparks.end(), [](const Spark5& s) {return s.life <= 0; }), sparks.end());
        box(0, 0, "\033[93m\033[1m╔══════════════════════════════╗\033[0m");
        box(0, 1, "\033[93m\033[1m║                              ║\033[0m");
        box(0, 2, "\033[92m\033[1m║    ★  STAGE 5 CLEAR! ★      ║\033[0m");
        box(0, 3, "\033[93m\033[1m║                              ║\033[0m");
        box(0, 4, "\033[93m\033[1m╚══════════════════════════════╝\033[0m");
        sGotoxy(cx + 4, cy + 6); std::cout << "\033[97m SCORE : \033[93m\033[1m" << score << "\033[0m";
        sGotoxy(cx + 4, cy + 9); std::cout << "\033[92m\033[1mENTER\033[0m\033[90m - next stage\033[0m";
        std::cout.flush();
        std::this_thread::sleep_for(std::chrono::milliseconds(80));
    }
}

static void drawGameOver5(int score, int hi) {
    int cx = MX5 + 1 + SW5 * CELL5 / 2 - 11;
    int cy = MY5 + SH5 / 2 - 3;
    for (int y = 0; y < 8; y++) { sGotoxy(cx - 1, cy + y); std::cout << "\033[40m                       " << RST5; }
    sGotoxy(cx, cy);   std::cout << FG_RED5 BOLD5 "╔═════════════════════╗" RST5;
    sGotoxy(cx, cy + 1); std::cout << FG_RED5 BOLD5 "║    GAME OVER        ║" RST5;
    sGotoxy(cx, cy + 2); std::cout << FG_RED5 BOLD5 "╚═════════════════════╝" RST5;
    sGotoxy(cx + 2, cy + 4); std::cout << FG_YELLOW5 "SCORE : " BOLD5 << score << RST5;
    sGotoxy(cx + 2, cy + 5); std::cout << FG_WHITE5  "BEST  : " BOLD5 << hi << RST5;
    sGotoxy(cx + 1, cy + 7);
    std::cout << "\033[92m\033[1mR\033[0m\033[90m retry   \033[91m\033[1mQ\033[0m\033[90m quit\033[0m";
}

// ═══════════════════════════════════════
//  스테이지 5 메인 루프
// ═══════════════════════════════════════
bool runStage5() {
    initSnakeConsole();

    // 시작 안내
    sCls();
    sGotoxy(25, 8);  std::cout << FG_YELLOW5 BOLD5 "=== STAGE 5: SNAKE HARD MODE ===" RST5;
    sGotoxy(25, 11); std::cout << FG_WHITE5 "Eat food and reach 200 points!" RST5;
    sGotoxy(25, 12); std::cout << FG_WHITE5 "W/A/S/D or Arrow keys to move" RST5;
    sGotoxy(25, 13); std::cout << FG_WHITE5 "P: pause   Q: quit" RST5;
    sGotoxy(25, 16); std::cout << FG_GREEN5 BOLD5 "Press ENTER to start" RST5;
    std::cout.flush();

    while (true) {
        if (_kbhit()) {
            char c = (char)_getch();
            if (c == '\r' || c == '\n') break;
            if (c == '\t') break;              // TAB도 시작으로 처리
            if (tolower(c) == 'q') return false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    int hi = 0;

RESTART5:
    sCls();
    drawBorder5();
    for (int y = 0; y < SH5; y++)
        for (int x = 0; x < SW5; x++)
            drawCell5(x, y, ((x + y) % 2 == 0) ? "\033[48;5;232m" : "\033[48;5;233m", "  ");

    Game5 g(hi);
    g.draw();
    drawPanel5(0, hi, 6, (int)g.snake.size(), false, false);

    auto prev = std::chrono::steady_clock::now();

    while (g.alive) {
        handleKey5(g);
        auto now = std::chrono::steady_clock::now();
        int dt = (int)std::chrono::duration_cast<std::chrono::milliseconds>(now - prev).count();
        if (dt >= g.speed) {
            g.update();
            if (g.alive) {
                g.draw();
                drawPanel5(g.score, g.hi, g.level, (int)g.snake.size(), g.paused, g.started);
            }
            prev = now;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(4));
    }

    hi = g.hi;

    if (g.cleared) {
        drawClear5(g.score, hi);
        while (true) {
            if (_kbhit()) {
                char c = (char)_getch();
                if (c == '\r' || c == '\n') return true;
                if (c == '\t') return true;    // TAB도 다음 스테이지로
                if (tolower(c) == 'q') return false;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    }
    else {
        drawGameOver5(g.score, hi);
        while (true) {
            if (_kbhit()) {
                char c = (char)tolower(_getch());
                if (c == 'r') goto RESTART5;
                if (c == 'q') return false;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    }
}