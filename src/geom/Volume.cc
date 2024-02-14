#include "geom/Volume.hh"
#include "geom/Ray.hh"
#include "geom/LineSegment.hh"
#include "spdlog/spdlog.h"

#include <limits>
#include <numeric>

using NuGeom::LogicalVolume;
using NuGeom::PhysicalVolume;

double LogicalVolume::Mass() const {
    return Volume() * m_material.Density()
        + DaughterMass();
}

double AddVolume(double a, const std::shared_ptr<PhysicalVolume> &b) {
    return a + b -> GetLogicalVolume() -> Volume();
}

double LogicalVolume::DaughterVolumes() const {
    return std::accumulate(m_daughters.begin(),
                           m_daughters.end(),
                           0.0, AddVolume);
}

double LogicalVolume::Volume() const {
    return m_shape -> Volume() - DaughterVolumes();
}

double AddMass(double a, const std::shared_ptr<PhysicalVolume> &b) {
    return a + b -> GetLogicalVolume() -> Mass();
}

double LogicalVolume::DaughterMass() const {
    return std::accumulate(m_daughters.begin(),
                           m_daughters.end(),
                           0.0, AddMass);
}

bool LogicalVolume::SphereTrace(const Ray &ray, double &time, size_t &step, size_t &idx) const {
    step = 0;
    Vector3D pos = ray.Origin();
    time = 0;
    auto res = GetSDF(pos);
    while(step < m_max_steps && std::abs(res.first) > m_epsilon && InWorld(pos)) {
        pos = ray.Propagate(time);
        res = GetSDF(pos);
        time += res.first;
        step++;
    }
    if(step == m_max_steps || !InWorld(pos) || res.second == 0) return false;
    idx = res.second;
    return true;
}

bool LogicalVolume::RayTrace(const Ray &ray, double &time, std::shared_ptr<PhysicalVolume> &vol) const {
    time = std::numeric_limits<double>::infinity();
    for(const auto &daughter : Daughters()) {
        double ctime = daughter -> Intersect(ray);
        if(ctime < time) {
            time = ctime;
            vol = daughter;
        }
    }
    return time < std::numeric_limits<double>::infinity();
}

void LogicalVolume::GetLineSegments(const Ray &ray, std::vector<LineSegment> &segments) const {
    static constexpr double eps = 1e-8;
    double time = 0;
    auto shift_ray = Ray(ray.Propagate(eps), ray.Direction());
    std::shared_ptr<PhysicalVolume> pvol = nullptr;
    if(!RayTrace(shift_ray, time, pvol)) {
        auto tmp_origin = ray.Propagate(eps);
        auto tmp_ray = Ray(tmp_origin, ray.Direction());
        time = m_shape -> Intersect(tmp_ray) + eps;
    }
    time += eps;
    segments.emplace_back(ray.Origin(), ray.Propagate(time), m_material);

    if(!pvol) return;
    auto origin = ray.Propagate(time);
    auto new_ray = Ray(origin, ray.Direction());
    pvol -> GetLineSegments(new_ray, segments, {});
}

double PhysicalVolume::Intersect(const Ray &in_ray) const {
    auto ray = TransformRay(in_ray);
    return m_volume -> GetShape() -> Intersect(ray);
}

void PhysicalVolume::GetLineSegments(const Ray &in_ray, std::vector<LineSegment> &segments,
                                     const Transform3D &from_global) const {
    static constexpr double eps = 1e-8;
    auto local_ray = Transform3D::ApplyRay(in_ray, from_global);
    auto ray = TransformRay(local_ray);
    auto shift_ray = Ray(ray.Propagate(eps), ray.Direction());
    double time = 0;
    std::shared_ptr<PhysicalVolume> pvol = nullptr;
    if(!RayTrace(shift_ray, time, pvol)) {
        auto tmp_origin = ray.Propagate(eps);
        auto tmp_ray = Ray(tmp_origin, ray.Direction());
        time = m_volume -> GetShape() -> Intersect(tmp_ray);

        if(m_mother) {
            pvol = m_mother;
        }
    }
    time += eps;
    auto origin = in_ray.Propagate(time);
    segments.emplace_back(in_ray.Origin(), origin, m_volume -> GetMaterial());
    auto new_ray = Ray(origin, in_ray.Direction());

    if(!pvol) {
        if(m_volume -> Mother()) {
            m_volume -> Mother() -> GetLineSegments(new_ray, segments); 
        }
        return;
    }
    Transform3D transform = m_transform;
    if(pvol == m_mother) {
        transform = pvol -> GetTransform().Inverse();
    }
    auto newtransform = from_global*transform;
    pvol -> GetLineSegments(new_ray, segments, newtransform);
}

NuGeom::Ray PhysicalVolume::TransformRay(const Ray &ray) const {
    if(is_identity) return ray;
    else if(is_translation) return Transform3D::TranslateRay(ray, m_trans);
    return Transform3D::ApplyRay(ray, m_trans, m_rot);
}

NuGeom::Ray PhysicalVolume::TransformRayInverse(const Ray &ray) const {
    if(is_identity) return ray;
    else if(is_translation) return Transform3D::TranslateRay(ray, m_trans);
    return Transform3D::ApplyRay(ray, m_transform.Inverse());
}

