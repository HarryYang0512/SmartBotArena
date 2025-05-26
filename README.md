# 🕹️ SmartBot Arena

A 2D arcade-style game built in **C++ with SDL3**, featuring a player-controlled bot and a Python-powered AI enemy. This project demonstrates real-time interaction between native C++ rendering and external AI decision-making — ideal for showcasing skills in game development, AI integration, and cross-language architecture.

---

## 🎯 Features

- ✅ **C++ SDL3 Game Engine** – handles rendering, player control, collision, and bullets
- ✅ **Python AI Enemy** – receives game state, makes decisions, and dodges bullets
- ✅ **Bullet Shooting System** – press Spacebar to fire and hit the enemy
- ✅ **Wraparound Screen** – classic arcade-style boundary teleport
- ✅ **Win/Loss Detection** – win by shooting the enemy, lose by colliding
- ✅ **Expandable Architecture** – supports ML integration and gameplay upgrades

---

## 🤖 Enemy AI

The enemy bot logic is written in Python (`PythonAI/bot_model.py`), and receives game state as JSON from C++ via command-line arguments. The AI:
- Chases the player based on relative position
- Dodges bullets if one is approaching from the left

The logic can later be replaced by a trained ML model (e.g., PyTorch, scikit-learn).

---

## 📁 Project Structure
SmartBotArena/
├── CppGame/
│ ├── src/
│ │ └── main.cpp # Main C++ game loop (SDL3)
│ └── SmartBotArena.exe # Output binary
├── PythonAI/
│ └── bot_model.py # Enemy AI logic in Python
├── assets/ # (optional) images, sounds
├── .vscode/ # VSCode task configuration
└── README.md

---

## 🛠️ Build & Run

### Requirements
- SDL3 (dev libraries) [Download](https://github.com/libsdl-org/SDL/releases)
- C++ compiler (MinGW64 / MSVC)
- Python 3.x (added to PATH)

### Build (via VSCode Task)

Ensure `.vscode/tasks.json` contains:

```json
"args": [
  "src/main.cpp",
  "-IC:/libs/SDL3/include",
  "-LC:/libs/SDL3/lib/x64",
  "-lSDL3",
  "-o", "SmartBotArena.exe"
]
