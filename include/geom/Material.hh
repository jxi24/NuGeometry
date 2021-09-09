#pragma once

#include "geom/Element.hh"

#include <vector>

namespace NuGeom {

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
};

class SimpleMaterial : public BaseMaterial {
    public:
        SimpleMaterial() = default; 
        SimpleMaterial(const std::string &name, double density, size_t ncomponents) 
            : BaseMaterial(name, ncomponents), m_density{density} {}

        std::vector<Element> Elements() const override { return m_elements; }
        void AddElement(const Element&, int) override;
        void AddElement(const Element&, double) override;
        Element SelectElement(double) const override;
        double Density() const override { return m_density; }

    private:
        std::vector<Element> m_elements;
        std::vector<double> m_fractions;
        std::vector<int> m_natoms;
        double m_density;
};

// TODO: Wrapper to interface with ROOT TGeoMaterial
class RootMaterial : public BaseMaterial {
    public:
        std::vector<Element> Elements() const override { return {}; }
        void AddElement(const Element &/*elm*/, int /*natoms*/) override { }
        void AddElement(const Element &/*elm*/, double /*fraction*/) override { }
        Element SelectElement(double /*ran*/) const override { return Element(); }
        double Density() const override { return 0; }
};

// TODO: Wrapper to interface with GEANT G4Material
class GeantMaterial : public BaseMaterial {
    public:
        std::vector<Element> Elements() const override { return {}; }
        void AddElement(const Element &/*elm*/, int /*natoms*/) override { }
        void AddElement(const Element &/*elm*/, double /*fraction*/) override { }
        Element SelectElement(double /*ran*/) const override { return Element(); }
        double Density() const override { return 0; }

};

}
