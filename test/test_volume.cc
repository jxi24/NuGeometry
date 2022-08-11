#include "catch2/catch.hpp"

#include "geom/Volume.hh"
#include "geom/Ray.hh"
#include "geom/LineSegment.hh"

using NuGeom::LogicalVolume;
using NuGeom::PhysicalVolume;

TEST_CASE("Single LogicalVolume", "[Volume]") {
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

TEST_CASE("Intersect", "[Volume]") {
    NuGeom::Material mat("Water", 1.0, 2);
    mat.AddElement(NuGeom::Element("Hydrogen", 1, 1), 2);
    mat.AddElement(NuGeom::Element("Oxygen", 8, 16), 1);
    auto box = std::make_shared<NuGeom::Box>();
    auto vol = std::make_shared<LogicalVolume>(mat, box); 
    PhysicalVolume pvol(vol, NuGeom::Transform3D{}, NuGeom::Transform3D{});

    SECTION("Outside Volume") {
        NuGeom::Ray ray({0, 0, -1}, {0, 0, 1});
        static constexpr double expected_time = 0.5;
        CHECK_THAT(pvol.Intersect(ray), Catch::WithinAbs(expected_time, 1e-8));
    }

    SECTION("Inside Volume") {
        static constexpr double eps = 1e-7;
        NuGeom::Ray ray({0, 0, -0.5+eps}, {0, 0, 1});
        static constexpr double expected_time = 1.0-eps;
        CHECK_THAT(pvol.Intersect(ray), Catch::WithinAbs(expected_time, 1e-8));
    }
}

TEST_CASE("Line Segments", "[Volume]") {
    NuGeom::Material mat("Water", 1.0, 2);
    mat.AddElement(NuGeom::Element("Hydrogen", 1, 1), 2);
    mat.AddElement(NuGeom::Element("Oxygen", 8, 16), 1);
    auto inner_box = std::make_shared<NuGeom::Box>();
    auto inner_vol = std::make_shared<LogicalVolume>(mat, inner_box); 
    NuGeom::RotationX3D rot(45*M_PI/180.0);
    auto inner_pvol = std::make_shared<PhysicalVolume>(inner_vol, NuGeom::Transform3D{}, rot);

    auto outer_box = std::make_shared<NuGeom::Box>(NuGeom::Vector3D{2, 2, 2});
    auto outer_vol = std::make_shared<LogicalVolume>(mat, outer_box); 
    outer_vol->AddDaughter(inner_pvol);
    inner_vol->SetMother(outer_vol);
    NuGeom::RotationX3D rot2(90*M_PI/180.0);
    auto outer_pvol = std::make_shared<PhysicalVolume>(outer_vol, NuGeom::Transform3D{}, rot2);
    inner_pvol->SetMother(outer_pvol);

    auto world_box = std::make_shared<NuGeom::Box>(NuGeom::Vector3D{4, 4, 4});
    auto world = std::make_shared<LogicalVolume>(mat, world_box);
    outer_vol->SetMother(world);
    world -> AddDaughter(outer_pvol);

    NuGeom::Ray ray({0, 0, -2}, {0, 0, 1});
    std::vector<NuGeom::LineSegment> segments;

    world->GetLineSegments(ray, segments);

    CHECK(segments.size() == 5);
    CHECK_THAT(segments[0].Length(), Catch::WithinAbs(1, 1e-8));
    CHECK_THAT(segments[1].Length(), Catch::WithinAbs(1-sqrt(2)/2, 1e-8));
    CHECK_THAT(segments[2].Length(), Catch::WithinAbs(sqrt(2), 1e-8));
    CHECK_THAT(segments[3].Length(), Catch::WithinAbs(1-sqrt(2)/2, 1e-8));
    CHECK_THAT(segments[4].Length(), Catch::WithinAbs(1, 1e-8));

    CHECK_THAT(segments[0].Start().Z(), Catch::WithinAbs(-2, 1e-8));
    CHECK_THAT(segments[1].Start().Z(), Catch::WithinAbs(segments[0].End().Z(), 1e-8));
    CHECK_THAT(segments[2].Start().Z(), Catch::WithinAbs(segments[1].End().Z(), 1e-8));
    CHECK_THAT(segments[3].Start().Z(), Catch::WithinAbs(segments[2].End().Z(), 1e-8));
    CHECK_THAT(segments[4].Start().Z(), Catch::WithinAbs(segments[3].End().Z(), 1e-8));
    CHECK_THAT(segments[4].End().Z(), Catch::WithinAbs(2, 1e-8));
    CHECK_THAT(segments[1].Start().Z(), Catch::WithinAbs(-1, 1e-8));
}
