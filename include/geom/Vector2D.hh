#pragma once

#include <array>
#include <cmath>

namespace NuGeom {

class Vector2D {
    public:
        Vector2D() : m_vec{} {}
        Vector2D(double x, double y) : m_vec{x, y} {}
        Vector2D(std::array<double, 2> vec) : m_vec{vec} {}
        Vector2D(const Vector2D&) = default;
        Vector2D(Vector2D&&) = default;

        Vector2D& operator=(const Vector2D&) = default;
        Vector2D& operator=(Vector2D&&) = default;

        // const access
        const double& X() const { return m_vec[0]; }
        const double& Y() const { return m_vec[1]; }

        // non-const access
        double& X() { return m_vec[0]; }
        double& Y() { return m_vec[1]; }

        // Functions
        double Dot(const Vector2D&) const;
        Vector2D Cross(const Vector2D&) const;
        double Norm2() const { return Dot(*this); }
        double Norm() const { return sqrt(Norm2()); }
        Vector2D Unit() const;
        Vector2D Abs() const { return {std::abs(X()), std::abs(Y())}; }
        Vector2D Max(const Vector2D& = Vector2D()) const;
        double MaxComponent() const;

        // Operators
        friend Vector2D operator*(double, const Vector2D&);
        friend Vector2D operator/(double, const Vector2D&);
   
        bool operator==(const Vector2D &other) const {
            return m_vec == other.m_vec;
        }
        Vector2D& operator*=(double scale) {
            m_vec[0] *= scale;
            m_vec[1] *= scale;

            return *this;
        }
        Vector2D& operator/=(double scale) {
            return *this *= 1.0/scale;
        }
        Vector2D& operator+=(const Vector2D & other) {
            m_vec[0] += other.m_vec[0];
            m_vec[1] += other.m_vec[1];

            return *this;
        }
        Vector2D& operator-=(const Vector2D & other) {
            return *this += -other;
        }
        Vector2D operator*(double scale) const {
            return Vector2D{*this} *= scale;
        }
        Vector2D operator+(const Vector2D &other) const {
            return Vector2D{*this} += other;
        }
        Vector2D operator-(const Vector2D &other) const {
            return Vector2D{*this} -= other;
        }
        Vector2D operator-() const {
            return {-m_vec[0], -m_vec[1]};
        }

    private:
        std::array<double, 2> m_vec;
};

Vector2D operator*(double, const Vector2D&);
Vector2D operator/(double, const Vector2D&);

}
