#include "catch2/catch.hpp"

#include "geom/Volume.hh"

using NuGeom::LogicalVolume;

TEST_CASE("Single LogicalVolume") {
    NuGeom::Material mat("Water", 1.0, 2);
    mat.AddElement(NuGeom::Element("Hydrogen", 1, 1), 2);
    mat.AddElement(NuGeom::Element("Oxygen", 8, 16), 1);
    auto box = std::make_shared<NuGeom::Box>();
    LogicalVolume vol(mat, box); 

    SECTION("Volume is correct") {
        CHECK(vol.Volume() == Approx(1));
    }

    SECTION("Mass is correct") {
        CHECK(vol.Mass() == Approx(1));
    }
}
