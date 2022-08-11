#pragma once

#include "geom/Material.hh"
#include "geom/Vector3D.hh"

#include <memory>

namespace NuGeom {

class LineSegment {
    public:
        LineSegment() = default;
        LineSegment(double length, Material mat) 
            : m_length{length}, m_material{std::move(mat)} {}
        LineSegment(double length, size_t idx) 
            : m_length{length}, m_idx{idx} {}
        LineSegment(Vector3D start, Vector3D end, Material mat) 
            : m_start{start}, m_end{end}, m_material{std::move(mat)} { m_length = (end - start).Norm(); }

        Vector3D Start() const { return m_start; }
        Vector3D End() const { return m_end; }
        double Length() const { return m_length; }
        size_t ShapeID() const { return m_idx; }
        Material GetMaterial() const { return m_material; }

    private:
        Vector3D m_start{}, m_end{};
        double m_length;
        size_t m_idx;
        Material m_material;
};

}
