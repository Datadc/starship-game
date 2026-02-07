#ifndef STARSHIP_PROJECTILE_HXX
#define STARSHIP_PROJECTILE_HXX

#include "entity.hxx"

namespace starship {

class Projectile : public starship::Entity {
private:
    float lifetime;
    float maxLifetime;

public:
    Projectile(const Vector2D& pos, const Vector2D& vel)
        : Entity(pos, 0.3f), lifetime(0.0f), maxLifetime(2.0f) {
        velocity = vel;
    }

    void update(float deltaTime) override {
        Entity::update(deltaTime);
        lifetime += deltaTime;
        
        // Deactivate after maximum lifetime
        if (lifetime > maxLifetime) {
            active = false;
        }
    }
};

} // namespace starship

#endif // STARSHIP_PROJECTILE_HPP