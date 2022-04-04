#pragma once

#include "geom/Material.hh"
#include "geom/Shape.hh"
#include "geom/Volume.hh"
#include <vector>

namespace NuGeom {

class Ray;
class LineSegment;

class World {
    public:
        World() = default;
        World(std::shared_ptr<LogicalVolume> volume) : m_volume{std::move(volume)} {}
        World(Vector3D min, Vector3D max, 
              size_t max_steps = 1000, double epsilon = 1e-4)
            : m_min{min}, m_max{max},
              m_max_steps{max_steps}, m_epsilon{epsilon} {}

        Shape* GetShape(size_t idx) const;
        Material GetMaterial(size_t idx) const;

        bool InWorld(const Vector3D&) const;
        bool SphereTrace(const Ray&, double&, size_t&, size_t&) const;
        std::vector<LineSegment> GetLineSegments(const Ray&) const;

    private:
        std::pair<double, size_t> GetSDF(const Vector3D&) const;
        std::pair<double, size_t> GetSDFNonNeg(const Vector3D&) const;

        Vector3D m_min{}, m_max{};
        size_t m_max_steps{512};
        double m_epsilon{1e-4};
        std::shared_ptr<LogicalVolume> m_volume;
};

}
