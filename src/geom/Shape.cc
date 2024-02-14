#include "geom/Shape.hh"
#include "geom/Vector2D.hh"
#include "geom/Vector3D.hh"
#include "geom/Ray.hh"
#include "pugixml.hpp"
#include "spdlog/spdlog.h"
#include <limits>
#include <algorithm>

NuGeom::Location NuGeom::Shape::Contains(const Vector3D &point) const {
    double dist = SignedDistance(point);
    if(dist < 0) return NuGeom::Location::kInterior;
    else if(dist > 0) return NuGeom::Location::kExterior;
    else return NuGeom::Location::kSurface;
}

NuGeom::Vector3D NuGeom::Shape::TransformPoint(const Vector3D &point) const {
    return m_rotation.Apply(m_translation.Apply(point)); 
}

NuGeom::Ray NuGeom::Shape::TransformRay(const Ray &in_ray) const {
    auto origin = m_rotation.Apply(m_translation.Apply(in_ray.Origin())); 
    auto direction = m_rotation.Apply(in_ray.Direction());
    return {origin, direction};
}


double NuGeom::Shape::Intersect(const Ray &in_ray) const {
    auto ray = identity_transform ? in_ray : TransformRay(in_ray);
    return IntersectImpl(ray);
}

std::pair<double, double> NuGeom::Shape::SolveQuadratic(double a, double b, double c) const {
    const double det = b*b - 4*a*c;
    if(det < 0) return {std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()};
    double t1 = 2*c/(-b-sqrt(det));
    double t2 = 2*c/(-b+sqrt(det));
    t1 = t1 > 0 ? t1 : std::numeric_limits<double>::infinity();
    t2 = t2 > 0 ? t2 : std::numeric_limits<double>::infinity();
    return {t1, t2};
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

// TODO: Ensure logic for this is the same as SDF calculation
double NuGeom::CombinedShape::IntersectImpl(const Ray &ray) const {
    double intersect1 = m_left -> Intersect(ray);
    double intersect2 = m_right -> Intersect(ray);
    double intersect{};
    switch(m_op) {
        case ShapeBinaryOp::kUnion:
            intersect = std::min(intersect1, intersect2);
            break;
        case ShapeBinaryOp::kIntersect:
            intersect = std::max(intersect1, intersect2);
            break;
        case ShapeBinaryOp::kSubtraction:
            intersect = std::max(-intersect1, intersect2);
            break;
    }

    return intersect;
}

// TODO: Implement volume function
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

double NuGeom::Box::IntersectImpl(const Ray &ray) const {
    // Calculate intersection with all planes
    const double tx1 = (-m_params.X() - ray.Origin().X())/ray.Direction().X();
    const double tx2 = (m_params.X() - ray.Origin().X())/ray.Direction().X();
    const double ty1 = (-m_params.Y() - ray.Origin().Y())/ray.Direction().Y();
    const double ty2 = (m_params.Y() - ray.Origin().Y())/ray.Direction().Y();
    const double tz1 = (-m_params.Z() - ray.Origin().Z())/ray.Direction().Z();
    const double tz2 = (m_params.Z() - ray.Origin().Z())/ray.Direction().Z();
    const auto tx = std::minmax(tx1, tx2);
    const auto ty = std::minmax(ty1, ty2);
    const auto tz = std::minmax(tz1, tz2);

    // Find intersection in x and y direction first
    double tmin, tmax;
    if(tx.first > ty.second || ty.first > tx.second) return std::numeric_limits<double>::infinity();
    tmin = std::max(tx.first, ty.first);
    tmax = std::min(tx.second, ty.second);

    // Find intersection in z direction
    if(tmin > tz.second || tz.first > tmax) return std::numeric_limits<double>::infinity();
    tmin = std::max(tmin, tz.first);
    tmax = std::min(tmax, tz.second);

    return tmin > 0 ? tmin : tmax > 0 ? tmax : std::numeric_limits<double>::infinity();
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

double NuGeom::Sphere::IntersectImpl(const Ray &ray) const {
    const double a = ray.Direction()*ray.Direction();
    const double b = 2*ray.Origin()*ray.Direction();
    const double c = ray.Origin()*ray.Origin() - m_radius;
    auto intersects = SolveQuadratic(a, b, c);
    return std::min(intersects.first, intersects.second);
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

double NuGeom::Cylinder::IntersectImpl(const Ray &ray) const {
    const double a = ray.Direction().X()*ray.Direction().X() + ray.Direction().Y()*ray.Direction().Y();
    const double b = 2*ray.Direction().X()*ray.Origin().X() + 2*ray.Direction().Y()*ray.Origin().Y();
    const double c = ray.Origin().X()*ray.Origin().X() + ray.Origin().Y()*ray.Origin().Y() - m_radius;
    auto intersects = SolveQuadratic(a, b, c);
    // Ensure the ray does not pass below or above finite cylinder
    double z1 = std::numeric_limits<double>::infinity(), z2 = std::numeric_limits<double>::infinity();
    if(intersects.first != std::numeric_limits<double>::infinity()) {
        z1 = ray.Origin().Z() + intersects.first*ray.Direction().Z();
        if(z1 < 0 || z1 > m_height) intersects.first = std::numeric_limits<double>::infinity();
    }
    if(intersects.second != std::numeric_limits<double>::infinity()) {
        z2 = ray.Origin().Z() + intersects.second*ray.Direction().Z();
        if(z2 < 0 || z2 > m_height) intersects.second = std::numeric_limits<double>::infinity();
    }
    // Calculate the time for the intersection with the endcaps if ray passes through the endcaps
    double t3 = z1*z2 < 0 ? -ray.Origin().Z()/ray.Direction().Z() : std::numeric_limits<double>::infinity();
    double t4 = (z1-m_height)*(z2-m_height) < 0 ? (m_height-ray.Origin().Z())/ray.Direction().Z()
                : std::numeric_limits<double>::infinity();
    t3 = t3 > 0 ? t3 : std::numeric_limits<double>::infinity();
    t4 = t4 > 0 ? t4 : std::numeric_limits<double>::infinity();
    return std::min(std::min(std::min(intersects.first, intersects.second), t3), t4);
}
