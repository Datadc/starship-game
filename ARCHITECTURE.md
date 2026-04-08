# Starship Game Architecture

## Overview

`starship-game` is a C++17 arcade-style shooter inspired by Asteroids. The project is organized as a lightweight game engine library with a clear separation between game model, entity behavior, and rendering.

The core engine is implemented in `include/starship/` and `src/game.cxx`. The example application in `examples/main.cxx` is responsible for SDL2 rendering and input.

## Project Structure

- `include/starship/`
  - `Vector2D.hxx` - 2D math utilities used by all entities.
  - `entity.hxx` - Base `Entity` class for position, velocity, collision, and screen wrapping.
  - `starship.hxx` - Player rocket class with movement, thrust, health, and boundary constraints.
  - `asteroid.hxx` - Asteroid entity with size tiers, point values, and split behavior.
  - `projectile.hxx` - Projectile entity with lifetime and movement.
  - `powerup.hxx` - Power-up entity with type, lifetime, and color metadata.
  - `game.hxx` - Main game engine managing entity collections, spawning, collisions, and game state.
- `src/game.cxx`
  - `Game` implementation, update loop, input handling, collision resolution, and reset logic.
- `examples/main.cxx`
  - Entry point and rendering layer using SDL2.
- `CMakeLists.txt`
  - Build configuration for the library and executable.

## Core Architecture

### Entity Model

The engine is centered around an `Entity` base class that provides common functionality:

- `position` and `velocity`
- `radius` for circular collision detection
- `active` state for lifecycle management
- `update(deltaTime)` to apply movement
- `collidesWith(other)` for circular collision tests
- `wrapScreen(width, height)` for horizontal screen wrapping

Derived entity classes include:

- `Starship` - player-controlled rocket
- `Asteroid` - falling rocks that split into smaller asteroids
- `Projectile` - bullets fired upward
- `PowerUp` - collectible bonuses with timed lifetime

### Game Class

`starship::Game` is the main engine. It contains:

- `Starship player`
- `std::vector<Asteroid> asteroids`
- `std::vector<Projectile> projectiles`
- `std::vector<PowerUp> powerUps`
- score, level, width, height
- random number generator for spawning
- timers for spawn frequency, shooting cooldown, and active power-up effects
- `bool gameOver`

Key responsibilities:

- `update(deltaTime)` - advances game simulation each frame
- `handleInput(input, deltaTime)` - responds to player controls
- `spawnAsteroids(int)` / `spawnAsteroid(...)` - continuous asteroid generation
- `spawnPowerUp(...)` - creates collectible power-ups with random type
- `shootProjectile()` - normal and multi-shot firing
- `checkCollisions()` - resolves projectile/asteroid, player/power-up, and player/asteroid collisions
- `removeInactiveEntities()` - cleans up dead objects
- `reset()` - restarts game state

### Gameplay Flow

The engine follows a deterministic frame update:

1. If the game is over, updates stop.
2. Update shoot cooldown.
3. Update player state, apply drag, and constrain horizontal movement.
4. Update all asteroids, deactivate those falling off-screen.
5. Update projectiles, deactivate out-of-bounds bullets.
6. Update power-ups and wrap them horizontally.
7. Update active power-up timers.
8. Resolve collisions and spawn new game objects.
9. Remove inactive entities from vectors.
10. Spawn new asteroids continuously.
11. Advance the level when all asteroids are cleared.
12. Set `gameOver` when the player is destroyed and out of health.

## Architecture Diagrams

### Component Diagram

```
+-------------+        +-------------+         +-------------+
|   User /    |------->|    Game     |-------->|  Renderer   |
| Input /     |        |   Engine    |         |  (SDL2)     |
| Controls    |        +-------------+         +-------------+
+-------------+                |
                               |
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
        |------------------|
        | position         |
        | velocity         |
        | radius           |
        | active           |
        |------------------|
        | update()         |
        | collidesWith()   |
        | wrapScreen()     |
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
                  | player |      
                  | vectors|      
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
| Update shoot cooldown   |
+-------------------------+
        |
        v
+-------------------------+
| Update player state     |
+-------------------------+
        |
        v
+-------------------------+
| Update asteroids        |
+-------------------------+
        |
        v
+-------------------------+
| Update projectiles      |
+-------------------------+
        |
        v
+-------------------------+
| Update power-ups        |
+-------------------------+
        |
        v
+-------------------------+
| Check collisions        |
+-------------------------+
        |
        v
+-------------------------+
| Remove inactive entities |
+-------------------------+
        |
        v
+-------------------------+
| Spawn new asteroids     |
+-------------------------+
        |
        v
+-------------------------+
| Advance level if clear  |
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

## Entity Behavior

### Starship

- Fixed bottom position in `applyBoundaries(width, height)`
- Horizontal movement only (`moveLeft`, `moveRight`, `stopMoving`)
- Optional thrust with a direction from `rotation`
- Drag reduces velocity over time
- Health tracked by `health`; `takeDamage()` deactivates when health reaches zero
- `restoreHealth()` supports extra life power-ups

### Asteroid

- Three sizes: `LARGE`, `MEDIUM`, `SMALL`
- Radii: 20, 12, and 6
- `getPoints()` returns score values per size
- `canSplit()` and `getNextSize()` determine fragmenting behavior
- When destroyed by a projectile, large/medium asteroids split into two smaller asteroids

### Projectile

- Lifetime-limited to 2 seconds
- Straight upward movement
- Deactivates on leaving the visible screen

### PowerUp

- Four types: `SHIELD`, `MULTI_SHOT`, `RAPID_FIRE`, `EXTRA_LIFE`
- Lifetime of 10 seconds on-screen
- Colored metadata is available for rendering
- Each power-up drifts downward with optional horizontal velocity

## Collision and Power-Up Systems

### Collision handling

- `Entity::collidesWith()` uses circular collision via distance and radii
- Projectiles vs asteroids:
  - projectiles deactivate on hit
  - asteroids deactivate, award score, optionally spawn a power-up
  - asteroids split if not small
- Player vs power-ups:
  - collects power-up and activates its ability
- Player vs asteroid:
  - if not shielded, damage is applied
  - player respawns if health remains
  - asteroid is removed

### Power-up effects

- `SHIELD` - temporary invincibility
- `MULTI_SHOT` - fires three projectiles in a spread
- `RAPID_FIRE` - decreases shoot cooldown by 50%
- `EXTRA_LIFE` - restores health, capped at 5 lives
- All active effects last 8 seconds

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
