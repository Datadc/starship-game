# Starship Game Architecture

## Overview

`starship-game` is a small C++ arcade shooter with a clean separation between game logic and rendering.

The core engine lives in `include/starship/` and `src/game.cxx`. Rendering and input are handled by the SDL2 example in `examples/main.cxx`.

## Project Structure

- `include/starship/`
  - core entity definitions and game model interfaces
- `src/game.cxx`
  - game engine implementation and update loop
- `examples/main.cxx`
  - SDL2 rendering and input integration
- `CMakeLists.txt`
  - build configuration

## Core Architecture

### Entity Model

The engine is built around an `Entity` base class for shared motion and collision behavior.

Derived entities are:
- `Starship` — player-controlled rocket
- `Asteroid` — falling obstacles that can fragment
- `Projectile` — fired shots
- `PowerUp` — collectible effects

### Game Engine

`starship::Game` owns the entity collections and game state.

Its main responsibilities are:
- updating all entities each frame
- handling player input
- spawning asteroids and power-ups
- checking collisions
- removing inactive objects
- tracking score, level, and game-over state

## Architecture Diagrams

### Component Diagram

```
+-------------+        +-------------+         +-------------+
|   User /    |------->|    Game     |-------->|  Renderer   |
| Input /     |        |   Engine    |         |  (SDL2)     |
| Controls    |        +-------------+         +-------------+
+-------------+                |
                               v
                      +-------------------+
                      |   Entity System   |
                      +-------------------+
                               |
      +---------+------+------+------+---------+
      |         |      |      |      |         |
+---------+ +---------+ +---------+ +---------+
| Starship| | Asteroid| |Projectile| | PowerUp |
+---------+ +---------+ +---------+ +---------+
```

### Class Hierarchy

```
        +------------------+
        |     Entity       |
        +------------------+
                 /|\
                / | \
               /  |  \
              /   |   \
             v    v    v
      +---------+  +---------+  +---------+  +---------+
      |Starship |  |Asteroid |  |Projectile|  | PowerUp |
      +---------+  +---------+  +---------+  +---------+
                 \   |   /       
                  \  |  /        
                   \ | /         
                    v v          
                  +--------+      
                  | Game   |      
                  +--------+      
```

### Frame Update Flow

```
+-------------+
| Start Frame |
+-------------+
        |
        v
+-------------------------+
| Update game state       |
+-------------------------+
        |
        v
+-------------------------+
| Resolve collisions      |
+-------------------------+
        |
        v
+-------------------------+
| Remove inactive objects |
+-------------------------+
        |
        v
+-------------------------+
| Spawn entities          |
+-------------------------+
        |
        v
+-------------------------+
| Check game over         |
+-------------------------+
        |
        v
+-------------+
| End Frame   |
+-------------+
```

## Key Concepts

- `Entity` centralizes position, movement, and collision behavior.
- `Game` is the orchestrator: it updates entities, applies input, and manages game progression.
- Rendering is separate from game logic so the engine can be reused or extended.
- Power-ups and asteroids are managed as active collections and removed when no longer needed.

## Essential Flows

- Player input goes into `Game`, which updates the `Starship`.
- `Game` updates all entity lists every frame.
- Collisions are resolved inside the engine; effects are applied, and objects are marked inactive.
- Inactive objects are cleaned up before the next frame.
- Asteroids and power-ups are spawned dynamically to sustain gameplay.

## Spawning and Difficulty

- Initial asteroid wave: 8 asteroids
- Continuous spawn timer: every 2 seconds
- Spawn count scales with level: `1 + (level / 3)`
- New waves spawn when all asteroids are cleared: `6 + level * 2`
- Asteroid spawn position is random across the top of the screen
- Horizontal velocity has a random drift component
- Power-ups have a 15% spawn chance when an asteroid is destroyed

## Rendering and Example Use

Rendering is intentionally separated from game logic. The library exposes getters for all entity collections and game state, allowing any renderer to draw the scene.

Example usage pattern:

```cpp
starship::Game game(width, height);

while (!quit) {
    float deltaTime = ...;
    game.handleInput(input, deltaTime);
    game.update(deltaTime);
    renderGame(game);
}
```

The SDL2 example in `examples/main.cxx` is responsible for:

- creating the window and renderer
- handling SDL events and player input
- drawing the starship, asteroids, projectiles, power-ups, HUD, and game over screen

## Design Decisions

- `Entity` base class enables polymorphism and reusable motion/collision logic.
- `Game` manages state in simple `std::vector` containers to keep memory ownership straightforward.
- Timers are stored as `float` values for smooth delta-time updates.
- Game logic is independent of rendering, making the engine portable.
- Power-ups are implemented as timed effects rather than permanent upgrades.

## Future Enhancement Opportunities

- Add an `Enemy` class and AI behaviors
- Introduce tile-based levels or wave patterns
- Separate input handling from game state further
- Add sound effects and music
- Implement particle systems for explosions
- Make entity rendering pluggable with multiple backends

## Key Files at a Glance

- `include/starship/Vector2D.hxx`
- `include/starship/entity.hxx`
- `include/starship/starship.hxx`
- `include/starship/asteroid.hxx`
- `include/starship/projectile.hxx`
- `include/starship/powerup.hxx`
- `include/starship/game.hxx`
- `src/game.cxx`
- `examples/main.cxx`
