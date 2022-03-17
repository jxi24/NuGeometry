#pragma once

#include "geom/Element.hh"

#include <vector>

namespace NuGeom {

/*
class BaseMaterial {
    public:
        BaseMaterial() = default;
        BaseMaterial(const std::string &name, size_t ncomponents)
            : m_name{name}, m_ncomponents{ncomponents} {}
        virtual ~BaseMaterial() {}
        virtual std::vector<Element> Elements() const = 0;
        virtual void AddElement(const Element&, int) = 0;
        virtual void AddElement(const Element&, double) = 0;
        virtual Element SelectElement(double) const = 0;
        virtual double Density() const = 0;
        size_t NComponents() const { return m_ncomponents; }
        std::string Name() const { return m_name; }

    private:
        std::string m_name{};
        size_t m_ncomponents{};
}; */

class Material {
    public:
        Material() = default; 
        Material(const std::string &name, double density, size_t ncomponents)
            : m_name{name}, m_density{density}, m_ncomponents{ncomponents} {}

        size_t NComponents() const { return m_ncomponents; }
        std::vector<Element> Elements() const { return m_elements; }
        std::vector<double> MassFractions() const { return m_fractions; }
        size_t NElements() const { return m_elements.size(); }
        void AddElement(const Element&, int);
        void AddElement(const Element&, double);
        void AddMaterial(const Material&, double);
        Element SelectElement(double) const;
        double Density() const { return m_density; }
        std::string Name() const { return m_name; }

        template<typename OStream>
        friend OStream& operator<<(OStream &os, const Material &material) {
            os << "Material:\n";
            os << "  Name: " << material.Name() << "\n";
            os << "  Density: " << material.m_density << "\n"; 
            os << "  NComponents: " << material.m_ncomponents << "\n";
            os << "  Elements:\n";
            bool component = material.m_natoms.size() != 0;
            size_t idx = 0;
            for(const auto &elm : material.m_elements) {
                ++idx;
                if(component) {
                    os << "    - " << idx << ": " << elm << " " << material.m_natoms[idx-1] << "\n";
                } else {
                    os << "    - " << idx << ": " << elm << " " << material.m_fractions[idx-1] << "\n";
                }
            }
            return os;
        }

    private:
        std::string m_name;
        std::vector<Element> m_elements;
        std::vector<double> m_fractions;
        std::vector<int> m_natoms;
        double m_density;
        size_t m_ncomponents;

        static std::map<std::string, Material> s_materials;
};

/*
// TODO: Wrapper to interface with ROOT TGeoMaterial
class RootMaterial : public BaseMaterial {
    public:
        std::vector<Element> Elements() const override { return {}; }
        void AddElement(const Element &elm, int natoms) override { }
        void AddElement(const Element &elm, double fraction) override { }
        Element SelectElement(double ran) const override { return Element(); }
        double Density() const override { return 0; }
};

// TODO: Wrapper to interface with GEANT G4Material
class GeantMaterial : public BaseMaterial {
    public:
        std::vector<Element> Elements() const override { return {}; }
        void AddElement(const Element &elm, int natoms) override { }
        void AddElement(const Element &elm, double fraction) override { }
        Element SelectElement(double ran) const override { return Element(); }
        double Density() const override { return 0; }

};
*/

}
