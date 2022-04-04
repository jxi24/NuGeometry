add_library(geom_utils SHARED
    Logging.cc
)
target_link_libraries(geom_utils PRIVATE project_options project_warnings
                                 PUBLIC spdlog::spdlog fmt::fmt)

add_library(geom SHARED
    Vector2D.cc
    Vector3D.cc
    Transform3D.cc
    Element.cc
    Material.cc
    Shape.cc
    World.cc
    Parser.cc
    Volume.cc
)
target_link_libraries(geom PRIVATE project_options project_warnings
                           PUBLIC geom_utils yaml::cpp pugixml::pugixml)

add_executable(geom_test
    main.cc
)
target_link_libraries(geom_test PRIVATE project_options project_warnings
                                PUBLIC geom)
