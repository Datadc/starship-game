#ifndef ASTEROID_HXX
#define ASTEROID_HXX

#include "entity.hxx"
#include <vector>
#include <random>
#include <cmath>

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
    float rotation;
    std::vector<Vector2D> shapePoints;

    static constexpr float PI = 3.14159265358979323846f;

    static float getRandomRotationSpeed(std::mt19937& rng) {
        std::uniform_real_distribution<float> speedDist(20.0f, 60.0f);
        float speed = speedDist(rng);
        if (std::uniform_int_distribution<int>(0, 1)(rng) == 0) {
            speed = -speed;
        }
        return speed;
    }

    static std::vector<Vector2D> generateShape(Size s, std::mt19937& rng) {
        int vertexCount;
        switch (s) {
            case Size::LARGE:  vertexCount = 10; break;
            case Size::MEDIUM: vertexCount = 8; break;
            case Size::SMALL:  vertexCount = 6; break;
            default:           vertexCount = 8; break;
        }

        float baseRadius = getRadiusForSize(s);
        std::uniform_real_distribution<float> jitter(0.65f, 1.15f);

        std::vector<Vector2D> shape;
        shape.reserve(vertexCount);
        for (int i = 0; i < vertexCount; ++i) {
            float angle = 2.0f * PI * i / vertexCount;
            float radius = baseRadius * jitter(rng);
            shape.emplace_back(std::cos(angle) * radius, std::sin(angle) * radius);
        }
        return shape;
    }

public:
    // Default constructor - places asteroid near center with default velocity
    Asteroid()
        : Entity(Vector2D(400.0f, 300.0f), getRadiusForSize(Size::LARGE)),
          size(Size::LARGE), rotationSpeed(45.0f), rotation(0.0f),
          shapePoints({
              { -18.0f, -5.0f }, { -12.0f, 10.0f }, { 0.0f, 18.0f },
              { 12.0f, 12.0f }, { 18.0f, 0.0f }, { 10.0f, -12.0f },
              { -8.0f, -18.0f }
          }) {
        velocity = Vector2D(50.0f, 50.0f);
    }

    Asteroid(const Vector2D& pos, const Vector2D& vel, Size s, std::mt19937& rng)
        : Entity(pos, getRadiusForSize(s)),
          size(s),
          rotationSpeed(getRandomRotationSpeed(rng)),
          rotation(std::uniform_real_distribution<float>(0.0f, 360.0f)(rng)),
          shapePoints(generateShape(s, rng)) {
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
    float getRotation() const { return rotation; }
    const std::vector<Vector2D>& getShape() const { return shapePoints; }

    // Update position with wrapping (screen bounds managed externally)
    void update(float deltaTime) override {
        Entity::update(deltaTime);
        rotation += rotationSpeed * deltaTime;
        if (rotation >= 360.0f) rotation -= 360.0f;
        if (rotation < 0.0f) rotation += 360.0f;
    }
};

} // namespace starship

#endif // ASTEROID_HXX
