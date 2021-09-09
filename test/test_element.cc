#include "catch2/catch.hpp"
#include "geom/Element.hh"

TEST_CASE("Elements", "[Materials]") {
    SECTION("Saving Elements to map works") {
        NuGeom::Element hydrogen("hydrogen", "H", 1, 1);
        CHECK(NuGeom::Element::CommonElements().size() == 2);
        NuGeom::Element hydrogen2("hydrogen");
        NuGeom::Element hydrogen3("H");
        CHECK(hydrogen.Z() == hydrogen2.Z());
        CHECK(hydrogen.Z() == hydrogen3.Z());
        CHECK(hydrogen.A() == hydrogen2.A());
        CHECK(hydrogen.A() == hydrogen3.A());
        CHECK(hydrogen.Mass() == hydrogen2.Mass());
        CHECK(hydrogen.Mass() == hydrogen3.Mass());
        CHECK(hydrogen.Name() == hydrogen2.Name());
        CHECK(hydrogen.Name() == hydrogen3.Name());
        CHECK(hydrogen.Symbol() == hydrogen2.Symbol());
        CHECK(hydrogen.Symbol() == hydrogen3.Symbol());
    }
}
