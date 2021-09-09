#include "geom/Transform3D.hh"

#include <array>
#include <cmath>

NuGeom::Rotation3D::Rotation3D(const Vector3D &axis, double angle)
    : m_axis{axis}, m_angle{angle} {}

NuGeom::Rotation3D::Rotation3D(double psi, double theta, double phi, Euler euler) {
    std::array<double, 9> rotmat1{}, rotmat2{}, rotmat3{};
    switch(euler) {
        case Euler::ZYX:
            rotmat1 = {1, 0, 0, 0, cos(phi), -sin(phi), 0, sin(phi), cos(phi)};
            rotmat2 = {cos(theta), 0, sin(theta), 0, 1, 0, -sin(theta), 0, cos(theta)};
            rotmat3 = {cos(psi), -sin(psi), 0, sin(psi), cos(psi), 0, 0, 0, 1};
            break;
        case Euler::ZXZ:
            rotmat1 = {cos(phi), -sin(phi), 0, sin(phi), cos(phi), 0, 0, 0, 1};
            rotmat2 = {cos(theta), 0, sin(theta), 0, 1, 0, -sin(theta), 0, cos(theta)};
            rotmat3 = {cos(psi), -sin(psi), 0, sin(psi), cos(psi), 0, 0, 0, 1};
            break;
    }

    std::array<double, 9> rotmat{};
    for(size_t i = 0; i < 3; ++i) {
        for(size_t j = 0; j < 3; ++j) {
            for(size_t k = 0; k < 3; ++k) {
                for(size_t l = 0; l < 3; ++l) {
                    rotmat[3*i+l] += rotmat1[3*i+j]*rotmat2[3*j+k]*rotmat3[3*k+l];
                }
            }
        }
    }

    m_angle = acos((rotmat[0]+rotmat[4]+rotmat[8]-1)/2.0);
    m_axis = {(rotmat[7]-rotmat[5])/(2*sin(m_angle)),
              (rotmat[6]-rotmat[2])/(2*sin(m_angle)),
              (rotmat[3]-rotmat[1])/(2*sin(m_angle))};
}

NuGeom::Vector3D NuGeom::Rotation3D::Apply(const Vector3D &in) const {
    return in*cos(m_angle)+m_axis.Cross(in)*sin(m_angle)+m_axis*m_axis.Dot(in)*(1-cos(m_angle));
}

NuGeom::Rotation3D NuGeom::operator*(const NuGeom::Rotation3D &r1, const NuGeom::Rotation3D &r2) {
    double cosa = cos(r1.m_angle/2.0);
    double sina = sin(r1.m_angle/2.0);
    double cosb = cos(r2.m_angle/2.0);
    double sinb = sin(r2.m_angle/2.0);
    NuGeom::Rotation3D result;
    result.m_angle = 2.0*acos(cosa*cosb-sina*sinb*r1.m_axis.Dot(r2.m_axis));
    result.m_axis = sina*cosb*r1.m_axis + cosa*sinb*r2.m_axis + sina*sinb*r1.m_axis.Cross(r2.m_axis);
    result.m_axis /= sin(result.m_angle/2.0);

    return result;
}
