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

std::vector<NuGeom::LineSegment> World::GetLineSegments(const Ray &ray) const {
    double prev_dist = 0;
    std::vector<NuGeom::LineSegment> segments;
    size_t step = 0;
    Vector3D pos = ray.Origin();
    double distance = 0;
    double res = 0;
    std::deque<size_t> material_list;
    material_list.push_back(SIZE_MAX);
    while(step < m_max_steps && InWorld(pos)) {
        pos = ray.Propagate(distance);
        auto tmp = GetSDFNonNeg(pos);
        res = tmp.first;
        distance += res;
        if(std::abs(res) < m_epsilon) {
            if(tmp.second != material_list.back()) {
                segments.emplace_back(distance - prev_dist, material_list.back());
                material_list.push_back(tmp.second);
            } else {
                segments.emplace_back(distance - prev_dist, material_list.back());
                material_list.pop_back();
            }
            prev_dist = distance;
            distance += 0.01;
        }
        step++;
    }
    segments.emplace_back(distance - prev_dist, material_list.back());
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
