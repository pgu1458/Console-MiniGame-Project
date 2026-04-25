#include "Stage1_Dragon.h"

// ── Player ────────────────────────────────────
void Player::draw() {
    int px = (int)x, py = (int)y;
    drawString(px - 1, py - 1, " ^ ", 0x0B);
    drawString(px - 1, py, "<*>", 0x0E);
    drawString(px - 1, py + 1, "/ \\", 0x0B);
}

// ── Bullet ────────────────────────────────────
void Bullet::update() { y -= 1.5f; if (y < 0) alive = false; }
void Bullet::draw() { drawChar((int)x, (int)y, '|', 0x0C); }

// ── Enemy ─────────────────────────────────────
Enemy::Enemy(float _x, float _y, int _type) : x(_x), y(_y), type(_type) {
    if (type == 0) { hp = 1;  maxHp = 1;  speed = 0.4f + (rand() % 3) * 0.1f; }
    if (type == 1) { hp = 5;  maxHp = 5;  speed = 0.25f + (rand() % 2) * 0.1f; }
    if (type == 2) { hp = 10; maxHp = 10; speed = 0.15f + (rand() % 2) * 0.05f; }
}
void Enemy::update() { y += speed; if (y >= HEIGHT) alive = false; }
void Enemy::draw() {
    int ex = (int)x, ey = (int)y;
    if (type == 0) {
        drawString(ex - 1, ey - 1, "\\./", 0x0D);
        drawString(ex - 1, ey, "(v)", 0x0C);
    }
    else if (type == 1) {
        drawString(ex - 1, ey - 1, "/^\\", 0x0B);
        drawString(ex - 1, ey, "[B]", 0x0B);
        int bar = (int)((float)hp / maxHp * 5);
        std::string b; for (int i = 0; i < 5; i++) b += (i < bar ? '=' : '.');
        drawString(ex - 2, ey + 1, b, 0x0A);
    }
    else {
        drawString(ex - 1, ey - 1, "***", 0x0E);
        drawString(ex - 1, ey, "{X}", 0x0E);
        int bar = (int)((float)hp / maxHp * 5);
        std::string b; for (int i = 0; i < 5; i++) b += (i < bar ? '#' : '.');
        drawString(ex - 2, ey + 1, b, 0x06);
    }
}

// ── BossBullet ────────────────────────────────
void BossBullet::update() {
    x += dx; y += dy;
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) alive = false;
}
void BossBullet::draw() { drawChar((int)x, (int)y, '*', 0x0E); }

// ── Boss ──────────────────────────────────────
void Boss::update(std::vector<BossBullet>& bBullets) {
    x += moveDir;
    if (x <= 3 || x >= WIDTH - 4) moveDir = -moveDir;
    if (++shootTimer >= 20) {
        shootTimer = 0;
        int shots = 2 + rand() % 3;
        for (int i = 0; i < shots; i++) {
            float angle = -0.8f + (rand() % 17) * 0.1f;
            BossBullet bb;
            bb.x = x; bb.y = y + 2;
            bb.dx = angle; bb.dy = 0.3f;
            bBullets.push_back(bb);
        }
    }
}
void Boss::draw() {
    int bx = (int)x, by = (int)y;
    drawString(bx - 3, by - 1, "=======", 0x0C);
    drawString(bx - 3, by, "|BOSS |", 0x0C);
    drawString(bx - 3, by + 1, "=======", 0x0C);
    int bar = (int)((float)hp / 300 * 20);
    std::string hpBar = "HP[";
    for (int i = 0; i < 20; i++) hpBar += (i < bar ? '#' : '.');
    hpBar += "]";
    drawString(1, 1, hpBar, 0x0C);
}

// ── Item ──────────────────────────────────────
void Item::update() { y += 0.2f; if (y >= HEIGHT) alive = false; }
void Item::draw() { drawChar((int)x, (int)y, '$', 0x0A); }

// ─────────────────────────────────────────────
//  스테이지 1 메인 루프
// ─────────────────────────────────────────────
bool runStage1() {
    // 창 크기: 드래곤 게임 크기
    setConsoleSize(41, HEIGHT + 2);

    Player player;
    std::vector<Bullet>     bullets;
    std::vector<Enemy>      enemies;
    std::vector<BossBullet> bossBullets;
    std::vector<Item>       items;

    Boss boss;
    bool bossActive = false;
    bool bossSpawned = false;
    bool bossCleared = false;

    int   spawnTimer = 0;
    int   itemTimer = 0;
    bool  gameOver = false;
    float shootCool = 0;

    // 스테이지 시작 안내
    system("cls");
    gotoxy(8, HEIGHT / 2 - 1); SetConsoleTextAttribute(hOut, 0x0E);
    std::cout << "=== STAGE 1: DRAGON FLIGHT ===";
    gotoxy(10, HEIGHT / 2 + 1); SetConsoleTextAttribute(hOut, 0x0F);
    std::cout << "보스를 쓰러뜨리면 다음 스테이지!";
    gotoxy(14, HEIGHT / 2 + 3); std::cout << "아무 키나 눌러 시작";
    _getch();
    system("cls");

    while (!gameOver && !bossCleared) {
        DWORD startTime = GetTickCount();

        // 1) 입력
        if (GetAsyncKeyState(VK_LEFT) & 0x8000 && player.x > 1)          player.x -= 1.5f;
        if (GetAsyncKeyState(VK_RIGHT) & 0x8000 && player.x < WIDTH - 2)  player.x += 1.5f;
        if (GetAsyncKeyState(VK_UP) & 0x8000 && player.y > 1)           player.y -= 1.0f;
        if (GetAsyncKeyState(VK_DOWN) & 0x8000 && player.y < HEIGHT - 2) player.y += 1.0f;
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) { gameOver = true; break; }
        if (GetAsyncKeyState(VK_TAB) & 0x8000) { bossCleared = true; break; } // 추가

        // 2) 자동 발사
        if (shootCool <= 0) {
            float px = player.x, py = player.y - 2;
            if (player.bulletLevel == 1) {
                bullets.push_back({ px, py });
            }
            else if (player.bulletLevel == 2) {
                bullets.push_back({ px - 1, py });
                bullets.push_back({ px + 1, py });
            }
            else {
                bullets.push_back({ px - 2, py });
                bullets.push_back({ px,     py });
                bullets.push_back({ px + 2, py });
            }
            shootCool = 5;
        }
        if (shootCool > 0) shootCool--;

        // 3) 업데이트
        if (!bossSpawned && player.score >= 500) {
            bossSpawned = true;
            bossActive = true;
            boss = Boss();
            enemies.clear();
        }

        if (!bossActive) {
            if (++spawnTimer >= 30) {
                spawnTimer = 0;
                float ex = 2.0f + rand() % (WIDTH - 4);
                int t = rand() % 10;
                if (t <= 6) enemies.push_back(Enemy(ex, 1.0f, 0));
                else if (t <= 8) enemies.push_back(Enemy(ex, 1.0f, 1));
                else             enemies.push_back(Enemy(ex, 1.0f, 2));
            }
        }

        if (++itemTimer >= 100) {
            itemTimer = 0;
            if (rand() % 3 == 0) {
                float ix = 2.0f + rand() % (WIDTH - 4);
                items.push_back({ ix, 1.0f });
            }
        }

        for (auto& b : bullets)    b.update();
        for (auto& e : enemies)    e.update();
        for (auto& bb : bossBullets) bb.update();
        for (auto& it : items)      it.update();
        if (bossActive && boss.alive) boss.update(bossBullets);

        // 4) 충돌 감지
        // 플레이어 총알 vs 일반 적
        for (auto& b : bullets) {
            for (auto& e : enemies) {
                if (!b.alive || !e.alive) continue;
                if (abs(b.x - e.x) < 2 && abs(b.y - e.y) < 2) {
                    b.alive = false;
                    e.hp--;
                    if (e.hp <= 0) {
                        e.alive = false;
                        if (e.type == 0) player.score += 10;
                        else if (e.type == 1) player.score += 50;
                        else                  player.score += 100;
                    }
                }
            }
        }

        // 플레이어 총알 vs 보스
        if (bossActive && boss.alive) {
            for (auto& b : bullets) {
                if (!b.alive) continue;
                if (abs(b.x - boss.x) < 4 && abs(b.y - boss.y) < 2) {
                    b.alive = false;
                    boss.hp--;
                    if (boss.hp <= 0) {
                        boss.alive = false;
                        bossActive = false;
                        bossCleared = true;
                        player.score += 1000;
                    }
                }
            }
        }

        // 보스 총알 vs 플레이어
        for (auto& bb : bossBullets) {
            if (!bb.alive) continue;
            if (abs(bb.x - player.x) < 2 && abs(bb.y - player.y) < 2) {
                bb.alive = false;
                player.hp--;
                if (player.hp <= 0) gameOver = true;
            }
        }

        // 일반 적 vs 플레이어
        for (auto& e : enemies) {
            if (!e.alive) continue;
            if (abs(e.x - player.x) < 2 && abs(e.y - player.y) < 2) {
                e.alive = false;
                player.hp--;
                if (player.hp <= 0) gameOver = true;
            }
        }

        // 아이템 vs 플레이어
        for (auto& it : items) {
            if (!it.alive) continue;
            if (abs(it.x - player.x) < 2 && abs(it.y - player.y) < 2) {
                it.alive = false;
                if (player.bulletLevel < 3) player.bulletLevel++;
            }
        }

        // 죽은 오브젝트 제거
        bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
            [](const Bullet& b) { return !b.alive; }), bullets.end());
        enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
            [](const Enemy& e) { return !e.alive; }), enemies.end());
        bossBullets.erase(std::remove_if(bossBullets.begin(), bossBullets.end(),
            [](const BossBullet& b) { return !b.alive; }), bossBullets.end());
        items.erase(std::remove_if(items.begin(), items.end(),
            [](const Item& it) { return !it.alive; }), items.end());

        // 5) 렌더링
        clearBuffer();

        for (int x = 0; x < WIDTH; x++) {
            drawChar(x, 0, '-', 0x08);
            drawChar(x, HEIGHT - 1, '-', 0x08);
        }
        for (int y = 0; y < HEIGHT; y++) {
            drawChar(0, y, '|', 0x08);
            drawChar(WIDTH - 1, y, '|', 0x08);
        }

        drawString(1, 0, " SCORE:" + std::to_string(player.score), 0x0E);
        std::string hpBar = "HP:";
        for (int i = 0; i < player.hp; i++) hpBar += (char)3;
        drawString(22, 0, hpBar, 0x0C);

        for (auto& it : items)       it.draw();
        for (auto& b : bullets)     b.draw();
        for (auto& e : enemies)     e.draw();
        for (auto& bb : bossBullets) bb.draw();
        if (bossActive && boss.alive) boss.draw();
        player.draw();

        render();

        DWORD elapsed = GetTickCount() - startTime;
        if (elapsed < 16) Sleep(16 - elapsed);
    }

    if (bossCleared) {
        // 클리어 연출
        system("cls");
        SetConsoleTextAttribute(hOut, 0x0A);
        gotoxy(8, HEIGHT / 2 - 1); std::cout << "=== STAGE 1 CLEAR! ===";
        gotoxy(8, HEIGHT / 2 + 1); std::cout << "다음 스테이지: 화살 피하기!";
        gotoxy(14, HEIGHT / 2 + 3);
        SetConsoleTextAttribute(hOut, 0x0F);
        std::cout << "아무 키나 눌러 계속";
        _getch();
        return true;
    }
    return false; // 게임오버
}
