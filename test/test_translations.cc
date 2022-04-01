#include "catch2/catch.hpp"

#include "geom/Transform3D.hh"
#include <iostream>

TEST_CASE("Single Translation", "[Transform3D]") {
    const NuGeom::Vector3D input{0, 0, 0};
    const NuGeom::Vector3D expected{1, 2, 3};

    SECTION("Single translation") {
        NuGeom::Translation3D trans{1, 2, 3};

        auto result = trans.Apply(input);
        CHECK(result.X() == Approx(expected.X()).margin(1e-10));
        CHECK(result.Y() == Approx(expected.Y()).margin(1e-10));
        CHECK(result.Z() == Approx(expected.Z()).margin(1e-10));
    }

    SECTION("Translation X-axis") {
        NuGeom::TranslationX3D trans(5);

        auto result = trans.Apply(input);
        CHECK(result.X() == Approx(5).margin(1e-10));
    }

    SECTION("Translation Y-axis") {
        NuGeom::TranslationY3D trans(5);

        auto result = trans.Apply(input);
        CHECK(result.Y() == Approx(5).margin(1e-10));
    }

    SECTION("Translation Z-axis") {
        NuGeom::TranslationZ3D trans(5);

        auto result = trans.Apply(input);
        CHECK(result.Z() == Approx(5).margin(1e-10));
    }

    SECTION("Translate and Inverse") {
        NuGeom::Translation3D trans{1, 2, 3};

        auto result = trans.Apply(input);
        auto trans2 = trans.Inverse();
        result = trans2.Apply(result);
        CHECK(result.X() == Approx(0).margin(1e-10));
        CHECK(result.Y() == Approx(0).margin(1e-10));
        CHECK(result.Z() == Approx(0).margin(1e-10));
    }
}
