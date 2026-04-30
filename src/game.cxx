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
      shieldTimer(0.0f),
      multiShotTimer(0.0f),
      rapidFireTimer(0.0f),
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
    
    // Update power-ups
    for (auto& powerUp : powerUps) {
        powerUp.update(deltaTime);
        powerUp.wrapScreen(width, height);  // Allow horizontal wrapping
        // Deactivate if off bottom of screen
        if (powerUp.getPosition().y > height + 50) {
            powerUp.setActive(false);
        }
    }
    
    // Update power-up timers
    if (shieldTimer > 0) shieldTimer -= deltaTime;
    if (multiShotTimer > 0) multiShotTimer -= deltaTime;
    if (rapidFireTimer > 0) rapidFireTimer -= deltaTime;
    if (speedBoostTimer > 0) speedBoostTimer -= deltaTime;
    
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
            player.moveLeft(hasSpeedBoost() ? 300.0f : 150.0f);
            break;
        case 'd': case 'D':
            player.moveRight(hasSpeedBoost() ? 300.0f : 150.0f);
            break;
        case 'w': case 'W':
            player.thrust(deltaTime, hasSpeedBoost() ? 2.0f : 1.0f);
            break;
        case ' ':
            float currentShootDelay = hasRapidFire() ? shootDelay * 0.5f : shootDelay;
            if (shootCooldown <= 0) {
                shootProjectile();
                shootCooldown = currentShootDelay;
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
        std::uniform_real_distribution<float> angleDist(-0.2f, 0.2f);
        float horizontalAngle = angleDist(rng); // Slight horizontal drift
        Vector2D vel(std::sin(horizontalAngle) * speed, speed);
        
        spawnAsteroid(pos, vel, Asteroid::Size::LARGE);
    }
}

void Game::spawnAsteroid(const Vector2D& pos, const Vector2D& vel, Asteroid::Size size) {
    asteroids.emplace_back(pos, vel, size, rng);
}

void Game::spawnPowerUp(const Vector2D& pos) {
    std::uniform_int_distribution<int> typeDist(0, 4);
    PowerUp::Type type = static_cast<PowerUp::Type>(typeDist(rng));
    powerUps.emplace_back(pos, type);
    
    // Give the last added power-up some velocity
    if (!powerUps.empty()) {
        std::uniform_real_distribution<float> horizontalVel(-15.0f, 15.0f);
        powerUps.back().setVelocity(Vector2D(horizontalVel(rng), 80.0f));  // Faster downward movement
    }
}

void Game::applyPowerUp(PowerUp::Type type) {
    const float effectDuration = 8.0f;  // 8 seconds
    
    switch (type) {
        case PowerUp::Type::SHIELD:
            shieldTimer = effectDuration;
            break;
        case PowerUp::Type::MULTI_SHOT:
            multiShotTimer = effectDuration;
            break;
        case PowerUp::Type::RAPID_FIRE:
            rapidFireTimer = effectDuration;
            break;
        case PowerUp::Type::SPEED_BOOST:
            speedBoostTimer = 5.0f;
            break;
        case PowerUp::Type::EXTRA_LIFE:
            if (player.getHealth() < 5) {  // Max 5 lives
                player.restoreHealth();
            }
            break;
    }
}

void Game::shootProjectile() {
    if (!player.isActive()) return;
    
    // Projectiles always shoot upward (negative y direction)
    Vector2D pos = player.getPosition();
    
    if (multiShotTimer > 0) {
        // Multi-shot: 3 projectiles in spread
        Vector2D velCenter(0.0f, -300.0f);
        Vector2D velLeft(-50.0f, -300.0f);
        Vector2D velRight(50.0f, -300.0f);
        
        projectiles.emplace_back(pos, velLeft);
        projectiles.emplace_back(pos, velCenter);
        projectiles.emplace_back(pos, velRight);
    } else {
        // Normal shot
        Vector2D vel(0.0f, -300.0f);
        projectiles.emplace_back(pos, vel);
    }
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
                
                // Chance to spawn power-up when asteroid is destroyed
                std::uniform_real_distribution<float> powerUpChance(0.0f, 1.0f);
                if (powerUpChance(rng) < 0.15f) {  // 15% chance
                    spawnPowerUp(asteroid.getPosition());
                }
                
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
    
    // Check player-power-up collisions
    if (player.isActive()) {
        for (auto& powerUp : powerUps) {
            if (!powerUp.isActive()) continue;
            
            if (player.collidesWith(powerUp)) {
                powerUp.setActive(false);
                applyPowerUp(powerUp.getType());
                break;
            }
        }
    }
    
    // Check player-asteroid collisions
    if (player.isActive() && !isShielded()) {
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
    
    powerUps.erase(
        std::remove_if(powerUps.begin(), powerUps.end(),
            [](const PowerUp& p) { return !p.isActive(); }),
        powerUps.end()
    );
}

void Game::reset() {
    player = Starship(Vector2D(width / 2, height / 2));
    asteroids.clear();
    projectiles.clear();
    powerUps.clear();
    score = 0;
    level = 1;
    shootCooldown = 0.0f;
    spawnTimer = 0.0f;
    shieldTimer = 0.0f;
    multiShotTimer = 0.0f;
    rapidFireTimer = 0.0f;
    gameOver = false;
    spawnAsteroids(8);
}

} // namespace starship