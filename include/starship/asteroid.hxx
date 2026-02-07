#ifndef ASTEROID_HXX
#define ASTEROID_HXX

#include "entity.hxx"

namespace starship {

class Asteroid : public Entity {
public:
    enum class Size {
        LARGE,
        MEDIUM,
        SMALL
    };

private:
    Size size;
    float rotationSpeed;

public:
    // Default constructor - places asteroid near center with default velocity
    Asteroid()
        : Entity(Vector2D(400.0f, 300.0f), getRadiusForSize(Size::LARGE)),
          size(Size::LARGE), rotationSpeed(45.0f) {
        velocity = Vector2D(50.0f, 50.0f);
    }

    Asteroid(const Vector2D& pos, const Vector2D& vel, Size s)
        : Entity(pos, getRadiusForSize(s)), size(s), rotationSpeed(45.0f) {
        velocity = vel;
    }

    // Get radius based on size
    static float getRadiusForSize(Size s) {
        switch (s) {
            case Size::LARGE:  return 20.0f;
            case Size::MEDIUM: return 12.0f;
            case Size::SMALL:  return 6.0f;
            default:           return 20.0f;
        }
    }

    // Points awarded for destroying this asteroid
    int getPoints() const {
        switch (size) {
            case Size::LARGE:  return 20;
            case Size::MEDIUM: return 50;
            case Size::SMALL:  return 100;
            default:           return 0;
        }
    }

    bool canSplit() const {
        return size != Size::SMALL;
    }

    Size getNextSize() const {
        if (size == Size::LARGE) return Size::MEDIUM;
        if (size == Size::MEDIUM) return Size::SMALL;
        return Size::SMALL;
    }

    // Getters for position/velocity
    const Vector2D& getPosition() const { return position; }
    const Vector2D& getVelocity() const { return velocity; }

    // Update position with wrapping (screen bounds managed externally)
    void update(float deltaTime) override {
        Entity::update(deltaTime);
    }
};

} // namespace starship

#endif // ASTEROID_HXX
