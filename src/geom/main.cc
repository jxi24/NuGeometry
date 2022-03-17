#include <iostream>
#include <fstream>
#include <chrono>
#include "geom/Parser.hh"
#include "geom/Vector2D.hh"
#include "geom/Vector3D.hh"
#include "geom/Ray.hh"
#include "geom/World.hh"
#include "geom/LineSegment.hh"

constexpr size_t imgWidth = 1080;
constexpr size_t imgHeight = 720;

using NuGeom::Vector2D;
using NuGeom::Vector3D;

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
};

Vector3D CalcNormal(NuGeom::World world, const Vector3D &pos) {
    // Center sample
    NuGeom::Shape *shape = world.GetShape(0);
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

void PixelColor(const NuGeom::World &world, const Camera &camera, size_t i, size_t j, Vector3D &color, Vector3D &cost) {
    // Get ray
    NuGeom::Ray ray = camera.MakeRay(i, j);

    double distance = 0;
    size_t step = 0;
    size_t idx;
    bool hit = world.SphereTrace(ray, distance, step, idx);
    if(!hit || idx == 0) {
        Vector3D offset(ray.Direction().Y() * 0.4, ray.Direction().Y() * 0.4, ray.Direction().Y() * 0.4);
        color = Vector3D(0.30, 0.36, 0.60) - offset;
    } else {
        Vector3D Light = Vector3D(2, 2, 0).Unit();
        auto norm = CalcNormal(world, ray.Propagate(distance));
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
        bool hit2 = world.SphereTrace(ray2, distance2, step2, idx2);
        if(hit2) shadow = 1.0;

        color.X() = color.X()*(1-shadow) + color.X()*0.8*shadow;
        color.Y() = color.Y()*(1-shadow) + color.Y()*0.8*shadow;
        color.Z() = color.Z()*(1-shadow) + color.Z()*0.8*shadow;
    }
    color = Vector3D(pow(color.X(), 0.4545),
                             pow(color.Y(), 0.4545),
                             pow(color.Z(), 0.4545));
    cost = Vector3D(1, 0, 0)*(double(step)/512);
}

bool WriteToFile(const std::string &filename, const std::vector<Vector3D> &pixels) {
    std::ofstream out(filename);
    out << "P3\n" << imgWidth << " " << imgHeight << "\n255\n";
    for(size_t j = imgHeight; j != 0; --j) {
        for(size_t i = 0; i < imgWidth; ++i) {
            Vector3D color = pixels[i*imgHeight + j];
            int ir = std::max(int(255.99*color.R()), 0);
            int ib = std::max(int(255.99*color.B()), 0);
            int ig = std::max(int(255.99*color.G()), 0);

            out << ir << " " << ig << " " << ib << "\n";
        }
    }
    out.close();
    return true;
}

void render(const NuGeom::World& world, const Camera &camera, std::vector<Vector3D> &pixels, std::vector<Vector3D> pixelsCost) {
    std::cout << "Rendering..." << std::endl;
    for(size_t i = 0; i < imgWidth; ++i) {
        if(i % 10 == 0) std::cout << "Rendering row: " << i << "\r";
        for(size_t j = 0; j < imgHeight; ++j) {
            Vector3D color, cost;
            PixelColor(world, camera, i, j, color, cost);
            pixels[i*imgHeight + j] = color;
            pixelsCost[i*imgHeight + j] = cost;
        }
        std::cout << std::flush;
    }
    std::cout << std::endl;
}

void line(int x0, int y0, int x1, int y1, std::vector<Vector3D> &pixels) {
    bool steep = false;
    if(std::abs(x0-x1) < std::abs(y0-y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if(x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int dx = x1-x0;
    int dy = y1-y0;
    int derror2 = std::abs(dy)*2;
    int error2 = 0;
    int y = y0;
    if(steep) {
        for(int x=x0; x<=x1; x++) {
            pixels[static_cast<size_t>(y)*imgHeight + static_cast<size_t>(x)] = Vector3D(1, 0, 0);
            error2 += derror2;
            if(error2 > dx) {
                y += (y1 > y0 ? 1 : -1);
                error2 -= dx*2;
            }
        }
    } else {
        for(int x=x0; x<=x1; x++) {
            pixels[static_cast<size_t>(x)*imgHeight + static_cast<size_t>(y)] = Vector3D(1, 0, 0);
            error2 += derror2;
            if(error2 > dx) {
                y += (y1 > y0 ? 1 : -1);
                error2 -= dx*2;
            }
        }
    }
}

void renderRay(const NuGeom::Ray &ray, double t, std::vector<Vector3D> &pixels) {
    Vector3D start = ray.Origin();
    Vector3D end = ray.Propagate(t);

    int x0 = static_cast<int>((start.X() + 1)*static_cast<double>(imgWidth) / 2.0);
    int y0 = static_cast<int>((start.Y() + 1)*static_cast<double>(imgHeight) / 2.0);
    int x1 = static_cast<int>((end.X() + 1)*static_cast<double>(imgWidth) / 2.0);
    int y1 = static_cast<int>((end.Y() + 1)*static_cast<double>(imgHeight) / 2.0);

    line(x0, y0, x1, y1, pixels);
}

int main(int argc, char* argv[]) {
    if(argc != 2) {
        std::cout << "Usage:\n  geom_test <input>\n";
        return -1;
    }
    NuGeom::GDMLParser parse(argv[1]);
    NuGeom::World world = parse.GetWorld();

    Camera camera({-150, 0, 0}, {0, 0, 0}, 90, 1);

    std::vector<Vector3D> pixels(imgHeight*imgWidth);
    std::vector<Vector3D> pixelsCost(imgHeight*imgWidth);

    render(world, camera, pixels, pixelsCost);

    // Camera neutrino({0, 0, -10}, {0, 0, 0});

    // // Get ray
    // constexpr size_t nrays = 1000;
    // auto start = std::chrono::high_resolution_clock::now(); 
    // for(size_t i = 0; i < nrays; ++i) {
    //     NuGeom::Ray ray = neutrino.MakeRay(imgWidth/2, imgHeight/2);
    //     (void)world.GetLineSegments(ray);
    // }
    // auto end = std::chrono::high_resolution_clock::now(); 
    // std::chrono::duration<double> elapsed = end - start;
    // std::cout << "Total Time for " << nrays << " rays = " << elapsed.count() << "\n";
    // std::cout << "Time per ray = " << elapsed.count() / nrays << "\n";
    // NuGeom::Ray ray = neutrino.MakeRay(imgWidth/2, imgHeight/2);
    // auto lines = world.GetLineSegments(ray);
    // for(const auto &line : lines) {
    //     std::cout << line.Length() << " " << (line.ShapeID() == SIZE_MAX ? -1 : static_cast<int>(line.ShapeID())) << "\n";
    // }

    // renderRay(ray, 100, pixels);

    WriteToFile("render.ppm", pixels);
    WriteToFile("renderCost.ppm", pixelsCost);

    return 0;
}
