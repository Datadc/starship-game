#ifndef STARSHIP_VECTOR2D_HXX
#define STARSHIP_VECTOR2D_HXX

#include <cmath>

namespace starship {

struct Vector2D {
    float x;
    float y;

    Vector2D() : x(0.0f), y(0.0f) {}
    Vector2D(float x, float y) : x(x), y(y) {}

    // Vector addition
    Vector2D operator+(const Vector2D& other) const {
        return Vector2D(x + other.x, y + other.y);
    }

    // Vector subtraction
    Vector2D operator-(const Vector2D& other) const {
        return Vector2D(x - other.x, y - other.y);
    }

    // Scalar multiplication
    Vector2D operator*(float scalar) const {
        return Vector2D(x * scalar, y * scalar);
    }

    // Compound addition
    Vector2D& operator+=(const Vector2D& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    // Length (magnitude) of the vector
    float length() const {
        return std::sqrt(x * x + y * y);
    }

    // Normalize the vector
    Vector2D normalized() const {
        float len = length();
        if (len > 0) {
            return Vector2D(x / len, y / len);
        }
        return Vector2D(0, 0);
    }

    // Distance between two points
    static float distance(const Vector2D& a, const Vector2D& b) {
        return (b - a).length();
    }
};

} // namespace starship

#endif // STARSHIP_VECTOR2D_HPP