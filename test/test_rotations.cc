#include "catch2/catch.hpp"

#include "geom/Transform3D.hh"
#include <iostream>

TEST_CASE("Single Rotations", "[Transform3D]") {
    const NuGeom::Vector3D input{1, 0, 0};
    const NuGeom::Vector3D expected{0, 1, 0};

    SECTION("Single rotation from axis-angle") {
        NuGeom::Vector3D axis{0, 0, 1};
        NuGeom::Rotation3D rot{axis, M_PI_2};

        auto result = rot.Apply(input);
        CHECK(result.X() == Approx(expected.X()).margin(1e-10));
        CHECK(result.Y() == Approx(expected.Y()).margin(1e-10));
        CHECK(result.Z() == Approx(expected.Z()).margin(1e-10));
    }

    SECTION("Single rotation from Euler-zyx") {
        NuGeom::Rotation3D rot{M_PI_2, 0, 0};

        auto result = rot.Apply(input);
        CHECK(result.X() == Approx(expected.X()).margin(1e-10));
        CHECK(result.Y() == Approx(expected.Y()).margin(1e-10));
        CHECK(result.Z() == Approx(expected.Z()).margin(1e-10));
    }

    SECTION("Single rotation from Euler-zxz") {
        NuGeom::Rotation3D rot{M_PI_2, 0, 0, NuGeom::Rotation3D::Euler::ZXZ};

        auto result = rot.Apply(input);
        CHECK(result.X() == Approx(expected.X()).margin(1e-10));
        CHECK(result.Y() == Approx(expected.Y()).margin(1e-10));
        CHECK(result.Z() == Approx(expected.Z()).margin(1e-10));
    }
}

TEST_CASE("Multiple Rotations") {
    SECTION("Combining rotations") {
        const NuGeom::Vector3D input{1, 0, 0};
        const NuGeom::Vector3D expected{0, 0, 1};

        NuGeom::Vector3D axis1{0, 0, 1};
        NuGeom::Vector3D axis2{0, 1, 0};
        NuGeom::Rotation3D rot1{axis1, M_PI_2};
        NuGeom::Rotation3D rot2{axis2, -M_PI_2};
        auto rot = rot1*rot2;

        auto result = rot.Apply(input);
        CHECK(result.X() == Approx(expected.X()).margin(1e-10));
        CHECK(result.Y() == Approx(expected.Y()).margin(1e-10));
        CHECK(result.Z() == Approx(expected.Z()).margin(1e-10));
    }
}
