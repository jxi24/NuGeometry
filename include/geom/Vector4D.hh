#pragma once

#include <array>
#include <cmath>

namespace NuGeom {

class Vector3D;

namespace Visualization {

class Vector4D {
    public:
        Vector4D() : m_vec{} {}
        Vector4D(double x, double y, double z, double w) : m_vec{x, y, z, w} {}
        Vector4D(std::array<double, 4> vec) : m_vec{vec} {}
        Vector4D(const Vector4D&) = default;
        Vector4D(Vector4D&&) = default;

        Vector4D& operator=(const Vector4D&) = default;
        Vector4D& operator=(Vector4D&&) = default;

        // const access
        const double& X() const { return m_vec[0]; }
        const double& Y() const { return m_vec[1]; }
        const double& Z() const { return m_vec[2]; }
        const double& W() const { return m_vec[3]; }
        const double& R() const { return m_vec[0]; }
        const double& G() const { return m_vec[1]; }
        const double& B() const { return m_vec[2]; }
        const double& A() const { return m_vec[3]; }

        // non-const access
        double& X() { return m_vec[0]; }
        double& Y() { return m_vec[1]; }
        double& Z() { return m_vec[2]; }
        double& W() { return m_vec[3]; }
        double& R() { return m_vec[0]; }
        double& G() { return m_vec[1]; }
        double& B() { return m_vec[2]; }
        double& A() { return m_vec[3]; }

        // Functions
        double Dot(const Vector4D&) const;
        Vector4D Cross(const Vector4D&) const;
        double Norm2() const { return Dot(*this); }
        double Norm() const { return sqrt(Norm2()); }
        Vector4D Unit() const;
        Vector4D Abs() const { return {std::abs(X()), std::abs(Y()), std::abs(Z()), std::abs(W())}; }
        Vector4D Max(const Vector4D& = Vector4D()) const;
        Vector3D To3D() const;
        double MaxComponent() const;

        // Operators
        friend Vector4D operator*(double, const Vector4D&);
        friend Vector4D operator/(double, const Vector4D&);
   
        bool operator==(const Vector4D &other) const {
            return m_vec == other.m_vec;
        }
        Vector4D& operator*=(double scale) {
            m_vec[0] *= scale;
            m_vec[1] *= scale;
            m_vec[2] *= scale;
            m_vec[3] *= scale;

            return *this;
        }
        Vector4D& operator/=(double scale) {
            return *this *= 1.0/scale;
        }
        Vector4D& operator+=(const Vector4D &other) {
            m_vec[0] += other.m_vec[0];
            m_vec[1] += other.m_vec[1];
            m_vec[2] += other.m_vec[2];
            m_vec[3] += other.m_vec[3];

            return *this;
        }
        Vector4D& operator-=(const Vector4D &other) {
            return *this += -other;
        }
        Vector4D operator*(double scale) const {
            return Vector4D{*this} *= scale;
        }
        double operator*(const Vector4D &other) const {
            return Dot(other);
        }
        Vector4D operator+(const Vector4D &other) const {
            return Vector4D{*this} += other;
        }
        Vector4D operator-(const Vector4D &other) const {
            return Vector4D{*this} -= other;
        }
        Vector4D operator-() const {
            return {-m_vec[0], -m_vec[1], -m_vec[2], -m_vec[3]};
        }

        template<typename OStream>
        friend OStream& operator<<(OStream &os, const Vector4D &vec) {
            os << "Vector4D(" << vec.X() << ", " << vec.Y() << ", " << vec.Z() << ", " << vec.W() << ")";
            return os;
        }

    private:
        std::array<double, 4> m_vec;
};

Vector4D operator*(double, const Vector4D&);
Vector4D operator/(double, const Vector4D&);

}
}
