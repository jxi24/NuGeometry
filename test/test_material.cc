#include "catch2/catch.hpp"

#include "geom/Material.hh"

TEST_CASE("Material", "[Materials]") {
    SECTION("Making Material Works as expected") {
        NuGeom::Material water("water", 1.0, 2);
        CHECK(water.Name() == "water");
        CHECK(water.NComponents() == 2);
        CHECK(water.Density() == 1.0);

        water.AddElement(NuGeom::Element("Hydrogen", 1, 1), 2);
        CHECK_THROWS_WITH(water.AddElement(NuGeom::Element("Dummy", 2, 2), 0.5),
                          Catch::Contains("fractions or natoms"));
        water.AddElement(NuGeom::Element("Oxygen", 8, 16), 1);

        auto elm = water.SelectElement(0.5);
        CHECK(elm.Name() == "Oxygen");

        CHECK_THROWS_WITH(water.AddElement(NuGeom::Element("Dummy", 2, 2), 2),
                          Catch::Contains("Too many elements added"));

        NuGeom::Material air("air", 0.00129, 2);
        CHECK(air.Name() == "air");
        CHECK(air.NComponents() == 2);
        CHECK(air.Density() == 0.00129);

        air.AddElement(NuGeom::Element("Nitrogen", 7, 14), 0.7);
        CHECK_THROWS_WITH(air.AddElement(NuGeom::Element("Dummy", 2, 2), 2),
                          Catch::Contains("fractions or natoms"));
        air.AddElement(NuGeom::Element("Oxygen"), 0.3);

        elm = air.SelectElement(0.3);
        CHECK(elm.Name() == "Nitrogen");

        CHECK_THROWS_WITH(air.AddElement(NuGeom::Element("Dummy", 2, 2), 0.5),
                          Catch::Contains("Too many elements added"));

        // TODO: Change to check for warning printed to stdout about mass fraction > 1
        // NuGeom::Material dummy("dummy", 1, 2);
        // dummy.AddElement(NuGeom::Element("Nitrogen"), 0.7);
        // CHECK_THROWS_WITH(dummy.AddElement(NuGeom::Element("Oxygen"), 0.7),
        //                   Catch::Contains("Mass fractions sum to"));
    }
}
