#ifndef STARSHIP_POWERUP_HXX
#define STARSHIP_POWERUP_HXX

#include "entity.hxx"

namespace starship {

class PowerUp : public Entity {
public:
    enum class Type {
        SHIELD,
        MULTI_SHOT,
        RAPID_FIRE,
        EXTRA_LIFE,
        SPEED_BOOST
    };

private:
    Type type;
    float lifetime;
    float maxLifetime;  // How long power-up stays on screen

public:
    PowerUp(const Vector2D& pos, Type t)
        : Entity(pos, 8.0f), type(t), lifetime(0.0f), maxLifetime(10.0f) {
        // Velocity will be set by the spawning method
    }

    void update(float deltaTime) override {
        Entity::update(deltaTime);
        lifetime += deltaTime;

        // Deactivate after lifetime expires
        if (lifetime > maxLifetime) {
            active = false;
        }
    }

    Type getType() const { return type; }
    float getLifetimeRatio() const { return lifetime / maxLifetime; }

    // Get color based on type (RGBA values)
    struct Color {
        int r, g, b, a;
    };
    
    Color getColor() const {
        switch (type) {
            case Type::SHIELD:      return {0, 255, 255, 255};    // Cyan
            case Type::MULTI_SHOT:  return {255, 0, 255, 255};    // Magenta
            case Type::RAPID_FIRE:  return {255, 255, 0, 255};    // Yellow
            case Type::EXTRA_LIFE:  return {0, 255, 0, 255};      // Green
            case Type::SPEED_BOOST: return {255, 165, 0, 255};    // Orange
            default:                return {255, 255, 255, 255};  // White
        }
    }
};

} // namespace starship

#endif // STARSHIP_POWERUP_HPP