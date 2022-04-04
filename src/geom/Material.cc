#include "geom/Material.hh"
#include "geom/Utilities.hh"

#include "spdlog/spdlog.h"

#include <algorithm>
#include <iostream>
#include <numeric>
#include <stdexcept>

using NuGeom::Material;

void Material::AddElement(const Element &elm, int natoms) {
    if(m_elements.size() >= m_ncomponents)
        throw std::runtime_error("Too many elements added to material. Increase ncomponents");

    if(m_natoms.size() != m_elements.size())
        throw std::runtime_error("Either initialize with material with all fractions or natoms. Do not mix");

    m_elements.push_back(elm);
    m_natoms.push_back(natoms);

    // Calculate fractions
    if(m_elements.size() == m_ncomponents) {

        double total_mass{};
        for(size_t i = 0; i < m_ncomponents; ++i) {
            m_fractions.push_back(m_elements[i].Mass() * static_cast<double>(m_natoms[i]));
            total_mass += m_fractions.back();
        }

        for(size_t i = 0; i < m_ncomponents; ++i) {
            m_fractions[i] /= total_mass;
        }
    }
}

void Material::AddElement(const Element &elm, double fraction) {
    if(m_elements.size() >= m_ncomponents)
        throw std::runtime_error("Too many elements added to material. Increase ncomponents");

    if(m_fractions.size() != m_elements.size())
        throw std::runtime_error("Either initialize with material with all fractions or natoms. Do not mix");

    m_elements.push_back(elm);
    m_fractions.push_back(fraction);

    // Calculate fractions
    if(m_elements.size() == m_ncomponents) {
        auto sum = std::accumulate(m_fractions.begin(), m_fractions.end(), 0.0);
        if(!NuGeom::is_close(sum, 1.0, 1e-4)) {
            spdlog::warn("Material: Mass fractions sum to {} and not 1", sum);
        }
    }
}

void Material::AddMaterial(const Material &mat, double fraction) {
    if(m_elements.size() >= m_ncomponents)
        throw std::runtime_error("Too many elements added to material. Increase ncomponents");

    if(m_fractions.size() != m_elements.size())
        throw std::runtime_error("Add material requires the use of fractions for all elements");

    m_ncomponents += mat.NElements() - 1;

    size_t idx = 0;
    for(const auto &elm : mat.Elements()) {
        auto it = std::find(m_elements.begin(), m_elements.end(), elm);
        if(it != m_elements.end()) {
            m_ncomponents--;
            auto dist = static_cast<size_t>(std::distance(m_elements.begin(), it));
            m_fractions[dist] += fraction*mat.MassFractions()[idx++];
        } else {
            AddElement(elm, fraction*mat.MassFractions()[idx++]);
        }
    }
}

NuGeom::Element Material::SelectElement(double ran) const {
    if(m_elements.size() != m_ncomponents)
        throw std::runtime_error("Material does not have the right number of elements!");

    double sum = m_fractions[0];
    size_t idx = 0;
    while(true) {
        if(ran < sum) return m_elements[idx];
        sum += m_fractions[++idx];
    }
}
