#pragma once

#include "geom/Vector3D.hh"

namespace NuGeom {

class Ray {
    public:
        Ray() = default;
        Ray(const Vector3D &origin, const Vector3D &direction, bool normalize=true) 
            : m_origin{origin}, m_direction{direction} {
                if(normalize) m_direction = m_direction.Unit();
            }

        Vector3D Origin() const { return m_origin; }
        Vector3D Direction() const { return m_direction; }
        Vector3D Propagate(double t) const { return m_origin + t*m_direction; }

    private:
        Vector3D m_origin, m_direction;
};

}
