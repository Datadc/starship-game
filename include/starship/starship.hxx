#ifndef STARSHIP_STARSHIP_HXX
#define STARSHIP_STARSHIP_HXX

#include "entity.hxx"
#include <cmath>

namespace starship {

class Starship : public starship::Entity {
private:
    float rotation;      // Angle in radians
    float thrustPower;
    int health;

public:
    Starship(const Vector2D& pos)
        : Entity(pos, 1.0f), rotation(0.0f), thrustPower(50.0f), health(3) {}

    void rotateLeft(float deltaTime) {
        rotation -= 3.0f * deltaTime;
    }

    void rotateRight(float deltaTime) {
        rotation += 3.0f * deltaTime;
    }

    void moveLeft(float speed) {
        // Move left (negative x direction)
        velocity.x = -speed;
    }

    void moveRight(float speed) {
        // Move right (positive x direction)
        velocity.x = speed;
    }

    void stopMoving() {
        velocity.x = 0;
    }

    void thrust(float deltaTime) {
        // Apply thrust in the direction we're facing
        Vector2D thrustDir(std::cos(rotation), std::sin(rotation));
        velocity += thrustDir * thrustPower * deltaTime;
        
        // Cap maximum speed
        float maxSpeed = 40.0f;
        if (velocity.length() > maxSpeed) {
            velocity = velocity.normalized() * maxSpeed;
        }
    }

    void applyDrag(float deltaTime) {
        // Slow down over time
        velocity = velocity * (1.0f - 0.5f * deltaTime);
    }

    void applyBoundaries(float width, float height) {
        // Constrain to left/right boundaries
        if (position.x < radius) position.x = radius;
        if (position.x > width - radius) position.x = width - radius;
        // Keep rocket at bottom of screen (fixed y position)
        position.y = height - 30.0f;
        // Only allow horizontal velocity
        velocity.y = 0;
    }

    Vector2D getForwardDirection() const {
        return Vector2D(std::cos(rotation), std::sin(rotation));
    }

    float getRotation() const { return rotation; }
    int getHealth() const { return health; }
    
    void takeDamage() {
        health--;
        if (health <= 0) {
            active = false;
        }
    }

    void respawn(const Vector2D& pos) {
        position = pos;
        velocity = Vector2D(0, 0);
        rotation = 0;
        active = true;
    }
};

} // namespace starship

#endif // STARSHIP_STARSHIP_HPP