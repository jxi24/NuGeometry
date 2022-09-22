#include <iostream>
#include <fstream>
#include <chrono>
#include "geom/Parser.hh"
#include "geom/Vector2D.hh"
#include "geom/Vector3D.hh"
#include "geom/Ray.hh"
#include "geom/Camera.hh"
#include "geom/World.hh"
#include "geom/LineSegment.hh"

using NuGeom::Vector3D;

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

void PixelColor(const NuGeom::World &world, const NuGeom::Camera &camera, size_t i, size_t j, Vector3D &color, Vector3D &/*cost*/) {
    // Get ray
    NuGeom::Ray ray = camera.MakeRay(i, j);

    double distance = 0;
    // size_t step = 0;
    size_t idx;
    // bool hit = world.SphereTrace(ray, distance, step, idx);
    bool hit = world.RayTrace(ray, distance, idx);
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
    // cost = Vector3D(1, 0, 0)*(double(step)/512);
}

bool WriteToFile(const std::string &filename, const std::vector<Vector3D> &pixels) {
    std::ofstream out(filename);
    out << "P3\n" << NuGeom::Camera::Width() << " " << NuGeom::Camera::Height() << "\n255\n";
    for(size_t j = NuGeom::Camera::Height(); j != 0; --j) {
        for(size_t i = 0; i < NuGeom::Camera::Width(); ++i) {
            Vector3D color = pixels[i*NuGeom::Camera::Height() + j];
            int ir = std::max(int(255.99*color.R()), 0);
            int ib = std::max(int(255.99*color.B()), 0);
            int ig = std::max(int(255.99*color.G()), 0);

            out << ir << " " << ig << " " << ib << "\n";
        }
    }
    out.close();
    return true;
}

void render(const NuGeom::World& world, const NuGeom::Camera &camera, std::vector<Vector3D> &pixels, std::vector<Vector3D> pixelsCost) {
    std::cout << "Rendering..." << std::endl;
    for(size_t i = 0; i < NuGeom::Camera::Width(); ++i) {
        if(i % 10 == 0) std::cout << "Rendering row: " << i << "\r";
        for(size_t j = 0; j < NuGeom::Camera::Height(); ++j) {
            Vector3D color, cost;
            PixelColor(world, camera, i, j, color, cost);
            pixels[i*NuGeom::Camera::Height() + j] = color;
            pixelsCost[i*NuGeom::Camera::Height() + j] = cost;
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
            pixels[static_cast<size_t>(y)*NuGeom::Camera::Height() + static_cast<size_t>(x)] = Vector3D(1, 0, 0);
            error2 += derror2;
            if(error2 > dx) {
                y += (y1 > y0 ? 1 : -1);
                error2 -= dx*2;
            }
        }
    } else {
        for(int x=x0; x<=x1; x++) {
            pixels[static_cast<size_t>(x)*NuGeom::Camera::Height() + static_cast<size_t>(y)] = Vector3D(1, 0, 0);
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

    int x0 = static_cast<int>((start.X() + 1)*static_cast<double>(NuGeom::Camera::Width()) / 2.0);
    int y0 = static_cast<int>((start.Y() + 1)*static_cast<double>(NuGeom::Camera::Height()) / 2.0);
    int x1 = static_cast<int>((end.X() + 1)*static_cast<double>(NuGeom::Camera::Width()) / 2.0);
    int y1 = static_cast<int>((end.Y() + 1)*static_cast<double>(NuGeom::Camera::Height()) / 2.0);

    line(x0, y0, x1, y1, pixels);
}

int main(int argc, char* argv[]) {
    if(argc != 2) {
        std::cout << "Usage:\n  geom_test <input>\n";
        return -1;
    }
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(argv[1]);
    if(!result)
        throw std::runtime_error("GDMLParser: Invalid file");

    NuGeom::GDMLParser parse(doc);
    NuGeom::World world = parse.GetWorld();

    NuGeom::Camera camera({-150, 30, 30}, {0, 0, 0}, 90, 1);

    std::vector<Vector3D> pixels(NuGeom::Camera::Height()*NuGeom::Camera::Width());
    std::vector<Vector3D> pixelsCost(NuGeom::Camera::Height()*NuGeom::Camera::Width());

    auto render_start = std::chrono::high_resolution_clock::now(); 
    render(world, camera, pixels, pixelsCost);
    auto render_end = std::chrono::high_resolution_clock::now(); 
    std::chrono::duration<double> render_elapsed = render_end - render_start;
    std::cout << "Total Time for render = " << render_elapsed.count() << "\n";

    NuGeom::Camera neutrino({-150, 0, 0}, {0, 0, 0});

    // Get ray
    constexpr size_t nrays = 10000;
    auto start = std::chrono::high_resolution_clock::now(); 
    for(size_t i = 0; i < nrays; ++i) {
        NuGeom::Ray ray = neutrino.MakeRay(NuGeom::Camera::Width()/2, NuGeom::Camera::Height()/2);
        (void)world.GetLineSegments(ray);
    }
    auto end = std::chrono::high_resolution_clock::now(); 
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Total Time for " << nrays << " rays = " << elapsed.count() << "\n";
    std::cout << "Time per ray = " << elapsed.count() / nrays << "\n";
    NuGeom::Ray ray = neutrino.MakeRay(NuGeom::Camera::Width()/2, NuGeom::Camera::Height()/2);
    auto lines = world.GetLineSegments(ray);
    for(const auto &line : lines) {
        std::cout << line.Length() << "\n";
    }

    // renderRay(ray, 100, pixels);

    WriteToFile("render.ppm", pixels);
    WriteToFile("renderCost.ppm", pixelsCost);

    return 0;
}

/*
class DetectorSim {
    public:
        void Setup(const std::string &geometry) {
            // Create world from geometry
            NuGeom::GDMLParser parser(geometry);
            world = parser.GetWorld();
        }

        void SetupMaterials() {

        }

        void SetupShapes() {
            while(true) {
                std::string shape;
                std::cout << "What shape? (box, sphere, or exit) ";
                std::cin >> shape;
                if(shape == "box") {
                    double x, y, z;
                    std::cout << "Dimensions of box (x, y, z) ";
                    std::cin >> x >> y >> z;
                    // Check x, y, z > 0
                    auto box = std::make_shared<NuGeom::Box>(x, y, z);
                    shapes.push_back(box);
                } else if(shape == "sphere") {
                    double r;
                    std::cout << "Radius of sphere ";
                    std::cin >> r;
                    // Check r > 0
                    auto sphere = std::make_shared<NuGeom::Sphere>(r);
                    shapes.push_back(sphere);
                } else if(shape == "exit") {
                    break;
                } else {
                    std::cout << "Invalid shape!" << std::endl;
                }
            }
        }

        void SetupVolumes() {
            // 1. Select world shape
            // 2. Ask if there is a shape inside
            int depth = 1;
            while(depth > 0) {
            // 3. Select a shape 
            // 4. Select a material
            // 5. Rotate / Translate shape 
            // 6. Add to world
            // 7. Return to 2. until you say there is no more in world 
            //    if yes: (depth++)
            //    if no: (depth--)
            }
        }

        std::vector<NuGeom::Material> GetMaterials() {
            // return materials in detector
        }

        void GetMeanFreePath(const std::vector<double> &cross_section) {
            // Fill result mfp
            if(cross_section.size() != m_mats.size())
                throw "ERROR";

            for(size_t i = 0; i < m_mats.size(); ++i) {
                m_mfp[m_mats[i]] = cross_section[i];
            }
        }

        std::pair<Vector3D, NuGeom::Material> GetInteraction(const NuGeom::Ray &ray) {
            auto segments = world.GetLineSegments(ray);
            Vector3D point;
            NuGeom::Material mat;
            // Choose interaction point
            return {point, mat};
        }

    private:
        NuGeom::World world;
        std::vector<std::shared_ptr<NuGeom::Shape>> shapes;
        std::vector<NuGeom::Material> m_mats;
        std::map<NuGeom::Material, double> m_mfp;

}; */
