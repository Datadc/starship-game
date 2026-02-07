#ifndef STARSHIP_GAME_HXX
#define STARSHIP_GAME_HXX

#include "starship.hxx"
#include "asteroid.hxx"
#include "projectile.hxx"
#include <vector>
#include <memory>
#include <random>

namespace starship {

class Game {
private:
    Starship player;
    std::vector<Asteroid> asteroids;
    std::vector<Projectile> projectiles;
    
    int score;
    int level;
    float width;
    float height;
    
    std::mt19937 rng;
    
    float shootCooldown;
    const float shootDelay = 0.3f;
    
    float spawnTimer;
    const float spawnInterval = 2.0f;  // Spawn new asteroid every 2 seconds
    
    bool gameOver;

public:
    Game(float width, float height);
    
    void update(float deltaTime);
    void handleInput(char input, float deltaTime);
    
    void spawnAsteroids(int count);
    void spawnAsteroid(const Vector2D& pos, const Vector2D& vel, Asteroid::Size size);
    void shootProjectile();
    
    void checkCollisions();
    void removeInactiveEntities();
    
    // Getters for rendering
    const Starship& getPlayer() const { return player; }
    Starship& getPlayer() { return player; }
    const std::vector<Asteroid>& getAsteroids() const { return asteroids; }
    const std::vector<Projectile>& getProjectiles() const { return projectiles; }
    
    int getScore() const { return score; }
    int getLevel() const { return level; }
    bool isGameOver() const { return gameOver; }
    float getWidth() const { return width; }
    float getHeight() const { return height; }
    
    void reset();
};

} // namespace starship

#endif // STARSHIP_GAME_HPP