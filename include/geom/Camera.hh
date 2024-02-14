#pragma once

#include "geom/Transform3D.hh"
#include "geom/Vector2D.hh"
#include "geom/Vector3D.hh"
#include "geom/Ray.hh"
#include <cmath>
#include <iostream>
#include <vector>
#include <cstdint>

namespace NuGeom {

class Camera {
    public:
        Camera(const Vector3D &pos, const Vector3D &target, double fov=90, double zoom=1) :
            m_pos{pos}, m_target{target}, m_fov{std::tan(fov*M_PI/180/2)}, m_zoom{zoom} {
            
            m_forward = (m_target - m_pos).Unit();
            m_up = Vector3D(0, 1, 0);
            m_right = Vector3D(0, 1, 0).Cross(m_forward);
            // m_up = m_forward.Cross(m_right).Unit();
            m_width = double(imgWidth);
            m_height = double(imgHeight);
            m_aspect = m_width / m_height;
            m_yaw = 0;
            m_pitch = 0;
        }

        NuGeom::Ray MakeRay(size_t px, size_t py) const {
            return NuGeom::Ray(m_pos, RayDirection(px, py));
        }

        void MoveForward(double distance) {
            m_pos += m_forward * distance;
        }

        void MoveRight(double distance) {
            m_pos += m_right * distance;
        }

        void MoveUp(double distance) {
            m_pos += m_up * distance;
        }

        void Rotate(double dx, double dy) {
            static constexpr double sensitivity = 0.1;
            m_yaw = dx * sensitivity*M_PI/180.0;
            m_pitch = dy * sensitivity*M_PI/180.0;

            // Clamp pitch between -90 and 90 degrees
            m_pitch = std::max(std::min(m_pitch, M_PI_2), -M_PI_2);
            UpdateVectors();
        }

        static constexpr size_t Width() { return imgWidth; }
        static constexpr size_t Height() { return imgHeight; }

        const Ray& GetRay(size_t x, size_t y) const { return m_rays[y*imgWidth+x]; }
        void PreComputeRays() {
            m_rays.resize(imgHeight*imgWidth);
            for(uint32_t y = 0; y < imgHeight; ++y) {
                for(uint32_t x = 0; x < imgWidth; ++x) {
                    m_rays[y*imgWidth + x] = MakeRay(x, y);
                }
            }
        }

    private:
        Vector3D RayDirection(size_t px, size_t py) const {
            Vector2D uv = NormalizeScreen(px, py);
            return uv.X() * m_right + uv.Y() * m_up + m_zoom * m_forward;
        }

        Vector2D NormalizeScreen(size_t px, size_t py) const {
            return {(static_cast<double>(px) / m_width * 2 - 1) * m_aspect * m_fov,
                    (static_cast<double>(py) / m_height * 2 - 1) * m_fov};
        }

        void UpdateVectors() {
            auto rot = NuGeom::Rotation3D(m_right, -m_pitch)*NuGeom::Rotation3D(m_up, m_yaw);
            m_forward = rot.Apply(m_forward);
            m_right = rot.Apply(m_right);
            // m_up = m_forward.Cross(m_right).Unit();
            m_target = m_pos + m_forward;
        }

        Vector3D m_pos, m_target;
        Vector3D m_forward, m_right, m_up;
        double m_fov, m_zoom, m_yaw, m_pitch;
        double m_width, m_height, m_aspect;

        std::vector<Ray> m_rays;

        // Image size
        // static constexpr size_t imgWidth = 1920;
        // static constexpr size_t imgHeight = 1080;
        static constexpr size_t imgWidth = 640;
        static constexpr size_t imgHeight = 480;

};

}
