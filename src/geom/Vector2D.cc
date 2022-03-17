#include "geom/Vector2D.hh"

using NuGeom::Vector2D;

double Vector2D::Dot(const Vector2D &other) const {
    return m_vec[0]*other.m_vec[0] + m_vec[1]*other.m_vec[1];
}

Vector2D Vector2D::Unit() const {
    double norm = Norm();
    return {m_vec[0]/norm, m_vec[1]/norm};
}

Vector2D Vector2D::Max(const Vector2D &other) const {
    return {std::max(X(), other.X()), std::max(Y(), other.Y())};
}

double Vector2D::MaxComponent() const {
    return std::max(X(), Y());
}

Vector2D NuGeom::operator*(double scale, const Vector2D &vec) {
    return Vector2D{vec} *= scale;
}

Vector2D NuGeom::operator/(double scale, const Vector2D &vec) {
    return Vector2D{vec} /= scale;
}
