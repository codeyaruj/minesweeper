# Minesweeper

A polished and fully featured implementation of Minesweeper with a clean architecture and smooth user experience.

This project separates game logic from rendering, making it easy to extend, test, and maintain.

---

## ✨ Features

### 🧠 Game Engine

* Pure game logic (no rendering dependencies)
* Efficient flat board storage using `std::vector<Cell>`
* Deterministic random number generator (seed support)
* First-click safety (never lose on your first move)
* Iterative flood-fill (safe for large boards)
* Save and load complete game state
* Replay system with move history
* Undo functionality
* Configurable board sizes and mine counts

---

### 🎨 Rendering & UI

* Responsive window resizing
* Smooth reveal animations
* Color-coded numbers (1–8)
* Hover highlighting
* Clean top panel (timer, mine counter, state)
* Debug overlay
* FPS counter (optional)

---

## 🎮 Controls

| Action               | Key         |
| -------------------- | ----------- |
| Reveal cell          | Left Click  |
| Toggle flag          | Right Click |
| Reset game           | `R`         |
| Toggle debug overlay | `D`         |
| Toggle FPS counter   | `F`         |
| Undo                 | `Ctrl + Z`  |
| Save game            | `Ctrl + S`  |
| Load game            | `Ctrl + L`  |
| Adjust tile size     | `+ / -`     |

---

## 📁 Project Structure

```
minesweeper/
├── include/
│   ├── engine/      # Game logic
│   ├── renderer/    # Rendering layer
│   └── core/        # Application glue
├── src/
│   ├── engine/
│   ├── renderer/
│   ├── core/
│   └── main.cpp
├── tests/           # Unit tests (planned)
└── CMakeLists.txt
```

The project is structured so that the game logic is completely independent of rendering. This allows easier testing and future expansion (for example, adding a different frontend).

---

## 🛠 Build Instructions

### Prerequisites

* CMake 3.15+
* C++17 compatible compiler
* Git (for fetching Raylib)

---

### Linux / macOS

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
./Minesweeper
```

---

### Windows (Visual Studio)

```bash
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release
.\Release\Minesweeper.exe
```

---

### Windows (MinGW)

```bash
mkdir build && cd build
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
mingw32-make -j4
.\Minesweeper.exe
```

---

## ⚙ Configuration

Default game settings can be modified in:

```
src/core/Application.cpp
```

Example:

```cpp
GameConfig game_config;
game_config.width = 30;
game_config.height = 16;
game_config.mine_count = 99;
game_config.seed = 12345;  // 0 = random seed
```

---

## 🔁 Deterministic Games

To generate reproducible boards, set a specific seed:

```cpp
GameConfig config(16, 16, 40, 42);
```

Using the same seed will generate the same board layout.

---

## 💾 Save / Load System

The game saves full state in binary format, including:

* Board configuration
* All cell states
* Move history
* Timer state
* Random seed

This allows full restoration of any saved session.

---

## 🔍 Debug Mode

Press `D` to toggle debug information:

* Board dimensions
* Mine count and positions
* Revealed cell count
* Flag count
* Move count
* Current seed

---

## 📊 Performance

* Board generation: O(n)
* Mine placement: O(m)
* Flood-fill reveal: O(k)
* Memory usage: ~5 bytes per cell (+ move history)

Even large boards remain responsive.

---

## 🧪 Testing

The engine layer is designed to be unit-testable independently of rendering.

Example test structure:

```cpp
TEST(GameEngine, FirstClickSafety) {
    GameConfig config(10, 10, 10, 42);
    GameEngine engine(config);

    engine.reveal(5, 5);
    EXPECT_FALSE(engine.get_cell(5, 5).is_mine);
}
```

---

## 🚀 Future Improvements

* Difficulty presets (Beginner, Intermediate, Expert)
* Persistent high scores
* Sound effects
* Statistics tracking
* Replay export/import
* Custom themes
* Touch support
* Achievements

---

## 📜 License

This project is intended for learning and experimentation.
Feel free to explore, modify, and extend it.

* Or rewrite it in a more product-style tone
* Or add badges and visual polish for open-source presentation
