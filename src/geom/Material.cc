#include "geom/Material.hh"
#include "geom/Utilities.hh"
#include <numeric>
#include <stdexcept>

using NuGeom::SimpleMaterial;

void SimpleMaterial::AddElement(const Element &elm, int natoms) {
    if(m_elements.size() >= NComponents())
        throw std::runtime_error("Too many elements added to material. Increase ncomponents");

    if(m_natoms.size() != m_elements.size())
        throw std::runtime_error("Either initialize with material with all fractions or natoms. Do not mix");

    m_elements.push_back(elm);
    m_natoms.push_back(natoms);

    // Calculate fractions
    if(m_elements.size() == NComponents()) {

        double total_mass{};
        for(size_t i = 0; i < NComponents(); ++i) {
            m_fractions.push_back(m_elements[i].Mass() * static_cast<double>(m_natoms[i]));
            total_mass += m_fractions.back();
        }

        for(size_t i = 0; i < NComponents(); ++i) {
            m_fractions[i] /= total_mass;
        }
    }
}

void SimpleMaterial::AddElement(const Element &elm, double fraction) {
    if(m_elements.size() >= NComponents())
        throw std::runtime_error("Too many elements added to material. Increase ncomponents");

    if(m_fractions.size() != m_elements.size())
        throw std::runtime_error("Either initialize with material with all fractions or natoms. Do not mix");

    m_elements.push_back(elm);
    m_fractions.push_back(fraction);

    // Calculate fractions
    if(m_elements.size() == NComponents()) {
        auto sum = std::accumulate(m_fractions.begin(), m_fractions.end(), 0.0);
        if(!NuGeom::is_close(sum, 1.0)) {
            // TODO: Clean up with fmt library
            throw std::runtime_error("Mass fractions sum to " + std::to_string(sum) + " and not 1");
        }
    }
}

NuGeom::Element SimpleMaterial::SelectElement(double ran) const {
    if(m_elements.size() != NComponents())
        throw std::runtime_error("Material does not have the right number of elements!");

    double sum = m_fractions[0];
    size_t idx = 0;
    while(true) {
        if(ran < sum) return m_elements[idx];
        sum += m_fractions[++idx];
    }
}
