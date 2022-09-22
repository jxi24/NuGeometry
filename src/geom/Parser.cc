#include "geom/Parser.hh"
#include "spdlog/spdlog.h"
#include <cstring>
#include <cmath>

using NuGeom::GDMLParser;

GDMLParser::GDMLParser(const pugi::xml_document &doc) {
    auto root = doc.child("gdml");
    ParseDefines(root.child("define"));
    ParseMaterials(root.child("materials"));
    ParseSolids(root.child("solids"));
    ParseStructure(root.child("structure"));

    auto setup = root.child("setup");
    m_world = World(m_volumes[setup.child("world").attribute("ref").value()]);

    spdlog::info("Number of constants defined: {}", m_def_constants.size());
    spdlog::info("Number of positions defined: {}", m_def_positions.size());
    spdlog::info("Number of rotations defined: {}", m_def_rotations.size());
    spdlog::info("Number of materials: {}", m_materials.size());
    spdlog::info("Number of solids: {}", m_shapes.size());
    spdlog::info("Number of volumes: {}", m_volumes.size());
    spdlog::info("Number of physical volumes: {}", m_phys_vols.size());
}

double GDMLParser::GetConstant(const std::string &name) const {
    if(m_def_constants.find(name) == m_def_constants.end())
        throw std::runtime_error(fmt::format("GDMLParser: Undefined constant {}", name));

    return m_def_constants.at(name);
}

NuGeom::Vector3D GDMLParser::GetPosition(const std::string &name) const {
    if(m_def_positions.find(name) == m_def_positions.end())
        throw std::runtime_error(fmt::format("GDMLParser: Undefined position {}", name));

    return m_def_positions.at(name);
}

NuGeom::Transform3D GDMLParser::GetTransform(const std::string &name) const {
    if(m_def_rotations.find(name) == m_def_rotations.end())
        throw std::runtime_error(fmt::format("GDMLParser: Undefined transform {}", name));

    return m_def_rotations.at(name);
}

NuGeom::Material GDMLParser::GetMaterial(const std::string &name) const {
    if(m_materials.find(name) == m_materials.end())
        throw std::runtime_error(fmt::format("GDMLParser: Undefined material {}", name));

    return m_materials.at(name);
}

std::vector<NuGeom::Material> GDMLParser::GetMaterials() const {
    std::vector<NuGeom::Material> mats;
    for(const auto &mat : m_materials) mats.push_back(mat.second);
    return mats;
}

void GDMLParser::ParseDefines(const pugi::xml_node &define) {
    for(const auto &node : define.children("constant")) {
        std::string name = node.attribute("name").value();
        double value = node.attribute("value").as_double();
        m_def_constants[name] = value;
    }

    for(const auto &node : define.children("position")) {
        // Load the position information
        std::string name = node.attribute("name").value();
        double x = node.attribute("x").as_double();
        double y = node.attribute("y").as_double();
        double z = node.attribute("z").as_double();
        Vector3D position(x, y, z);

        // Convert the units
        std::string unit = node.attribute("unit").value();
        if(unit == "m") {
            position *= 100; 
        } else if(unit == "mm") {
            position /= 10;
        }
        m_def_positions[name] = position;
    }

    for(const auto &node : define.children("rotation")) {
        // Load the rotation information
        std::string name = node.attribute("name").value();
        std::string unit = node.attribute("unit").value();
        double xRot = node.attribute("x").as_double();
        double yRot = node.attribute("y").as_double();
        double zRot = node.attribute("z").as_double();

        // Convert if needed
        double convert{};
        if(unit == "deg") convert = M_PI/180;
        else if(unit == "rad") convert = 1;
        else throw std::runtime_error("GDMLParser: Invalid angle unit");
        auto rotX = RotationX3D(xRot*convert); 
        auto rotY = RotationY3D(yRot*convert); 
        auto rotZ = RotationZ3D(zRot*convert); 
        Transform3D rot = rotZ*rotY*rotX;
        m_def_rotations[name] = rot;
    }
}

void GDMLParser::ParseMaterials(const pugi::xml_node &materials) {
    for(const auto &node : materials.children("element")) {
        std::string name = node.attribute("name").value();
        std::string symbol = node.attribute("formula").value();
        size_t z = node.attribute("Z").as_ullong();
        double mass = node.child("atom").attribute("value").as_double();
        Element elm(name, symbol, z, mass);
    }

    for(const auto &node : materials.children("material")) {
        std::string name = node.attribute("name").value();
        double density = node.child("D").attribute("value").as_double();
        std::string unit = "g/cm3";
        if(node.child("D").attribute("unit"))
            unit = node.child("D").attribute("unit").value();

        // TODO: Refactor this to make it cleaner
        if(node.child("fraction")) {
            auto nelements = static_cast<size_t>(std::distance(node.children("fraction").begin(),
                                                               node.children("fraction").end()));

            Material material(name, density, nelements);
            for(const auto &element : node.children("fraction")) {
                double fraction = element.attribute("n").as_double();
                // TODO: Allow other materials to be added to a new material
                if(m_materials.find(element.attribute("ref").as_string()) != m_materials.end()) {
                    material.AddMaterial(m_materials[element.attribute("ref").as_string()], fraction);         
                } else {
                    Element elm(element.attribute("ref").as_string());
                    material.AddElement(elm, fraction);
                }
            }
            m_materials[name] = material;
        } else if(node.child("composite")) {
            auto nelements = static_cast<size_t>(std::distance(node.children("composite").begin(),
                                                               node.children("composite").end()));
            Material material(name, density, nelements);
            for(const auto &element : node.children("composite")) {
                int natoms = element.attribute("n").as_int();
                // TODO: Allow other materials to be added to a new material
                if(m_materials.find(element.attribute("ref").as_string()) != m_materials.end()) {
                    throw std::runtime_error("GDMLParser: Using composite materials requires mass fractions");
                } else {
                    Element elm(element.attribute("ref").as_string());
                    material.AddElement(elm, natoms);
                }
            }
            m_materials[name] = material;
        } else {
            throw std::runtime_error("GDMLParser: Invalid material");
        }
    }
}

void GDMLParser::ParseSolids(const pugi::xml_node &solids) {
    for(const auto &solid : solids) {
        std::string name = solid.attribute("name").value();
        // TODO: Implement csg solids
        if(std::strcmp(solid.name(), "subtraction") == 0) {
            std::string first_name = solid.child("first").attribute("ref").value();
            std::string second_name = solid.child("second").attribute("ref").value();
            auto first_shape = m_shapes[first_name];
            auto second_shape = m_shapes[second_name];
            auto shape = std::make_shared<CombinedShape>(first_shape, second_shape, ShapeBinaryOp::kSubtraction);
            m_shapes[name] = shape;
        } else if(std::strcmp(solid.name(), "union") == 0) {
            std::string first_name = solid.child("first").attribute("ref").value();
            std::string second_name = solid.child("second").attribute("ref").value();
            auto first_shape = m_shapes[first_name];
            auto second_shape = m_shapes[second_name];
            auto shape = std::make_shared<CombinedShape>(first_shape, second_shape, ShapeBinaryOp::kUnion);
            m_shapes[name] = shape;
        } else if(std::strcmp(solid.name(), "intersection") == 0) {
            std::string first_name = solid.child("first").attribute("ref").value();
            std::string second_name = solid.child("second").attribute("ref").value();
            auto first_shape = m_shapes[first_name];
            auto second_shape = m_shapes[second_name];
            auto shape = std::make_shared<CombinedShape>(first_shape, second_shape, ShapeBinaryOp::kIntersect);
            m_shapes[name] = shape;
        } else {
            std::shared_ptr<Shape> shape = ShapeFactory::Initialize(solid.name(), solid); 
            m_shapes[name] = shape;
        }
    }
}

void GDMLParser::ParseStructure(const pugi::xml_node &structure) {
    for(const auto &node : structure.children("volume")) {
        std::string name = node.attribute("name").value(); 
        std::string material_ref = node.child("materialref").attribute("ref").value();
        std::string solid_ref = node.child("solidref").attribute("ref").value();
        Material material = m_materials[material_ref];
        auto shape = m_shapes[solid_ref];
        auto volume = std::make_shared<LogicalVolume>(material, shape);
        
        // Check for sub-volumes
        for(const auto &subnode : node.children("physvol")) {
            std::string volume_ref = subnode.child("volumeref").attribute("ref").value();

            Vector3D translation;
            if(subnode.child("positionref")) {
                std::string position_ref = subnode.child("positionref").attribute("ref").value();
                translation = m_def_positions[position_ref];
            } else if(subnode.child("position")) {
                double x = subnode.attribute("x").as_double();
                double y = subnode.attribute("y").as_double();
                double z = subnode.attribute("z").as_double();
                translation = Vector3D(x, y, z);

                // Convert the units
                std::string unit = subnode.attribute("unit").value();
                if(unit == "m") {
                    translation *= 100; 
                } else if(unit == "mm") {
                    translation /= 10;
                }
            }

            Transform3D rotation;
            if(subnode.child("rotationref")) {
                rotation = m_def_rotations[subnode.child("rotationref").attribute("ref").value()];
            } else if(subnode.child("rotation")) {
                double xRot = node.attribute("x").as_double();
                double yRot = node.attribute("y").as_double();
                double zRot = node.attribute("z").as_double();

                // Convert if needed
                double convert = 1;
                if(subnode.attribute("unit")) {
                    std::string unit = subnode.attribute("unit").value();
                    if(unit == "deg") convert = M_PI/180;
                    else if(unit == "rad") convert = 1;
                    else throw std::runtime_error("GDMLParser: Invalid angle unit: " + unit);
                }
                auto rotX = RotationX3D(xRot*convert); 
                auto rotY = RotationY3D(yRot*convert); 
                auto rotZ = RotationZ3D(zRot*convert); 
                rotation = rotZ*rotY*rotX;
            }

            auto subvolume = m_volumes[volume_ref];
            subvolume -> SetMother(volume);
            auto phys_vol = std::make_shared<PhysicalVolume>(subvolume, Translation3D(translation), rotation);
            m_phys_vols.push_back(phys_vol);
            volume -> AddDaughter(m_phys_vols.back());
        }
      
        spdlog::info("Volume: {}", name);
        spdlog::info("  Mass = {}", volume -> Mass());
        // Store volume information
        m_volumes[name] = volume;
    }
}
