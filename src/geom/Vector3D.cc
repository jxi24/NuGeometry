#include "geom/Vector3D.hh"

using NuGeom::Vector3D;

double Vector3D::Dot(const Vector3D &other) const {
    return m_vec[0]*other.m_vec[0] + m_vec[1]*other.m_vec[1] + m_vec[2]*other.m_vec[2];
}

Vector3D Vector3D::Cross(const Vector3D &other) const {
    return {m_vec[1]*other.m_vec[2] - m_vec[2]*other.m_vec[1],
            m_vec[2]*other.m_vec[0] - m_vec[0]*other.m_vec[2],
            m_vec[0]*other.m_vec[1] - m_vec[1]*other.m_vec[0]};
}

Vector3D Vector3D::Unit() const {
    double norm = Norm();
    return {m_vec[0]/norm, m_vec[1]/norm, m_vec[2]/norm};
}

Vector3D NuGeom::operator*(double scale, const Vector3D &vec) {
    return Vector3D{vec} *= scale;
}

Vector3D NuGeom::operator/(double scale, const Vector3D &vec) {
    return Vector3D{vec} /= scale;
}
