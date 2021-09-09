#pragma once

#include "geom/Vector3D.hh"

namespace NuGeom {

class Transform3D {
    public:
        Transform3D() = default;
        virtual ~Transform3D() = default;

        virtual Vector3D Apply(const Vector3D&) const = 0;
};

class Rotation3D : public Transform3D {
    public:
        enum class  Euler {
            ZYX,
            ZXZ,
        };
        Rotation3D() = default;
        Rotation3D(const Vector3D&, double);
        Rotation3D(double, double, double, Euler=Euler::ZYX);

        Vector3D Apply(const Vector3D&) const;

        friend Rotation3D operator*(const Rotation3D&, const Rotation3D&);

    private:
        Vector3D m_axis{};
        double m_angle{};
};

Rotation3D operator*(const Rotation3D&, const Rotation3D&);

class Translation3D : public Transform3D {
    public:
        Translation3D(const Vector3D &vec) : m_vec{vec} {}
        Translation3D(double x, double y, double z) : m_vec{x, y, z} {}

        Vector3D Apply(const Vector3D &vec) const {
            return vec + m_vec;
        }

    private:
        Vector3D m_vec;
};

}
