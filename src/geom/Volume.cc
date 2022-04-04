#include "geom/Volume.hh"

#include <numeric>

using NuGeom::LogicalVolume;
using NuGeom::PhysicalVolume;

double LogicalVolume::Mass() const {
    return Volume() * m_material.Density()
        + DaughterMass();
}

double AddVolume(double a, const std::shared_ptr<PhysicalVolume> &b) {
    return a + b -> GetLogicalVolume() -> Volume();
}

double LogicalVolume::DaughterVolumes() const {
    return std::accumulate(m_daughters.begin(),
                           m_daughters.end(),
                           0.0, AddVolume);
}

double LogicalVolume::Volume() const {
    return m_shape -> Volume() - DaughterVolumes();
}

double AddMass(double a, const std::shared_ptr<PhysicalVolume> &b) {
    return a + b -> GetLogicalVolume() -> Mass();
}

double LogicalVolume::DaughterMass() const {
    return std::accumulate(m_daughters.begin(),
                           m_daughters.end(),
                           0.0, AddMass);
}
