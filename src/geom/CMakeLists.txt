add_library(geom SHARED
    Vector2D.cc
    Vector3D.cc
    Transform3D.cc
    Element.cc
    Material.cc
    Shape.cc
    World.cc
    Parser.cc
)
target_link_libraries(geom PRIVATE project_options project_warnings
                           PUBLIC yaml::cpp pugixml::pugixml)

add_executable(geom_test
    main.cc
)
target_link_libraries(geom_test PRIVATE project_options project_warnings
                                PUBLIC geom)
