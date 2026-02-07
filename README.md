# Starship Game - C++ Game Library

A classic asteroids-style space shooter game implemented in C++ with SDL2 graphics rendering.

## Features

- **Object-oriented design** with clean separation of concerns
- **Real-time SDL2 graphics** with detailed rocket sprite rendering
- **Collision detection** using circular bounding boxes
- **Progressive difficulty** with level-scaled asteroid spawning
- **Continuous wave spawning** with asteroids appearing at top of screen
- **Score and lives tracking** with HUD display
- **Detailed rocket graphics** with nose cone, body, fins, and flame effects
- **Game-over screen** with final statistics
- **Extensible architecture** - easily add new entity types or renderers

## Project Structure

```
starship-game/
├── include/starship/          # Public header files
│   ├── Vector2D.hxx           # 2D vector math library
│   ├── entity.hxx             # Base entity class
│   ├── starship.hxx           # Player rocket
│   ├── asteroid.hxx           # Asteroid objects with size tiers
│   ├── projectile.hxx         # Projectiles (bullets)
│   └── game.hxx               # Main game logic
├── src/                       # Implementation files
│   └── game.cxx               # Game engine
├── examples/                  # Example programs
│   └── main.cxx               # SDL2 interactive game
└── CMakeLists.txt             # Build configuration
```

## Building on macOS (Apple Silicon)

### Prerequisites

```bash
# Install Xcode Command Line Tools
xcode-select --install

# Install CMake and SDL2 via Homebrew
brew install cmake sdl2 sdl2_ttf
```

### Build Instructions

```bash
# Navigate to project
cd starship-game

# Create and enter build directory
mkdir build
cd build

# Configure and build
cmake ..
make -j2

# Run the game
./starship-game
```

## How to Play

### Controls

- **A** - Move rocket left
- **D** - Move rocket right
- **SPACE** - Fire projectiles upward
- **Q** - Quit game

### Gameplay

1. **Rocket Position**: Your rocket is fixed at the bottom center of the screen
2. **Asteroid Waves**: Asteroids continuously spawn at the top and fall downward
3. **Destroy Asteroids**: Fire projectiles straight up to destroy asteroids
4. **Scoring**: Each asteroid destroyed = 1 point
5. **Lives**: Start with 3 lives. Lose a life when an asteroid touches your rocket
6. **Game Over**: Lose all 3 lives → game ends with final statistics displayed for 3 seconds
7. **Progressive Difficulty**: Asteroids spawn more frequently as you progress through levels

### Asteroid Types

- **Large Asteroids** (20px radius): Spawn at top, fall downward
- **Medium Asteroids** (12px radius): Created when large asteroids are destroyed
- **Small Asteroids** (6px radius): Created when medium asteroids are destroyed

### Scoring System

- **1 point** per asteroid destroyed (regardless of size)
- Score displayed at top center of screen
- Lives counter shown next to score

## Library Usage

You can use the starship library in your own projects:

```cpp
#include "starship/game.hpp"

int main() {
    int width = 800, height = 600;
    starship::Game game(width, height);
    
    // Your game loop with SDL2
    while (game.isActive()) {
        game.handleInput(events, deltaTime);
        game.update(deltaTime);
        
        // Render using your own renderer
        renderGame(game);
    }
    
    return 0;
}
```

Link against the library in CMake:

```cmake
target_link_libraries(your_program PRIVATE starship)
```

## Architecture

### Core Classes

- **Vector2D**: 2D math operations (addition, subtraction, scalar multiplication, normalization, distance)
- **Entity**: Base class for all game objects with position, velocity, collision detection
- **Starship**: Player-controlled rocket fixed at bottom center with left/right movement
- **Asteroid**: Space rocks that spawn at top and fall downward; split into smaller asteroids when destroyed
- **Projectile**: Projectiles fired straight upward with 2-second lifetime
- **Game**: Main game engine managing entities, collision detection, continuous asteroid spawning, and scoring

### Design Patterns

- **Inheritance**: Entity base class for polymorphic behavior (Starship, Asteroid, Projectile)
- **Composition**: Game contains collections of entities
- **Separation of Concerns**: Game logic separate from rendering (SDL2 in examples/main.cxx)
- **Delta-time physics**: Frame-rate independent movement and updates

### Gameplay Physics

- **Asteroid Spawning**: Continuous spawn timer (every 2 seconds at difficulty 1)
  - Spawn rate increases with level progression
  - Asteroids spawn randomly across top of screen
  - Fall straight down at constant velocity
- **Projectiles**: Fired straight upward with fixed velocity
  - Auto-deactivate after 2 seconds or leaving screen bounds
- **Collision Detection**: Circular bounding boxes for all entities
- **Screen Boundaries**: Asteroids and projectiles deactivate when leaving visible area

## Rendering Details (SDL2)

The main game example uses SDL2 for graphics rendering:

- **Rocket**: Detailed sprite with white nose cone, main body, light blue fins, and yellow/orange flame effect
- **Asteroids**: Gray circles rendered with line segments
- **Projectiles**: Small yellow/orange triangular fire effects
- **HUD**: Score and lives counter displayed at top center in white text
- **Game Over**: Red text display with final score and level reached, stays visible for 3 seconds before exit

## Future Enhancements

Potential additions to the library:

1. **Power-ups**: Shield, rapid fire, extra lives
2. **Enemy ships**: AI-controlled opponents
3. **Particle effects**: Enhanced explosions and debris
4. **Sound system**: Audio effects for shots and collisions
5. **Save/load**: High scores tracking
6. **Alternative renderers**: SFML, OpenGL backends
7. **Touch controls**: Mobile/tablet input support

## Requirements

- **C++17** or later
- **CMake 3.15+**
- **SDL2** and **SDL2_ttf** (for graphics and text rendering)
- **macOS** with Apple Silicon (arm64) or Intel processors

## License

MIT License - feel free to use this in your own projects!

## Credits

Developed as a modern C++ implementation of the classic Asteroids arcade game.