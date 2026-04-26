# 🎮 Console-MiniGame-Project

**5개의 미니게임을 연속으로 클리어하는 C++ 콘솔 게임**

![C++](https://img.shields.io/badge/C++-00599C?style=flat&logo=cplusplus&logoColor=white)
![Windows](https://img.shields.io/badge/Windows-0078D6?style=flat&logo=windows&logoColor=white)
![Visual Studio](https://img.shields.io/badge/Visual%20Studio-5C2D91?style=flat&logo=visualstudio&logoColor=white)

<img width="600" height="700" alt="image" src="https://github.com/user-attachments/assets/a673d3f3-7d67-4c3f-9800-085148ca1258" />

<!-- 📸 사진 필요: 타이틀 화면 캡처 → screenshots/title_screen.png -->

<br>

| 1스테이지 - Dragon Flight | 2스테이지 - Arrow Dodge | 3스테이지 - Infinity Stairs | 4스테이지 - Tetris Space | 5스테이지 - Snake Hard Mode |
|:---:|:---:|:---:|:---:|:---:|
| ![s1](screenshots/stage1_play.png) | ![s2](screenshots/stage2_play.png) | ![s3](screenshots/stage3_play.png) | ![s4](screenshots/stage4_play.png) | ![s5](screenshots/stage5_play.png) |

---

## 🗺️ 전체 게임 구성

```
┌─────────────────────────────────────────────────────┐
│                   PENTA STAGE GAME                   │
└──────────────────────┬──────────────────────────────┘
                       │ 시작
          ┌────────────▼────────────┐
          │  STAGE 1: Dragon Flight │  🐉 보스 300HP 처치
          └────────────┬────────────┘
                  클리어│ ✅  실패 → 💀 GAME OVER → 재시작
          ┌────────────▼────────────┐
          │  STAGE 2: Arrow Dodge   │  🏹 15초 생존
          └────────────┬────────────┘
                  클리어│ ✅  실패 → 💀 GAME OVER → 재시작
          ┌────────────▼────────────┐
          │  STAGE 3: Infinity Stair│  🪜 100점 획득
          └────────────┬────────────┘
                  클리어│ ✅  실패 → 💀 GAME OVER → 재시작
          ┌────────────▼────────────┐
          │ STAGE 4: Tetris Space ★ │  🚀 700점 획득
          └────────────┬────────────┘
                  클리어│ ✅  실패 → 💀 GAME OVER → 재시작
          ┌────────────▼────────────┐
          │ STAGE 5: Snake Hard Mode│  🐍 200점 획득
          └────────────┬────────────┘
                  클리어│ ✅
          ┌────────────▼────────────┐
          │      🎉 ALL CLEAR!      │
          └─────────────────────────┘
```

| 스테이지 | 게임명 | 클리어 조건 | 담당 |
|:-------:|--------|:----------:|:----:|
| Stage 1 | 🐉 Dragon Flight | 보스 (300HP) 처치 | 최승호 |
| Stage 2 | 🏹 Arrow Dodge | 15초 생존 | 김영현 |
| Stage 3 | 🪜 Infinity Stairs | 100점 획득 | 정민지 |
| **Stage 4** | **🚀 Tetris Space Edition** | **700점 획득** | **박은수** |
| Stage 5 | 🐍 Snake Hard Mode | 200점 획득 | 김수하 |

---

## 📸 게임 화면

| Stage 1 - Dragon Flight | Stage 2 - Arrow Dodge | Stage 3 - Infinity Stairs |
|:---:|:---:|:---:|
| ![stage1](screenshots/stage1_play.png) | ![stage2](screenshots/stage2_play.png) | ![stage3](screenshots/stage3_play.png) |

| Stage 4 - Tetris Space Edition | Stage 5 - Snake Hard Mode |
|:---:|:---:|
| ![stage4](screenshots/stage4_play.png) | ![stage5](screenshots/stage5_play.png) |

<!-- 📸 사진 필요: 각 스테이지 플레이 화면 캡처 후 screenshots/ 폴더에 저장
     stage1_play.png / stage2_play.png / stage3_play.png / stage4_play.png / stage5_play.png -->

---

---

## 🚀 Stage 4: Tetris Space Edition *(박은수)*

![stage4](screenshots/stage4_play.png)
<!-- 📸 사진 필요: Stage 4 플레이 화면 캡처 → screenshots/stage4_play.png -->

### 📌 개요

> 우주를 배경으로 한 테트리스 게임.  
> 블록을 쌓아 줄을 완성하면 점수를 획득하며, **700점 달성 시 클리어**됩니다.  
> 점수가 높아질수록 블록 낙하 속도가 빨라집니다.

---

### 🎮 조작 방법

| 키 | 동작 |
|:--:|------|
| `←` `→` | 블록 좌우 이동 |
| `↓` | 블록 빠르게 내리기 |
| `↑` | 블록 회전 |
| `Space` | 블록 즉시 드롭 |
| `P` | 일시정지 |

---

### 📐 게임 구조

```
┌──────────────────────────────────┐
│         게임 보드 (10×20)         │  ← 블록이 쌓이는 공간
├──────────────────────────────────┤
│  현재 블록                        │  ← 7가지 테트로미노 (I, O, T, S, Z, J, L)
│  다음 블록 미리보기                │  ← NEXT 블록 표시
│  점수 / 레벨 / 라인 수             │  ← HUD 표시
└──────────────────────────────────┘
```

---

### 📊 점수 시스템

| 줄 제거 수 | 획득 점수 |
|:---------:|:--------:|
| 1줄 | 100점 |
| 2줄 | 300점 |
| 3줄 | 500점 |
| 4줄 (테트리스) | 800점 |

- 레벨이 오를수록 블록 낙하 속도 증가
- **700점 달성 → 스테이지 클리어**
- 블록이 천장에 닿으면 → **GAME OVER**

---

### 🖼️ 플레이 화면

| 게임 중 | 클리어 순간 |
|:---:|:---:|
| ![stage4_play](screenshots/stage4_play.png) | ![stage4_clear](screenshots/stage4_clear.png) |
<!-- 📸 사진 필요: stage4_play.png / stage4_clear.png -->

---

### 💻 주요 구현 내용

- **테트로미노 7종** 구현 및 회전 로직
- **줄 완성 감지** 및 삭제 후 위 블록 내려오는 처리
- **레벨 시스템** — 점수 구간별 낙하 속도 조정
- **다음 블록 미리보기** UI 구현
- **최고점수 저장** (`tetris_highscore.dat` 파일 입출력)
- Windows API 기반 **콘솔 렌더링** (깜빡임 최소화)

---

---

## 🛠️ 개발 환경

- **언어**: C++
- **OS**: Windows
- **컴파일러**: MSVC (Visual Studio 2022)
- **라이브러리**: Windows API (`windows.h`, `conio.h`)

---

## 📁 파일 구조

```
Console-MiniGame-Project/
├── main.cpp
├── Common.cpp / Common.h
├── Stage1_Dragon.cpp / .h
├── Stage2_Arrow.cpp  / .h
├── Stage3_Stair.cpp  / .h
├── stage4_tetris.cpp / .h     ← Stage 4 (박은수)
├── Stage5_snake.cpp  / .h
├── mini_game.sln
├── mini_game.vcxproj
├── mini_game.vcxproj.filters
├── screenshots/
│   ├── title_screen.png
│   ├── gameover.png
│   ├── allclear.png
│   ├── stage4_play.png
│   └── stage4_clear.png
├── .gitignore
└── README.md
```

---

## ▶️ 빌드 및 실행

### Visual Studio
1. `mini_game.sln` 파일 열기
2. 상단 메뉴 → **프로젝트 → 솔루션 대상 변경**
3. `Ctrl + F5` 로 빌드 및 실행

---

## 👥 팀원

| 이름 | 역할 | 담당 스테이지 |
|:----:|:----:|:------------:|
| 최승호 | 팀장 | Stage 1 🐉 Dragon Flight |
| 김수하 | 부팀장 | Stage 5 🐍 Snake Hard Mode |
| 김영현 | 팀원 | Stage 2 🏹 Arrow Dodge |
| 정민지 | 팀원 | Stage 3 🪜 Infinity Stairs |
| 박은수 | 팀원 | Stage 4 🚀 Tetris Space Edition |

---

> 본 프로젝트는 학습 목적으로 제작되었습니다.
