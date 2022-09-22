#pragma once

#include "geom/Vector3D.hh"
#include "geom/Transform3D.hh"
#include "geom/Element.hh"
#include "geom/Material.hh"
#include "geom/Shape.hh"
#include "geom/Volume.hh"
#include "geom/World.hh"

#include "pugixml.hpp"

#include <map>
#include <memory>
#include <string>

namespace NuGeom {

class Parser {

};

class GDMLParser : public Parser {
    public:
        GDMLParser(const pugi::xml_document&);
        World GetWorld() const { return m_world; }
        double GetConstant(const std::string&) const;
        Vector3D GetPosition(const std::string&) const;
        Transform3D GetTransform(const std::string&) const;
        Material GetMaterial(const std::string&) const;
        std::vector<Material> GetMaterials() const;

    private:
        void ParseDefines(const pugi::xml_node&);
        void ParseMaterials(const pugi::xml_node&);
        void ParseSolids(const pugi::xml_node&);
        void ParseStructure(const pugi::xml_node&);

        std::map<std::string, double> m_def_constants;
        std::map<std::string, Vector3D> m_def_positions;
        std::map<std::string, Transform3D> m_def_rotations;
        std::map<std::string, Material> m_materials;
        std::map<std::string, std::shared_ptr<Shape>> m_shapes;
        std::map<std::string, std::shared_ptr<LogicalVolume>> m_volumes;
        std::vector<std::shared_ptr<PhysicalVolume>> m_phys_vols;

        World m_world;
};

}
