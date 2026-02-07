#include "starship/game.hxx"
#include <algorithm>
#include <cmath>

namespace starship {

Game::Game(float width, float height)
    : player(Vector2D(width / 2, height / 2)),
      score(0),
      level(1),
      width(width),
      height(height),
      rng(std::random_device{}()),
      shootCooldown(0.0f),
      spawnTimer(0.0f),
      gameOver(false) {
    spawnAsteroids(8);
}

void Game::update(float deltaTime) {
    if (gameOver) return;
    
    // Update shoot cooldown
    if (shootCooldown > 0) {
        shootCooldown -= deltaTime;
    }
    
    // Update player
    if (player.isActive()) {
        player.update(deltaTime);
        player.applyDrag(deltaTime);
        player.applyBoundaries(width, height);
    }
    
    // Update asteroids (remove if out of bounds)
    for (auto& asteroid : asteroids) {
        asteroid.update(deltaTime);
        // Deactivate if off bottom of screen
        if (asteroid.getPosition().y > height + 50) {
            asteroid.setActive(false);
        }
    }
    
    // Update projectiles
    for (auto& projectile : projectiles) {
        projectile.update(deltaTime);
        // Deactivate if off top of screen
        if (projectile.getPosition().y < -10) {
            projectile.setActive(false);
        }
    }
    
    checkCollisions();
    removeInactiveEntities();
    
    // Continuous asteroid spawning
    spawnTimer += deltaTime;
    if (spawnTimer >= spawnInterval) {
        spawnTimer = 0.0f;
        // Spawn 1-2 asteroids continuously, scaled by level
        int spawnCount = 1 + (level / 3);  // More asteroids as level increases
        spawnAsteroids(spawnCount);
    }
    
    // Check if all asteroids destroyed - advance level (bonus multiplier)
    if (asteroids.empty() && player.isActive()) {
        level++;
        spawnAsteroids(6 + level * 2);
    }
    
    // Check game over
    if (!player.isActive() && player.getHealth() <= 0) {
        gameOver = true;
    }
}

void Game::handleInput(char input, float deltaTime) {
    if (gameOver) {
        if (input == 'r' || input == 'R') {
            reset();
        }
        return;
    }
    
    switch (input) {
        case 'a': case 'A':
            player.moveLeft(150.0f);
            break;
        case 'd': case 'D':
            player.moveRight(150.0f);
            break;
        case 'w': case 'W':
            player.thrust(deltaTime);
            break;
        case ' ':
            if (shootCooldown <= 0) {
                shootProjectile();
                shootCooldown = shootDelay;
            }
            break;
    }
}

void Game::spawnAsteroids(int count) {
    std::uniform_real_distribution<float> xDist(0, width);
    std::uniform_real_distribution<float> speedDist(10.0f, 30.0f);
    
    for (int i = 0; i < count; i++) {
        // Spawn from top of screen, moving downward
        Vector2D pos(xDist(rng), -20.0f);
        
        // Velocity moves downward (positive y direction)
        float speed = speedDist(rng);
        float horizontalAngle = (static_cast<float>(rand()) / RAND_MAX) * 0.4f - 0.2f; // Slight horizontal drift
        Vector2D vel(std::sin(horizontalAngle) * speed, speed);
        
        spawnAsteroid(pos, vel, Asteroid::Size::LARGE);
    }
}

void Game::spawnAsteroid(const Vector2D& pos, const Vector2D& vel, Asteroid::Size size) {
    asteroids.emplace_back(pos, vel, size);
}

void Game::shootProjectile() {
    if (!player.isActive()) return;
    
    // Projectiles always shoot upward (negative y direction)
    Vector2D pos = player.getPosition();
    Vector2D vel(0.0f, -300.0f);  // Upward only, no side direction
    
    projectiles.emplace_back(pos, vel);
}

void Game::checkCollisions() {
    // Check projectile-asteroid collisions
    for (auto& projectile : projectiles) {
        if (!projectile.isActive()) continue;
        
        for (auto& asteroid : asteroids) {
            if (!asteroid.isActive()) continue;
            
            if (projectile.collidesWith(asteroid)) {
                projectile.setActive(false);
                asteroid.setActive(false);
                score += asteroid.getPoints();
                
                // Split asteroid if possible
                if (asteroid.canSplit()) {
                    Asteroid::Size nextSize = asteroid.getNextSize();
                    std::uniform_real_distribution<float> angleDist(-0.5f, 0.5f);
                    
                    for (int i = 0; i < 2; i++) {
                        Vector2D vel = asteroid.getVelocity();
                        float angle = std::atan2(vel.y, vel.x) + angleDist(rng);
                        float speed = vel.length() * 1.2f;
                        Vector2D newVel(std::cos(angle) * speed, std::sin(angle) * speed);
                        spawnAsteroid(asteroid.getPosition(), newVel, nextSize);
                    }
                }
                break;
            }
        }
    }
    
    // Check player-asteroid collisions
    if (player.isActive()) {
        for (auto& asteroid : asteroids) {
            if (!asteroid.isActive()) continue;
            
            if (player.collidesWith(asteroid)) {
                player.takeDamage();
                asteroid.setActive(false);
                
                if (player.getHealth() > 0) {
                    player.respawn(Vector2D(width / 2, height / 2));
                }
                break;
            }
        }
    }
}

void Game::removeInactiveEntities() {
    asteroids.erase(
        std::remove_if(asteroids.begin(), asteroids.end(),
            [](const Asteroid& a) { return !a.isActive(); }),
        asteroids.end()
    );
    
    projectiles.erase(
        std::remove_if(projectiles.begin(), projectiles.end(),
            [](const Projectile& p) { return !p.isActive(); }),
        projectiles.end()
    );
}

void Game::reset() {
    player = Starship(Vector2D(width / 2, height / 2));
    asteroids.clear();
    projectiles.clear();
    score = 0;
    level = 1;
    shootCooldown = 0.0f;
    spawnTimer = 0.0f;
    gameOver = false;
    spawnAsteroids(8);
}

} // namespace starship