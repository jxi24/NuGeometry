#include "geom/Element.hh"
#include "geom/Units.hh"
// #include "yaml-cpp/yaml.h"

#include <stdexcept>

using NuGeom::Element;

Element::Element(const std::string &name) {
    if(CommonElements().find(name) == CommonElements().end()) {
        throw std::runtime_error("Invalid element " + name);
    }
    *this = CommonElements().at(name);
}

Element::Element(const std::string &name, size_t Z,
                 double mass, size_t A) : m_name{name}, m_Z{Z}, m_mass{mass} {
    if(A == 0) {
        m_A = static_cast<size_t>(mass);
    } else {
        m_A = A;
    }

    if(CommonElements().find(name) == CommonElements().end()) {
        CommonElements()[name] = *this;
    }
}

Element::Element(const std::string &name, const std::string &symbol,
                 size_t Z, double mass, size_t A) : m_name{name}, m_symbol{symbol}, m_Z{Z}, m_mass{mass} {
    if(A == 0) {
        m_A = static_cast<size_t>(mass);
    } else {
        m_A = A;
    }

    if(CommonElements().find(name) == CommonElements().end()) {
        CommonElements()[name] = *this;        
    }

    if(CommonElements().find(symbol) == CommonElements().end()) {
        CommonElements()[symbol] = *this;
    }
}

// void NuGeom::LoadElements(const YAML::Node &node) {
//     for(const auto &subnode : node) {
//         auto name = subnode[0].as<std::string>();
//         auto symbol = subnode[1].as<std::string>();
//         auto z = subnode[2].as<size_t>();
//         auto a = subnode[3].as<size_t>();
//         auto m = subnode[4].as<double>();
// 
//         Element elm(name, symbol, z, a, m);
//         Element::CommonElements()[name] = elm;
//         Element::CommonElements()[symbol] = elm;
//     }
// }
