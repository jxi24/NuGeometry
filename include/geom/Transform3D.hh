#pragma once

#include "geom/Vector3D.hh"
#include <string>

namespace NuGeom {

class Rotation3D;
class Translation3D;
class Scale3D;

class Transform3D {
    public:
        Transform3D() : m_mat{identity} {}
        Transform3D(const std::array<double, 12>& transform) : m_mat{transform} {}
        Transform3D(double, double, double, double,
                    double, double, double, double,
                    double, double, double, double);
        Transform3D(const Rotation3D&, const Translation3D&);
        Transform3D(const Transform3D&) = default;
        Transform3D &operator=(const Transform3D&) = default;
        virtual ~Transform3D() = default;

        Vector3D Apply(const Vector3D&) const;

        Transform3D Inverse() const;
        Transform3D operator*(const Transform3D&) const;

        void Decompose(Scale3D&, Rotation3D&, Translation3D&) const;

        template<typename OStream>
        friend OStream& operator<<(OStream &os, const Transform3D &transform) {
            std::string output = "Transform3D(";
            for(const auto &elm : transform.m_mat) {
                output += std::to_string(elm) + ", ";
            }
            output = output.substr(0, output.size()-2);
            os << output << ")";
            return os;
        }

    protected:
        void SetTransform(const std::array<double, 12> &trans) { m_mat = trans; }
        std::array<double, 12> GetTransform() const { return m_mat; }

    private:
        std::array<double, 12> m_mat;
        static constexpr std::array<double, 12> identity{1, 0, 0, 0,
                                                         0, 1, 0, 0,
                                                         0, 0, 1, 0};
};

class Scale3D : public Transform3D {
    public:
        Scale3D() = default;
        Scale3D(const Vector3D&);
        Scale3D(double x, double y, double z);
};

class ScaleX3D : public Scale3D {
    public:
        ScaleX3D(double x) : Scale3D(x, 0, 0) {}
};

class ScaleY3D : public Scale3D {
    public:
        ScaleY3D(double y) : Scale3D(0, y, 0) {}
};

class ScaleZ3D : public Scale3D {
    public:
        ScaleZ3D(double z) : Scale3D(0, 0, z) {}
};

class Rotation3D : public Transform3D {
    public:
        Rotation3D() = default;
        Rotation3D(const Vector3D&, double);
};

class RotationX3D : public Rotation3D {
    public:
        RotationX3D(double theta) : Rotation3D({1, 0, 0}, theta) {}
};

class RotationY3D : public Rotation3D {
    public:
        RotationY3D(double theta) : Rotation3D({0, 1, 0}, theta) {}
};

class RotationZ3D : public Rotation3D {
    public:
        RotationZ3D(double theta) : Rotation3D({0, 0, 1}, theta) {}
};

class Translation3D : public Transform3D {
    public:
        Translation3D() = default;
        Translation3D(const Vector3D &vec) : Transform3D(1, 0, 0, vec.X(),
                                                         0, 1, 0, vec.Y(),
                                                         0, 0, 1, vec.Z()) {}
        Translation3D(double x, double y, double z) : Transform3D(1, 0, 0, x,
                                                                  0, 1, 0, y,
                                                                  0, 0, 1, z) {}
};

class TranslationX3D : public Translation3D {
    public:
        TranslationX3D(double x) : Translation3D(x, 0, 0) {}
};

class TranslationY3D : public Translation3D {
    public:
        TranslationY3D(double y) : Translation3D(0, y, 0) {}
};

class TranslationZ3D : public Translation3D {
    public:
        TranslationZ3D(double z) : Translation3D(0, 0, z) {}
};

}
