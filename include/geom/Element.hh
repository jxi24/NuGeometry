#pragma once

#include <map>
#include <string>

namespace YAML {
class Node;
}

namespace NuGeom {

class Element {
    private:
        std::string m_name{};
        std::string m_symbol{};
        size_t m_Z{};
        size_t m_A{};
        double m_mass{};

        static std::map<std::string, Element> common_elements;

    public:
        Element() = default;
        Element(const std::string&);
        Element(const std::string&, size_t, size_t, double=0);
        Element(const std::string&, const std::string&,
                size_t, size_t, double=0);
        Element(const Element&) = default;
        Element& operator=(const Element &) = default;
        Element(Element&&) = default;
        Element& operator=(Element &&) = default;

        std::string Name() const { return m_name; }
        std::string Symbol() const { return m_symbol; }
        size_t Z() const { return m_Z; }
        size_t A() const { return m_A; }
        double Mass() const { return m_mass; }

        static std::map<std::string, Element> &CommonElements() {
            return common_elements;
        }
};

void LoadElements(const YAML::Node&);

}
