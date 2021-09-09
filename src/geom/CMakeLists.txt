add_library(geom SHARED
    Vector3D.cc
    Transform3D.cc
    Element.cc
    Material.cc
)
target_link_libraries(geom PRIVATE project_options project_warnings
                           PUBLIC yaml::cpp)
