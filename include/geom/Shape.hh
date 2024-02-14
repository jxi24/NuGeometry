#pragma once


#include "geom/Vector3D.hh"
#include "geom/Transform3D.hh"

#include <cmath>
#include <functional>
#include <iostream>
#include <map>
#include <memory>

namespace pugi {
class xml_node;
}

namespace NuGeom {

class Ray;

enum class Location {
    kInterior,
    kSurface,
    kExterior
};

enum class ShapeBinaryOp {
    kUnion,
    kIntersect,
    kSubtraction
};

class Shape {
    public:
        Shape(const Rotation3D &rot = Rotation3D(), const Translation3D &trans = Translation3D())
            : m_rotation{rot.Inverse()}, m_translation{trans.Inverse()} {
            identity_transform = m_rotation.IsIdentity() && m_translation.IsIdentity();
        }
        virtual ~Shape() = default;

        /// Calculates if the shape contains the point
        ///@param point: The point to check the distance from the surface
        ///@return Location: Returns if the point is interior, surface, or exterior
        Location Contains(const Vector3D&) const;

        /// Calculates the signed distance a point is from the surface
        /// Negative values mean inside, positive values mean outside, and 0 means on the surface
        ///@param point: The point to check the distance from the surface
        ///@return double: The signed distance from the surface 
        virtual double SignedDistance(const Vector3D&) const = 0;

        /// Finds if a given ray intersects the shape 
        ///@param ray: The ray to check for an intersection
        ///@return double: The time that the intersection occurs at
        double Intersect(const Ray &in_ray) const;

        void SetRotation(const Rotation3D& rot) { m_rotation = rot.Inverse(); }
        void SetTranslation(const Translation3D &trans) { m_translation = trans.Inverse(); }
        virtual double Volume() const = 0;

    protected:
        Vector3D TransformPoint(const Vector3D&) const;
        Ray TransformRay(const Ray&) const;
        std::pair<double, double> SolveQuadratic(double, double, double) const;

    private:
        virtual double IntersectImpl(const Ray&) const = 0;
        Transform3D m_rotation;
        Transform3D m_translation;
        bool identity_transform{false};
};

class ShapeFactory {
    using Constructor = std::function<std::unique_ptr<Shape>(const pugi::xml_node&)>;

    static std::map<std::string, Constructor> &Registry() {
        static std::map<std::string, Constructor> registry;
        return registry;
    }

    public:
        static std::unique_ptr<Shape> Initialize(const std::string &name, const pugi::xml_node &node) {
            auto constructor = Registry().at(name);
            return constructor(node);
        }

        // TODO: Switch to using a logger!
        template<class Derived>
        static void Register(const std::string &name) {
            if(IsRegistered(name))
                std::cerr << name << " is already registered!\n";
            Registry()[name] = Derived::Construct;
        }

        static bool IsRegistered(const std::string &name) {
            return Registry().find(name) != Registry().end();
        }

        static void DisplayShapes() {
            std::cout << "Shapes:\n";
            for(const auto &registered : Registry())
                std::cout << "  - " << registered.first << "\n";
        }
};

template<class Derived>
class RegistrableShape {
    protected:
        RegistrableShape() = default;
        virtual ~RegistrableShape() {
            if(!m_registered)
                std::cerr << "Error registering shape\n";
        }

        static bool Register() {
            ShapeFactory::template Register<Derived>(Derived::Name());
            return true;
        }

    private:
        static const bool m_registered;
};
template<class Derived>
const bool RegistrableShape<Derived>::m_registered = RegistrableShape<Derived>::Register();

class CombinedShape : public Shape, RegistrableShape<CombinedShape> {
    public:
        CombinedShape(std::shared_ptr<Shape> left, std::shared_ptr<Shape> right,
                      ShapeBinaryOp op,
                      const Rotation3D &rotation = Rotation3D(),
                      const Translation3D &translation = Translation3D()) 
            : Shape(rotation, translation), m_left{std::move(left)}, m_right{std::move(right)}, m_op{op} {}

        static std::string Name() { return "CombinedShape"; }
        static std::unique_ptr<Shape> Construct(const pugi::xml_node &node);

        double SignedDistance(const Vector3D&) const override;
        double Volume() const override;

    private:
        double IntersectImpl(const Ray&) const override;
        std::shared_ptr<Shape> m_left, m_right;
        ShapeBinaryOp m_op;
};

class Box : public Shape, RegistrableShape<Box> {
    public:
        /// Initialize a box with one corner at (-x/2,-y/2,-z/2) and the other at (x/2,y/2,z/2)
        /// Then rotates the box, and translates the box
        ///@param dimensions: The width, depth, and height of the box
        ///@param rot: The rotation matrix of the box
        ///@param trans: The translation of the box from the origin
        Box(const Vector3D &size = Vector3D(1, 1, 1),
            const Rotation3D &rotation = Rotation3D(),
            const Translation3D &translation = Translation3D())
            : Shape(rotation, translation), m_params{size.X()/2, size.Y()/2, size.Z()/2} {}

        static std::string Name() { return "box"; }
        static std::unique_ptr<Shape> Construct(const pugi::xml_node &node);

        double SignedDistance(const Vector3D&) const override;
        double Volume() const override { return m_params.X()*m_params.Y()*m_params.Z()*8; }

    private:
        double IntersectImpl(const Ray&) const override;
        Vector3D m_params;
};

class Sphere : public Shape, RegistrableShape<Sphere> {
    public:
        /// Initialize a sphere center at the origin with the given radius
        /// Then rotates the sphere, and translates the sphere
        ///@param radius: The radius of the sphere
        ///@param rot: The rotation matrix of the sphere
        ///@param trans: The translation of the sphere from the origin
        Sphere(double radius = 1,
               const Rotation3D &rotation = Rotation3D(),
               const Translation3D &translation = Translation3D())
            : Shape(rotation, translation), m_radius{radius} {}

        static std::string Name() { return "orb"; }
        static std::unique_ptr<Shape> Construct(const pugi::xml_node &node);

        double SignedDistance(const Vector3D&) const override;
        double Volume() const override { return m_radius*m_radius*m_radius*4*M_PI/3.0; }

    private:
        double IntersectImpl(const Ray&) const override;
        double m_radius;
};

class Cylinder : public Shape, RegistrableShape<Cylinder> {
    public:
        /// Initialize a cylinder centered at the origin with the given radius and height
        /// Then rotates the cylinder, and translates the cylinder
        ///@param radius: The radius of the cylinder
        ///@param height: The height of the cylinder
        ///@param rot: The rotation matrix of the cylinder
        ///@param trans: The translation of the cylinder from the origin
        Cylinder(double radius = 1,
                 double height = 1,
                 const Rotation3D &rotation = Rotation3D(),
                 const Translation3D &translation = Translation3D())
            : Shape(rotation, translation), m_radius{radius}, m_height{height} {}

        static std::string Name() { return "tube"; }
        static std::unique_ptr<Shape> Construct(const pugi::xml_node &node);

        double SignedDistance(const Vector3D&) const override;
        double Volume() const override { return m_radius*m_radius*m_height*M_PI; }

    private:
        double IntersectImpl(const Ray&) const override;
        double m_radius;
        double m_height;
};

// TODO: Implement details
/*
class Cone : public Shape, RegistrableShape<Cone> {
    public:
        /// Initialize a cone centered at the origin with the given normal vector
        /// Then rotates the cone, and translates the cone
        ///@param normal: The surface normal to the cone
        ///@param rot: The rotation matrix of the cone
        ///@param trans: The translation of the cone from the origin
        Cone(Vector3D normal = Vector3D(),
              const Rotation3D &rotation = Rotation3D(),
              const Translation3D &translation = Translation3D())
            : Shape(rotation, translation), m_normal{normal.Unit()} {}

        static std::string Name() { return "cone"; }
        static std::unique_ptr<Shape> Construct(const pugi::xml_node &node);

        double SignedDistance(const Vector3D&) const override;

    private:
        Vector3D m_normal;

};*/

}
