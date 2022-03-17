#include "geom/Shape.hh"
#include "geom/Vector2D.hh"
#include "pugixml.hpp"

NuGeom::Location NuGeom::Shape::Contains(const Vector3D &point) const {
    double dist = SignedDistance(point);
    if(dist < 0) return NuGeom::Location::kInterior;
    else if(dist > 0) return NuGeom::Location::kExterior;
    else return NuGeom::Location::kSurface;
}

NuGeom::Vector3D NuGeom::Shape::TransformPoint(const Vector3D &point) const {
    return m_rotation.Apply(m_translation.Apply(point)); 
}

// TODO: Do this correctly!!!!
std::unique_ptr<NuGeom::Shape> NuGeom::CombinedShape::Construct(const pugi::xml_node &) {
    auto box1 = std::make_shared<NuGeom::Box>(); 
    auto box2 = std::make_shared<NuGeom::Box>(); 
    return std::make_unique<NuGeom::CombinedShape>(box1, box2, NuGeom::ShapeBinaryOp::kUnion); 
}

double NuGeom::CombinedShape::SignedDistance(const Vector3D &in_point) const {
    auto point = TransformPoint(in_point);
    double sdf1 = m_left -> SignedDistance(point);
    double sdf2 = m_right -> SignedDistance(point);
    double distance{};
    switch(m_op) {
        case ShapeBinaryOp::kUnion:
            distance = std::min(sdf1, sdf2);
            break;
        case ShapeBinaryOp::kIntersect:
            distance = std::max(sdf1, sdf2);
            break;
        case ShapeBinaryOp::kSubtraction:
            distance = std::max(-sdf1, sdf2);
            break;
    }

    return distance;
}

double NuGeom::CombinedShape::Volume() const {
    return 0;
}

std::unique_ptr<NuGeom::Shape> NuGeom::Box::Construct(const pugi::xml_node &node) {
    // Load the box parameters
    double x = node.attribute("x").as_double();
    double y = node.attribute("y").as_double();
    double z = node.attribute("z").as_double();
    Vector3D params(x, y, z);

    // Convert the units
    std::string unit = node.attribute("unit").value();
    if(unit == "m") {
        params *= 100; 
    } else if(unit == "mm") {
        params /= 10;
    }

    return std::make_unique<NuGeom::Box>(params);
}

double NuGeom::Box::SignedDistance(const Vector3D &in_point) const {
    auto point = TransformPoint(in_point);
    Vector3D q = point.Abs() - m_params;
    return q.Max().Norm() + std::min(q.MaxComponent(), 0.0);
}

std::unique_ptr<NuGeom::Shape> NuGeom::Sphere::Construct(const pugi::xml_node &node) {
    // Load the box parameters
    double radius = node.attribute("r").as_double();

    // Convert the units
    std::string unit = node.attribute("unit").value();
    if(unit == "m") {
        radius *= 100; 
    } else if(unit == "mm") {
        radius /= 10;
    }

    return std::make_unique<NuGeom::Sphere>(radius);
}

double NuGeom::Sphere::SignedDistance(const Vector3D &in_point) const {
    auto point = TransformPoint(in_point);
    return point.Norm() - m_radius;
}

// TODO: Handle deltaphi and rmin??
std::unique_ptr<NuGeom::Shape> NuGeom::Cylinder::Construct(const pugi::xml_node &node) {
    // Load the box parameters
    double radius = node.attribute("rmax").as_double();
    double height = node.attribute("z").as_double();

    // Convert the units
    std::string unit = node.attribute("unit").value();
    if(unit == "m") {
        height *= 100;
        radius *= 100; 
    } else if(unit == "mm") {
        radius /= 10;
        height /= 10;
    }

    return std::make_unique<NuGeom::Cylinder>(radius, height);
}

double NuGeom::Cylinder::SignedDistance(const Vector3D &in_point) const {
    auto point = TransformPoint(in_point);
    Vector2D q = Vector2D(Vector2D(point.X(), point.Y()).Norm(), std::abs(point.Z())) - Vector2D(m_radius, m_height);
    return q.Max().Norm() + std::min(q.MaxComponent(), 0.0);
}
