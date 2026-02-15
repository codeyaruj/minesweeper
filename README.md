# Advanced Minesweeper

A production-quality Minesweeper implementation in C++ with clean architecture separation.

## Features

### Game Engine
- **Zero Raylib dependencies** - Pure C++ game logic
- **Flat contiguous storage** - `std::vector<Cell>` for cache-friendly performance
- **Deterministic RNG** - Seed support for reproducible games
- **First-click safety** - Never hit a mine on first reveal
- **Iterative flood-fill** - No recursion, stack-safe
- **Full serialization** - Save/load game state
- **Replay system** - Store and replay all moves
- **Undo functionality** - Complete state restoration
- **Configurable boards** - Any size and mine count

### Rendering
- **Responsive scaling** - Window resize support
- **Smooth animations** - Reveal effects
- **Color-coded numbers** - 8 distinct colors for adjacency counts
- **Hover highlighting** - Visual feedback
- **Clean UI panel** - Timer, mine counter, game state
- **Debug overlay** - Show mines and statistics

### Controls
- **Left Click** - Reveal cell
- **Right Click** - Toggle flag
- **R** - Reset game
- **D** - Toggle debug overlay
- **F** - Toggle FPS counter
- **Ctrl+Z** - Undo last move
- **Ctrl+S** - Save game
- **Ctrl+L** - Load game
- **+/-** - Adjust tile size

## Architecture

```
minesweeper/
├── include/
│   ├── engine/          # Game logic (zero external dependencies)
│   │   ├── Cell.h
│   │   ├── Move.h
│   │   ├── GameConfig.h
│   │   └── GameEngine.h
│   ├── renderer/        # Rendering layer (Raylib)
│   │   └── Renderer.h
│   └── core/           # Application glue
│       └── Application.h
├── src/
│   ├── engine/
│   │   └── GameEngine.cpp
│   ├── renderer/
│   │   └── Renderer.cpp
│   ├── core/
│   │   └── Application.cpp
│   └── main.cpp
├── tests/              # Unit tests (future)
└── CMakeLists.txt
```

### Design Principles
- **Separation of concerns** - Game logic completely isolated from rendering
- **RAII** - Automatic resource management
- **No global state** - Everything explicitly passed
- **Modern C++17** - Smart pointers, structured bindings, constexpr
- **Unit-testable** - Engine can be tested without graphics

## Build Instructions

### Prerequisites
- CMake 3.15+
- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- Git (for fetching Raylib)

### Linux/macOS
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
./Minesweeper
```

### Windows (Visual Studio)
```bash
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release
.\Release\Minesweeper.exe
```

### Windows (MinGW)
```bash
mkdir build && cd build
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
mingw32-make -j4
.\Minesweeper.exe
```

## Configuration

Edit `src/core/Application.cpp` to change default settings:

```cpp
GameConfig game_config;
game_config.width = 30;        // Board width
game_config.height = 16;       // Board height
game_config.mine_count = 99;   // Number of mines
game_config.seed = 12345;      // Fixed seed (0 = random)
```

## Advanced Features

### Deterministic Games
Set a specific seed for reproducible boards:
```cpp
GameConfig config(16, 16, 40, 42);  // Last parameter is seed
```

### Save/Load System
Game state is saved in binary format including:
- Board configuration and state
- All cell states
- Move history
- Timer state

### Replay System
Access move history to implement replay:
```cpp
const auto& moves = engine.get_move_history();
for (size_t i = 0; i < moves.size(); ++i) {
    engine.replay_move(i);
}
```

### Debug Mode
Press **D** to see:
- Board dimensions
- Mine count and positions
- Revealed cell count
- Flag count
- Move count
- Current seed

## Performance Characteristics

- **Board generation**: O(n) where n = width × height
- **Mine placement**: O(m) where m = mine count
- **Flood-fill reveal**: O(k) where k = revealed cells (iterative queue)
- **Memory**: ~5 bytes per cell + move history

## Testing

The engine layer is fully unit-testable. Example test structure:

```cpp
TEST(GameEngine, FirstClickSafety) {
    GameConfig config(10, 10, 10, 42);
    GameEngine engine(config);
    
    engine.reveal(5, 5);
    EXPECT_FALSE(engine.get_cell(5, 5).is_mine);
}
```

## License

This is a demonstration project. Use freely for learning and reference.

## Future Enhancements

- [ ] Custom difficulty presets (Beginner, Intermediate, Expert)
- [ ] High score persistence
- [ ] Sound effects integration
- [ ] Multiplayer support
- [ ] Replay file export/import
- [ ] Statistics tracking
- [ ] Achievement system
- [ ] Custom themes
- [ ] Touchscreen support

## Technical Notes

### Why Flat Storage?
Using `std::vector<Cell>` instead of `std::vector<std::vector<Cell>>`:
- Better cache locality
- No pointer indirection
- Easier serialization
- Modern C++ best practice

### Why Iterative Flood-Fill?
Stack-based iteration instead of recursion:
- No stack overflow on large reveals
- Predictable memory usage
- Better performance profile

### Why Separate Engine/Renderer?
- Engine can be unit tested without graphics
- Engine can be used headless (AI, benchmarking)
- Multiple renderers possible (console, web, etc.)
- Clean architecture principles

## Author

Senior C++ Systems Engineer demonstration project.
