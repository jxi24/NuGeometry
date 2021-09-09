#include "geom/Element.hh"
#include "geom/Units.hh"
#include "yaml-cpp/yaml.h"

#include <stdexcept>

using NuGeom::Element;

std::map<std::string, Element> Element::common_elements;

Element::Element(const std::string &name) {
    if(common_elements.find(name) == common_elements.end()) {
        throw std::runtime_error("Invalid element " + name);
    }
    *this = common_elements.at(name);
}

Element::Element(const std::string &name, size_t Z,
                 size_t A, double mass) : m_name{name}, m_Z{Z}, m_A{A} {
    if(mass == 0) {
        m_mass = static_cast<double>(A)*Units::amu;
    } else { 
        m_mass = mass;
    }

    if(common_elements.find(name) == common_elements.end()) {
        common_elements[name] = *this;
    }
}

Element::Element(const std::string &name, const std::string &symbol,
                 size_t Z, size_t A, double mass) : m_name{name}, m_symbol{symbol}, m_Z{Z}, m_A{A} {
    if(mass == 0) {
        m_mass = static_cast<double>(A)*Units::amu;
    } else {
        m_mass = mass;
    }

    if(common_elements.find(name) == common_elements.end()) {
        common_elements[name] = *this;        
    }

    if(common_elements.find(symbol) == common_elements.end()) {
        common_elements[symbol] = *this;
    }
}

void NuGeom::LoadElements(const YAML::Node &node) {
    for(const auto &subnode : node) {
        auto name = subnode[0].as<std::string>();
        auto symbol = subnode[1].as<std::string>();
        auto z = subnode[2].as<size_t>();
        auto a = subnode[3].as<size_t>();
        auto m = subnode[4].as<double>();

        Element elm(name, symbol, z, a, m);
        Element::CommonElements()[name] = elm;
        Element::CommonElements()[symbol] = elm;
    }
}
