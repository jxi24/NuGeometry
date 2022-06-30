#pragma once

#include "geom/Material.hh"

#include <memory>

namespace NuGeom {

class LineSegment {
    public:
        LineSegment() = default;
        LineSegment(double length, Material mat) 
            : m_length{length}, m_material{std::move(mat)} {}
        LineSegment(double length, size_t idx) 
            : m_length{length}, m_idx{idx} {}

        double Length() const { return m_length; }
        size_t ShapeID() const { return m_idx; }
        Material GetMaterial() const { return m_material; }

    private:
        double m_length;
        size_t m_idx;
        Material m_material;
};

}
