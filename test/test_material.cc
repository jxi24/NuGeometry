#include "catch2/catch.hpp"

#include "geom/Material.hh"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/ostream_sink.h"

#include <sstream>

// Make logger send to stringstream
static std::ostringstream test_logger() {
    std::ostringstream oss;
    auto oss_sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss);
    auto oss_logger = std::make_shared<spdlog::logger>("test_log", oss_sink);
    oss_logger -> set_level(spdlog::level::debug);
    spdlog::register_logger(oss_logger);
    spdlog::set_default_logger(oss_logger);
    return oss;
}

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

        auto oss = test_logger();
        NuGeom::Material dummy("dummy", 1, 2);
        dummy.AddElement(NuGeom::Element("Nitrogen"), 0.7);
        dummy.AddElement(NuGeom::Element("Oxygen"), 0.7);
        CHECK_THAT(oss.str(), Catch::Contains("Mass fractions sum to 1.4 and not 1"));
    }
}
