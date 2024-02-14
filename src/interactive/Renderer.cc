#include "interactive/Renderer.hh"
#include "geom/Vector3D.hh"
#include "geom/World.hh"
#include "geom/Camera.hh"
#include <numeric>
#include <execution>

using NuGeom::Interactive::Renderer;

void Renderer::OnResize(uint32_t width, uint32_t height) {
    m_image.create(NuGeom::Camera::Width(), NuGeom::Camera::Height());
    m_horizontal.resize(width);
    m_vertical.resize(height);
    std::iota(m_horizontal.begin(), m_horizontal.end(), 0);
    std::iota(m_vertical.begin(), m_vertical.end(), 0);
}

void Renderer::Render(const NuGeom::World &world, const NuGeom::Camera &camera) { 
    m_world = &world;
    m_camera = &camera;
    std::for_each(std::execution::par, m_horizontal.begin(), m_horizontal.end(),
            [this](uint32_t i) {
                std::for_each(std::execution::par, m_vertical.begin(), m_vertical.end(),
                        [this, i](uint32_t j) {
                            m_image.setPixel(i, j, Utils::ToColor32(PixelColor(i, j)));
                });
    });
}

NuGeom::Vector3D Renderer::PixelColor(uint32_t i, uint32_t j) {
    // Get ray
    NuGeom::Ray ray = m_camera -> MakeRay(i, j);
    // auto ray = m_camera -> GetRay(i, j);

    double distance = 0;
    size_t idx;
    bool hit = m_world -> RayTrace(ray, distance, idx);
    NuGeom::Vector3D color;
    if(!hit || idx == 0) {
        Vector3D offset(ray.Direction().Y() * 0.4, ray.Direction().Y() * 0.4, ray.Direction().Y() * 0.4);
        color = Vector3D(0.30, 0.36, 0.60);
    } else {
        Vector3D Light = Vector3D(2, 2, 0).Unit();
        auto norm = CalcNormal(ray.Propagate(distance));
        double NoL = std::max(norm.Dot(Light), 0.0);
        Vector3D objectSurfaceColor;
        if(idx == 1) objectSurfaceColor = Vector3D(0.4, 0.5, 0.1);
        else if(idx == 2) objectSurfaceColor = Vector3D(0.1, 0.2, 0.8);
        else if(idx == 3) objectSurfaceColor = Vector3D(0.8, 0.2, 0.8);
        else if(idx == 4) objectSurfaceColor = Vector3D(0.1, 0.8, 0.1);
        Vector3D LDirectional = Vector3D(0.9, 0.9, 0.8)*NoL;
        Vector3D LAmbient(0.03, 0.04, 0.1);
        Vector3D diffuse(objectSurfaceColor.X() * (LDirectional.X() + LAmbient.X()),
                                 objectSurfaceColor.Y() * (LDirectional.Y() + LAmbient.Y()),
                                 objectSurfaceColor.Z() * (LDirectional.Z() + LAmbient.Z()));
        color = diffuse;

        double shadow = 0.0;
        Vector3D shadowRayOrigin = ray.Propagate(distance) + norm * 0.01;
        Vector3D shadowRayDirection = Light;
        NuGeom::Ray ray2(shadowRayOrigin, shadowRayDirection);
        double distance2;
        size_t step2;
        size_t idx2;
        bool hit2 = m_world -> RayTrace(ray2, distance2, idx2);
        if(hit2) shadow = 1.0;

        color.X() = color.X()*(1-shadow) + color.X()*0.8*shadow;
        color.Y() = color.Y()*(1-shadow) + color.Y()*0.8*shadow;
        color.Z() = color.Z()*(1-shadow) + color.Z()*0.8*shadow;
    }
    color = Vector3D(sqrt(color.X()),
                     sqrt(color.Y()),
                     sqrt(color.Z()));
    return color;
}

NuGeom::Vector3D Renderer::CalcNormal(const Vector3D &pos) {
    // Center sample
    NuGeom::Shape *shape = m_world -> GetShape(0);
    double c = shape -> SignedDistance(pos);

    // Use offset to compute gradient / normal
    double eps = 0.001;
    Vector3D xAxis(1, 0, 0);
    Vector3D yAxis(0, 1, 0);
    Vector3D zAxis(0, 0, 1);
    return Vector3D(shape -> SignedDistance(pos + eps*xAxis) - c,
                    shape -> SignedDistance(pos + eps*yAxis) - c,
                    shape -> SignedDistance(pos + eps*zAxis) - c).Unit();
}
