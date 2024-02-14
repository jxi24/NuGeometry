#include "geom/Camera.hh"
#include "geom/Parser.hh"
#include "geom/Vector2D.hh"
#include "geom/Vector3D.hh"
#include "interactive/Renderer.hh"
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <chrono>
#include <execution>

using NuGeom::Vector3D;

int main() {
    // TODO: Add to event loop to load
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file("SimpleBoxes.geom.manual.gdml");
    if(!result)
        throw std::runtime_error("GDMLParser: Invalid file");

    NuGeom::GDMLParser parse(doc);
    NuGeom::World world = parse.GetWorld();
    NuGeom::Camera camera({150, 0, 0}, {0, 0, 0}, 90, 1);
    camera.PreComputeRays();
    NuGeom::Interactive::Renderer renderer;
    renderer.OnResize(NuGeom::Camera::Width(), NuGeom::Camera::Height());

    auto render_start = std::chrono::high_resolution_clock::now(); 
    renderer.Render(world, camera);
    auto render_end = std::chrono::high_resolution_clock::now(); 
    std::chrono::duration<double> render_elapsed = render_end - render_start;
    std::cout << "Total Time for render = " << render_elapsed.count() << "\n";

    sf::RenderWindow window(sf::VideoMode(NuGeom::Camera::Width(), NuGeom::Camera::Height()), "SFML works!");
    window.setVerticalSyncEnabled(true);
    window.setActive(true);
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);

    sf::Clock deltaClock;
    std::string filename;
    bool needs_render = false;
    bool right_mouse_down = false;
    sf::Vector2i prev_mouse = sf::Mouse::getPosition(window);
    while(window.isOpen()) {
        window.clear();
        sf::Event event;
        while(window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);

            if(event.type == sf::Event::KeyPressed) {
                if(event.key.code == sf::Keyboard::A) {
                    camera.MoveRight(-5);
                    needs_render = true;
                }
                if(event.key.code == sf::Keyboard::D) {
                    camera.MoveRight(5);
                    needs_render = true;
                }
                if(event.key.code == sf::Keyboard::W) {
                    camera.MoveForward(5);
                    needs_render = true;
                }
                if(event.key.code == sf::Keyboard::S) {
                    camera.MoveForward(-5);
                    needs_render = true;
                }
                if(event.key.code == sf::Keyboard::Q) {
                    camera.MoveUp(-5);
                    needs_render = true;
                }
                if(event.key.code == sf::Keyboard::E) {
                    camera.MoveUp(5);
                    needs_render = true;
                }
            }
            if(event.type == sf::Event::MouseButtonPressed) {
                if(event.mouseButton.button == sf::Mouse::Right) {
                    window.setMouseCursorVisible(false);
                    right_mouse_down = true;
                }
            }
            if(event.type == sf::Event::MouseButtonReleased) {
                if(event.mouseButton.button == sf::Mouse::Right) {
                    window.setMouseCursorVisible(true);
                    right_mouse_down = false;
                }
            }
            if(event.type == sf::Event::MouseMoved) {
                auto mouse_pos = sf::Mouse::getPosition(window);
                auto delta_mouse = mouse_pos - prev_mouse;
                prev_mouse = mouse_pos;
                if(right_mouse_down) {
                    camera.Rotate(delta_mouse.x, delta_mouse.y);
                    needs_render = true;
                }
            }

            if(event.type == sf::Event::Closed) {
                window.close();
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        ImGui::Begin("Render Info");
        ImGui::Text("Filename:");
        ImGui::InputText("##Filename", &filename);
        if(ImGui::Button("LoadFile")) {
            std::cout << "Loading: " << filename << std::endl;
            pugi::xml_document doc;
            pugi::xml_parse_result result = doc.load_file(filename.c_str());
            if(!result)
                throw std::runtime_error("GDMLParser: Invalid file");

            NuGeom::GDMLParser parse(doc);
            world = parse.GetWorld();
            std::cout << world.NDaughters() << std::endl;
            needs_render = true;
        }
        if(needs_render || ImGui::Button("Render")) {
            camera.PreComputeRays();
            needs_render = false;
            render_start = std::chrono::high_resolution_clock::now(); 
            renderer.Render(world, camera);
            render_end = std::chrono::high_resolution_clock::now(); 
        }
        std::chrono::duration<double, std::milli> render_elapsed = render_end - render_start;
        ImGui::Text("Render Time: %6f", render_elapsed.count());

        sf::Texture texture;
        texture.loadFromImage(renderer.Image());
        sf::Sprite sprite(texture);
        window.draw(sprite);

        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();

    return 0;
}
