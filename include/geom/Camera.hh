#pragma once

#include "geom/Vector2D.hh"
#include "geom/Vector3D.hh"
#include "geom/Ray.hh"
#include <iostream>

namespace NuGeom {

class Camera {
    public:
        Camera(const Vector3D &pos, const Vector3D &target, double fov=90, double zoom=1) :
            m_pos{pos}, m_target{target}, m_fov{std::tan(fov*M_PI/180/2)}, m_zoom{zoom} {
            
            m_forward = (m_target - m_pos).Unit();
            m_right = Vector3D(0, 1, 0).Cross(m_forward);
            m_up = m_forward.Cross(m_right).Unit();
            m_width = double(imgWidth);
            m_height = double(imgHeight);
            m_aspect = m_width / m_height;
        }

        NuGeom::Ray MakeRay(size_t px, size_t py) const {
            return NuGeom::Ray(m_pos, RayDirection(px, py));
        }

        static constexpr size_t Width() { return imgWidth; }
        static constexpr size_t Height() { return imgHeight; }

    private:
        Vector3D RayDirection(size_t px, size_t py) const {
            Vector2D uv = NormalizeScreen(px, py);
            return uv.X() * m_right + uv.Y() * m_up + m_zoom * m_forward;
        }

        Vector2D NormalizeScreen(size_t px, size_t py) const {
            double x = double(px) / m_width * 2 - 1;
            double y = double(py) / m_height * 2 - 1;
            x *= m_aspect * m_fov;
            y *= m_fov;

            return {x, y};
        }

        Vector3D m_pos, m_target;
        Vector3D m_forward, m_right, m_up;
        double m_fov, m_zoom;
        double m_width, m_height, m_aspect;

        // Image size
        static constexpr size_t imgWidth = 1080;
        static constexpr size_t imgHeight = 720;

};

}
