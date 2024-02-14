add_executable(SFMLTest main_sfml.cc Renderer.cc)
target_link_libraries(SFMLTest PRIVATE project_options 
                               PUBLIC ImGui-SFML::ImGui-SFML geom tbb)
