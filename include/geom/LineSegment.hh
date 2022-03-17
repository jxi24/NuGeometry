#pragma once

#include "geom/Material.hh"

#include <memory>

namespace NuGeom {

class LineSegment {
    public:
        LineSegment() = default;
        LineSegment(double length, size_t idx) : m_length{length}, m_idx{idx} {}

        double Length() const { return m_length; }
        size_t ShapeID() const { return m_idx; }
        Material* GetMaterial() const { return p_material.get(); }

    private:
        double m_length;
        size_t m_idx;
        std::shared_ptr<Material> p_material;
};

}
