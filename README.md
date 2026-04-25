# 🎮 PentaStageGame

**5개의 미니게임을 연속으로 클리어하는 C++ 콘솔 게임**

![title_screen](screenshots/title_screen.png)
<!-- 📸 사진 필요: 게임 시작 타이틀 화면 캡처 후 screenshots/ 폴더에 넣기 -->

---

## 🕹️ 게임 플레이

![gameplay](screenshots/gameplay.gif)
<!-- 📸 사진 필요: 실제 플레이 장면 GIF (ScreenToGif 추천) -->

5개의 스테이지를 순서대로 클리어해야 합니다.  
하나라도 실패하면 **처음부터 다시 시작**됩니다.

| 스테이지 | 게임명 | 클리어 조건 |
|:-------:|--------|:----------:|
| Stage 1 | 🐉 Dragon Flight | 보스 (300HP) 처치 |
| Stage 2 | 🏹 Arrow Dodge | 15초 생존 |
| Stage 3 | 🪜 Infinity Stairs | 100점 획득 |
| Stage 4 | 🚀 Tetris Space Edition | 700점 획득 |
| Stage 5 | 🐍 Snake Hard Mode | 200점 획득 |

---

## ▶️ 실행 화면

| 게임오버 | 올클리어 |
|:---:|:---:|
| ![gameover](screenshots/gameover.png) | ![allclear](screenshots/allclear.png) |
<!-- 📸 사진 필요: 게임오버 화면, 올클리어 화면 각각 캡처 -->

---

## 🛠️ 개발 환경

![C++](https://img.shields.io/badge/C++-00599C?style=flat&logo=cplusplus&logoColor=white)
![Windows](https://img.shields.io/badge/Windows-0078D6?style=flat&logo=windows&logoColor=white)
![Visual Studio](https://img.shields.io/badge/Visual%20Studio-5C2D91?style=flat&logo=visualstudio&logoColor=white)

- **언어**: C++
- **OS**: Windows
- **컴파일러**: MSVC / MinGW (g++)
- **라이브러리**: Windows API (`windows.h`, `conio.h`)

---

## 📁 파일 구조

```
PentaStageGame/
├── main.cpp
├── Common.h
├── Stage1_Dragon.h / .cpp
├── Stage2_Arrow.h  / .cpp
├── Stage3_Stair.h  / .cpp
├── Stage4_Tetris.h / .cpp
├── Stage5_Snake.h  / .cpp
├── screenshots/
│   ├── title_screen.png
│   ├── gameplay.gif
│   ├── gameover.png
│   └── allclear.png
└── README.md
```

---

## ▶️ 빌드 및 실행

### Visual Studio
1. 모든 `.cpp` `.h` 파일을 프로젝트에 추가
2. `Ctrl + F5` 로 빌드 및 실행

### g++ (MinGW)
```bash
g++ -o game main.cpp Stage1_Dragon.cpp Stage2_Arrow.cpp Stage3_Stair.cpp Stage4_Tetris.cpp Stage5_Snake.cpp
./game.exe
```

---

## 🎮 조작 방법

| 키 | 동작 |
|:--:|------|
| `↑` `↓` `←` `→` | 이동 |
| `Space` | 공격 / 블록 드롭 |
| `R` | 재시작 |
| `Q` | 종료 |

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
