# Othello (Reversi) — Library + Console Demo

**Short description**
- **Library:** `libothello` — core game logic and API for Othello/Reversi.  
- **Demo:** simple console-based application demonstrating library usage.

**Repository layout**
- `include/`        — public headers (API)  
- `src/`            — library source files  
- `app/`            — console demo (e.g., `main.cpp`)  
- `CMakeLists.txt`  — build configuration  
- `README.md`       — this file

**Build (out-of-source)**
1. `mkdir build`  
2. `cd build`  
3. `cmake ..`                # default: Release  
4. `cmake --build . -- parallel`         # builds libothello and othello-app

**Run demo**
- From build directory:  
  - On Linux: `./othello-app` (or `./app/othello-app` depending on layout)  
- The demo runs in the terminal with a simple text UI for playing/testing the library.

**What to commit**
- Source code, headers, CMake files, docs.  
- Do NOT commit build artifacts or IDE/workspace files (add them to `.gitignore`).

**License**
- 

**Contributing**
- Open issues or submit PRs for bugs/features; keep changes focused (library vs demo).
