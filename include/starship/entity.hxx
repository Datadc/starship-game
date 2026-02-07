#ifndef STARSHIP_ENTITY_HXX
#define STARSHIP_ENTITY_HXX

#include "Vector2D.hxx"

namespace starship {

class Entity {
protected:
    Vector2D position;
    Vector2D velocity;
    float radius;
    bool active;

public:
    Entity(const Vector2D& pos, float radius)
        : position(pos), velocity(0, 0), radius(radius), active(true) {}

    virtual ~Entity() = default;

    virtual void update(float deltaTime) {
        position += velocity * deltaTime;
    }

    // Getters
    const Vector2D& getPosition() const { return position; }
    const Vector2D& getVelocity() const { return velocity; }
    float getRadius() const { return radius; }
    bool isActive() const { return active; }

    // Setters
    void setPosition(const Vector2D& pos) { position = pos; }
    void setVelocity(const Vector2D& vel) { velocity = vel; }
    void setActive(bool state) { active = state; }

    // Collision detection
    bool collidesWith(const Entity& other) const {
        float dist = Vector2D::distance(position, other.position);
        return dist < (radius + other.radius);
    }

    // Screen wrapping
    void wrapScreen(float width, float height) {
        if (position.x < 0) position.x += width;
        if (position.x > width) position.x -= width;
        if (position.y < 0) position.y += height;
        if (position.y > height) position.y -= height;
    }
};

} // namespace starship

#endif // STARSHIP_ENTITY_HPP