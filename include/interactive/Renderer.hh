#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>
#include <cstdint>
#include "geom/Vector3D.hh"

namespace NuGeom {
class World;
class Camera;
}

namespace NuGeom::Interactive {

namespace Utils {

inline sf::Color ToColor32(const NuGeom::Vector3D &color) {
    uint8_t red = std::max(int(255.99*color.R()), 0); 
    uint8_t blue = std::max(int(255.99*color.B()), 0); 
    uint8_t green = std::max(int(255.99*color.G()), 0); 
    uint8_t a = 255;

    return {red, green, blue, a};
}

}

class Renderer {
    public:
        Renderer() = default;

        void Render(const NuGeom::World& world, const NuGeom::Camera &camera);
        void OnResize(uint32_t width, uint32_t height);

        const sf::Image& Image() const { return m_image; }

    private:
        NuGeom::Vector3D PixelColor(uint32_t iwidth, uint32_t iheight);
        NuGeom::Vector3D CalcNormal(const NuGeom::Vector3D &pos);

        const NuGeom::World *m_world;
        const NuGeom::Camera *m_camera;
        std::vector<uint32_t> m_horizontal, m_vertical;
        sf::Image m_image;
};

}
