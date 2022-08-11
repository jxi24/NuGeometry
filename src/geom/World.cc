#include "geom/World.hh"
#include "geom/Ray.hh"
#include "geom/LineSegment.hh"
#include <limits>
#include <iostream>
#include <deque>

using NuGeom::World;

NuGeom::Shape* World::GetShape(size_t idx) const {
    if(idx == 0) return m_volume->GetShape();
    else {
        return m_volume -> Daughters()[idx-1]->GetLogicalVolume()->GetShape();
    }
}

NuGeom::Material World::GetMaterial(size_t idx) const {
    if(idx == 0) return m_volume->GetMaterial();
    else {
        return m_volume -> Daughters()[idx-1]->GetLogicalVolume()->GetMaterial();
    }
}

bool World::InWorld(const Vector3D &pos) const {
    return m_volume -> GetShape() -> SignedDistance(pos) <= 0;
}

bool World::SphereTrace(const Ray &ray, double &distance, size_t &step, size_t &idx) const {
    step = 0;
    Vector3D pos = ray.Origin();
    distance = 0;
    auto res = GetSDF(pos);
    while(step < m_max_steps && std::abs(res.first) > m_epsilon && InWorld(pos)) {
        pos = ray.Propagate(distance);
        res = GetSDF(pos);
        distance += res.first;
        step++;
    }
    if(step == m_max_steps || !InWorld(pos) || res.second == 0) return false;
    idx = res.second;
    return true;
}

bool World::RayTrace(const Ray &ray, double &distance, size_t &idx) const {
    double tmin = std::numeric_limits<double>::infinity();
    for(size_t i = 0; i < m_volume -> Daughters().size(); ++i) {
        double time = m_volume -> Daughters()[i] -> Intersect(ray);
        if(time < tmin) {
            tmin = time;
            idx = i+1;
        }
    }
    distance = tmin;
    return tmin < std::numeric_limits<double>::infinity();
}

std::vector<NuGeom::LineSegment> World::GetLineSegments(const Ray &ray) const {
    std::vector<NuGeom::LineSegment> segments;
    m_volume -> GetLineSegments(ray, segments);
    return segments;
}

std::pair<double, size_t> World::GetSDF(const Vector3D &pos) const {
    double distance = std::numeric_limits<double>::max();
    size_t idx = 0;
    for(size_t i = 0; i < m_volume->Daughters().size(); ++i) {
        double tmp = m_volume->Daughters()[i]->SignedDistance(pos);
        if(tmp < distance) {
            distance = tmp;
            idx = i+1;
        }
    }

    return {distance, idx};
}

std::pair<double, size_t> World::GetSDFNonNeg(const Vector3D &pos) const {
    double distance = std::numeric_limits<double>::max();
    size_t idx = 0;
    for(size_t i = 0; i < m_volume->Daughters().size(); ++i) {
        double tmp = std::abs(m_volume->Daughters()[i]->SignedDistance(pos));
        if(tmp < distance) {
            distance = tmp;
            idx = i+1;
        }
    }

    return {distance, idx};
}
