# Starship Game - C++ Game Library

A classic asteroids-style space shooter game implemented as a C++ library with a terminal-based demo.

## Features

- **Object-oriented design** with clean separation of concerns
- **Physics-based movement** with momentum and screen wrapping
- **Collision detection** using circular bounding boxes
- **Progressive difficulty** with level advancement
- **Terminal-based renderer** for quick testing (no external dependencies)
- **Extensible architecture** - easily add new entity types or renderers

## Project Structure

```
starship-game/
├── include/starship/       # Public header files
│   ├── vector2d.hpp       # 2D vector math
│   ├── entity.hpp         # Base entity class
│   ├── starship.hpp       # Player ship
│   ├── asteroid.hpp       # Asteroid objects
│   ├── projectile.hpp     # Bullets
│   └── game.hpp           # Main game logic
├── src/                   # Implementation files
│   └── game.cpp
├── examples/              # Example programs
│   └── main.cpp          # Terminal-based game
└── CMakeLists.txt        # Build configuration
```

## Building on macOS (M4)

### Prerequisites

```bash
# Install Xcode Command Line Tools
xcode-select --install

# Install CMake via Homebrew
brew install cmake
```

### Build Instructions

```bash
# Clone the repository (or navigate to your project)
cd starship-game

# Create build directory
mkdir build
cd build

# Configure for Apple Silicon
cmake ..

# Build
make

# Run the game
./starship-terminal
```

## How to Play

### Controls

- **W** - Thrust forward
- **A** - Rotate left
- **D** - Rotate right
- **SPACE** - Shoot
- **Q** - Quit game
- **R** - Restart (after game over)

### Gameplay

1. Destroy asteroids to earn points
2. Avoid collisions with asteroids (you have 3 lives)
3. Large asteroids split into medium, medium into small
4. Clear all asteroids to advance to the next level
5. Each level spawns more asteroids

### Scoring

- Large asteroids: 20 points
- Medium asteroids: 50 points
- Small asteroids: 100 points

## Library Usage

You can use the starship library in your own projects:

```cpp
#include "starship/game.hpp"

int main() {
    starship::Game game(80, 30);  // 80x30 game field
    
    while (!game.isGameOver()) {
        game.handleInput(getUserInput(), deltaTime);
        game.update(deltaTime);
        
        // Render using your own renderer
        renderGame(game);
    }
    
    return 0;
}
```

Link against the library:

```cmake
target_link_libraries(your_program PRIVATE starship)
```

## Architecture

### Core Classes

- **Vector2D**: 2D math operations (addition, multiplication, normalization)
- **Entity**: Base class for all game objects with position, velocity, collision
- **Starship**: Player-controlled ship with rotation, thrust, and shooting
- **Asteroid**: Floating space rocks that split when destroyed
- **Projectile**: Bullets fired by the player
- **Game**: Main game loop, collision detection, entity management

### Design Patterns

- **Inheritance**: Entity base class for polymorphic behavior
- **Composition**: Game contains collections of entities
- **Separation of Concerns**: Game logic separate from rendering

## Future Enhancements

Potential additions to the library:

1. **Power-ups**: Shield, rapid fire, extra lives
2. **Enemy ships**: AI-controlled opponents
3. **Particle effects**: Explosions and debris
4. **Sound system**: Abstract audio interface
5. **Save/load**: High scores and game state
6. **Multiplayer**: Network support
7. **Better renderers**: SDL2, SFML, or OpenGL backends

## License

MIT License - feel free to use this in your own projects!

## Credits

Created as a demonstration of C++ game library architecture for macOS M4.