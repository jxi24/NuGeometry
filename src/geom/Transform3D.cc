#include "geom/Transform3D.hh"
#include "geom/Ray.hh"

#include <array>
#include <cmath>
#include <iostream>

using NuGeom::Transform3D;
using NuGeom::Rotation3D;

constexpr std::array<double, 12> Transform3D::identity;

Transform3D::Transform3D(double xx, double xy, double xz, double tx,
                         double yx, double yy, double yz, double ty,
                         double zx, double zy, double zz, double tz) {
    m_mat = {xx, xy, xz, tx, yx, yy, yz, ty, zx, zy, zz, tz};
}

Transform3D::Transform3D(const Rotation3D &rot, const Translation3D &trans) {
    m_mat = rot.m_mat;
    m_mat[3] = trans.m_mat[3];
    m_mat[7] = trans.m_mat[7];
    m_mat[11] = trans.m_mat[11];
}

NuGeom::Vector3D Transform3D::Apply(const Vector3D &point) const {
    return {m_mat[0]*point.X() + m_mat[1]*point.Y() + m_mat[2]*point.Z() + m_mat[3],
            m_mat[4]*point.X() + m_mat[5]*point.Y() + m_mat[6]*point.Z() + m_mat[7],
            m_mat[8]*point.X() + m_mat[9]*point.Y() + m_mat[10]*point.Z() + m_mat[11]};
}

Transform3D Transform3D::Inverse() const {
    double detxx = m_mat[5]*m_mat[10] - m_mat[6]*m_mat[9];
    double detxy = m_mat[6]*m_mat[8] - m_mat[4]*m_mat[10];
    double detxz = m_mat[4]*m_mat[9] - m_mat[5]*m_mat[8];
    double det = m_mat[0]*detxx + m_mat[1]*detxy + m_mat[2]*detxz;
    if(det == 0) {
        std::cerr << "[WARNING] Transform3D::Inverse() has zero determinant\n";
        return Transform3D();
    }
    det = 1.0/det;
    detxx *= det;
    detxy *= det;
    detxz *= det;
    double detyx = (m_mat[2]*m_mat[9] - m_mat[1]*m_mat[10])*det;
    double detyy = (m_mat[0]*m_mat[10] - m_mat[2]*m_mat[8])*det;
    double detyz = (m_mat[1]*m_mat[8] - m_mat[0]*m_mat[9])*det;
    double detzx = (m_mat[1]*m_mat[6] - m_mat[2]*m_mat[5])*det;
    double detzy = (m_mat[2]*m_mat[4] - m_mat[0]*m_mat[6])*det;
    double detzz = (m_mat[0]*m_mat[5] - m_mat[1]*m_mat[4])*det;
    return {detxx, detyx, detzx, -detxx*m_mat[3]-detyx*m_mat[7]-detzx*m_mat[11],
            detxy, detyy, detzy, -detxy*m_mat[3]-detyy*m_mat[7]-detzy*m_mat[11],
            detxz, detyz, detzz, -detxz*m_mat[3]-detyz*m_mat[7]-detzz*m_mat[11]};
}

Transform3D Transform3D::operator*(const Transform3D &other) const {
    return {m_mat[0]*other.m_mat[0] + m_mat[1]*other.m_mat[4] + m_mat[2]*other.m_mat[8],
            m_mat[0]*other.m_mat[1] + m_mat[1]*other.m_mat[5] + m_mat[2]*other.m_mat[9],
            m_mat[0]*other.m_mat[2] + m_mat[1]*other.m_mat[6] + m_mat[2]*other.m_mat[10],
            m_mat[0]*other.m_mat[3] + m_mat[1]*other.m_mat[7] + m_mat[2]*other.m_mat[11] + m_mat[3],

            m_mat[4]*other.m_mat[0] + m_mat[5]*other.m_mat[4] + m_mat[6]*other.m_mat[8],
            m_mat[4]*other.m_mat[1] + m_mat[5]*other.m_mat[5] + m_mat[6]*other.m_mat[9],
            m_mat[4]*other.m_mat[2] + m_mat[5]*other.m_mat[6] + m_mat[6]*other.m_mat[10],
            m_mat[4]*other.m_mat[3] + m_mat[5]*other.m_mat[7] + m_mat[6]*other.m_mat[11] + m_mat[7],

            m_mat[8]*other.m_mat[0] + m_mat[9]*other.m_mat[4] + m_mat[10]*other.m_mat[8],
            m_mat[8]*other.m_mat[1] + m_mat[9]*other.m_mat[5] + m_mat[10]*other.m_mat[9],
            m_mat[8]*other.m_mat[2] + m_mat[9]*other.m_mat[6] + m_mat[10]*other.m_mat[10],
            m_mat[8]*other.m_mat[3] + m_mat[9]*other.m_mat[7] + m_mat[10]*other.m_mat[11] + m_mat[11]};
}

void Transform3D::Decompose(Scale3D &scale, Rotation3D &rot, Translation3D &trans) const {
    double sx = std::sqrt(m_mat[0]*m_mat[0] + m_mat[4]*m_mat[4] + m_mat[8]*m_mat[8]);
    double sy = std::sqrt(m_mat[1]*m_mat[1] + m_mat[5]*m_mat[5] + m_mat[9]*m_mat[9]);
    double sz = std::sqrt(m_mat[2]*m_mat[2] + m_mat[6]*m_mat[6] + m_mat[10]*m_mat[10]);

    scale.SetTransform({sx,  0,  0, 0,
                         0, sy,  0, 0,
                         0,  0, sz, 0});
    rot.SetTransform({m_mat[0]/sx, m_mat[1]/sy, m_mat[2]/sz, 0,
                      m_mat[4]/sx, m_mat[5]/sy, m_mat[6]/sz, 0,
                      m_mat[8]/sx, m_mat[9]/sy, m_mat[10]/sz, 0});
    trans.SetTransform({1, 0, 0, m_mat[3],
                        0, 1, 0, m_mat[7],
                        0, 0, 1, m_mat[11]});
}

NuGeom::Vector3D Transform3D::ApplyPoint(const Vector3D &point, const Transform3D &trans) {
    return trans.Apply(point);
}

NuGeom::Ray Transform3D::ApplyRay(const Ray &ray, const Transform3D &transform) {
    Scale3D scale;
    Rotation3D rot;
    Translation3D trans;
    transform.Decompose(scale, rot, trans);

    auto origin = rot.Apply(trans.Apply(ray.Origin()));
    auto direction = rot.Apply(ray.Direction());

    return {origin, direction};
}

Rotation3D::Rotation3D(const Vector3D &vec, double angle) : Transform3D() {
    // Ensure the vector is a unit vector
    auto axis = vec.Unit();
    double cosa = cos(angle);
    double sina = sin(angle);
    std::array<double, 12> mat =
        {cosa+axis.X()*axis.X()*(1-cosa), axis.X()*axis.Y()*(1-cosa) - axis.Z()*sina,
         axis.X()*axis.Z()*(1-cosa)+axis.Y()*sina, 0,
         axis.Y()*axis.X()*(1-cosa)+axis.Z()*sina, cosa+axis.Y()*axis.Y()*(1-cosa),
         axis.Y()*axis.Z()*(1-cosa)-axis.X()*sina, 0,
         axis.Z()*axis.X()*(1-cosa)-axis.Y()*sina, axis.Z()*axis.Y()*(1-cosa)+axis.X()*sina,
         cosa+axis.Z()*axis.Z()*(1-cosa), 0};
    SetTransform(mat);
}

Rotation3D::Rotation3D(const Transform3D &rot) {
    SetTransform(rot.GetTransform());
}

NuGeom::Translation3D::Translation3D(const Transform3D &trans) {
    SetTransform(trans.GetTransform());
}
