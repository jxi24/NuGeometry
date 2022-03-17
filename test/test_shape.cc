#include "catch2/catch.hpp"
#include "geom/Shape.hh"

TEST_CASE("Box", "[Shapes]") {
    SECTION("SDF is correct") {
        NuGeom::Box box;
        NuGeom::Vector3D point;
        CHECK(box.SignedDistance(point) == -0.5);
        point = NuGeom::Vector3D(0.5, 0.5, 0.5);
        CHECK(box.SignedDistance(point) == 0);
        point = NuGeom::Vector3D(1.5, 0.5, 0.5);
        CHECK(box.SignedDistance(point) == 1);
    }

    SECTION("Translated box") {
        NuGeom::Box box{{1, 1, 1}, {}, {1, 2, 3}};
        NuGeom::Vector3D point{1, 2, 3};
        CHECK(box.SignedDistance(point) == -0.5);
        point = NuGeom::Vector3D(1.5, 2.5, 3.4);
        CHECK(box.SignedDistance(point) == 0);
        point = NuGeom::Vector3D(2.5, 2.5, 3.5);
        CHECK(box.SignedDistance(point) == 1);
    }

    SECTION("Rotated box") {
        NuGeom::Box box{{1, 1, 1}, {{0, 0, 1}, 45*M_PI/180}};
        NuGeom::Vector3D point;
        CHECK(box.SignedDistance(point) == -0.5);
        point = NuGeom::Vector3D(sqrt(2)/2, 0, 0);
        CHECK(box.SignedDistance(point) == Approx(0).margin(1e-15));
        point = NuGeom::Vector3D(0, 0, 1.5);
        CHECK(box.SignedDistance(point) == 1);
    }
}

TEST_CASE("Sphere", "[Shapes]") {
    SECTION("SDF is correct") {
        NuGeom::Sphere sphere;
        NuGeom::Vector3D point;
        CHECK(sphere.SignedDistance(point) == -1);
        point = NuGeom::Vector3D(0, 1, 0);
        CHECK(sphere.SignedDistance(point) == 0);
        point = NuGeom::Vector3D(1, 1, 1);
        CHECK(sphere.SignedDistance(point) == sqrt(3) - 1);
    }

    SECTION("Translated sphere") {
        NuGeom::Sphere sphere{1, {}, {1, 2, 3}};
        NuGeom::Vector3D point{1, 2, 3};
        CHECK(sphere.SignedDistance(point) == -1);
        point = NuGeom::Vector3D(1, 3, 3);
        CHECK(sphere.SignedDistance(point) == 0);
        point = NuGeom::Vector3D(2, 3, 4);
        CHECK(sphere.SignedDistance(point) == sqrt(3) - 1);
    }

    SECTION("Rotated sphere") {
        NuGeom::Sphere sphere{1, {{0, 0, 1}, 45*M_PI/180}};
        NuGeom::Vector3D point;
        CHECK(sphere.SignedDistance(point) == -1);
        point = NuGeom::Vector3D(0, 1, 0);
        CHECK(sphere.SignedDistance(point) == 0);
        point = NuGeom::Vector3D(1, 1, 1);
        CHECK(sphere.SignedDistance(point) == sqrt(3) - 1);
    }
}

TEST_CASE("Cylinder", "[Shapes]") {
    SECTION("SDF is correct") {
        NuGeom::Cylinder cylinder;
        NuGeom::Vector3D point;
        CHECK(cylinder.SignedDistance(point) == -1);
        point = NuGeom::Vector3D(1.0/sqrt(2), 1.0/sqrt(2), 0);
        CHECK(cylinder.SignedDistance(point) == Approx(0.0).margin(1e-15));
        point = NuGeom::Vector3D(1, 0, 2);
        CHECK(cylinder.SignedDistance(point) == 1);
    }

    SECTION("Translated cylinder") {
        NuGeom::Cylinder cylinder{1, 1, {}, {1, 2, 3}};
        NuGeom::Vector3D point{1, 2, 3};
        CHECK(cylinder.SignedDistance(point) == -1);
        point = NuGeom::Vector3D(1+1.0/sqrt(2.0), 2+1.0/sqrt(2.0), 3);
        CHECK(cylinder.SignedDistance(point) == Approx(0.0).margin(1e-15));
        point = NuGeom::Vector3D(2, 2, 5);
        CHECK(cylinder.SignedDistance(point) == 1);
    }
    
    SECTION("Rotated cylinder") {
        NuGeom::Cylinder cylinder{1, 1, {{0, 1, 0}, M_PI/2}};
        NuGeom::Vector3D point;
        CHECK(cylinder.SignedDistance(point) == -1);
        point = NuGeom::Vector3D(0, 1.0/sqrt(2), 1.0/sqrt(2));
        CHECK(cylinder.SignedDistance(point) == Approx(0.0).margin(1e-15));
        point = NuGeom::Vector3D(2, 1, 0);
        CHECK(cylinder.SignedDistance(point) == 1);
    }
}

class PointGenerator : public Catch::Generators::IGenerator<NuGeom::Vector3D> {
    std::minstd_rand m_rand;
    std::uniform_real_distribution<double> m_dist;
    NuGeom::Vector3D current_point;

    public:
        PointGenerator(double low, double high):
            m_rand{std::random_device{}()},
            m_dist{low, high} {
                static_cast<void>(next());
        }
 
        NuGeom::Vector3D const& get() const override;
        bool next() override {
            current_point.X() = m_dist(m_rand);
            current_point.Y() = m_dist(m_rand);
            current_point.Z() = m_dist(m_rand);
            return true;
        }
};

NuGeom::Vector3D const& PointGenerator::get() const {
    return current_point;
}

Catch::Generators::GeneratorWrapper<NuGeom::Vector3D> randomPoint(double low, double high) {
    return Catch::Generators::GeneratorWrapper<NuGeom::Vector3D>(
            std::unique_ptr<Catch::Generators::IGenerator<NuGeom::Vector3D>>(new PointGenerator(low, high)));
}

TEST_CASE("Combined Shape", "[Shapes]") {
    NuGeom::Vector3D size{2, 2, 2};
    NuGeom::Rotation3D rotation;
    NuGeom::Translation3D translation{0, 0, 2};
    // TODO: Switch to mock shapes?
    auto box = std::make_shared<NuGeom::Box>(size);
    auto sphere = std::make_shared<NuGeom::Sphere>(1, rotation, translation);
    NuGeom::Vector3D point = GENERATE(take(30, randomPoint(-5, 5)));

    SECTION("Single Union") {
        NuGeom::CombinedShape shape(box, sphere, NuGeom::ShapeBinaryOp::kUnion);
        CHECK(shape.SignedDistance(point) == std::min(box -> SignedDistance(point), sphere -> SignedDistance(point)));
        NuGeom::CombinedShape shape2(sphere, box, NuGeom::ShapeBinaryOp::kUnion);
        CHECK(shape.SignedDistance(point) == shape2.SignedDistance(point));
    }

    SECTION("Single Intersect") {
        NuGeom::CombinedShape shape(box, sphere, NuGeom::ShapeBinaryOp::kIntersect);
        CHECK(shape.SignedDistance(point) == std::max(box -> SignedDistance(point), sphere -> SignedDistance(point)));
        NuGeom::CombinedShape shape2(sphere, box, NuGeom::ShapeBinaryOp::kIntersect);
        CHECK(shape.SignedDistance(point) == shape2.SignedDistance(point));
    }

    SECTION("Single Subtraction") {
        NuGeom::CombinedShape shape(box, sphere, NuGeom::ShapeBinaryOp::kSubtraction);
        CHECK(shape.SignedDistance(point) == std::max(-box -> SignedDistance(point), sphere -> SignedDistance(point)));
        NuGeom::CombinedShape shape2(sphere, box, NuGeom::ShapeBinaryOp::kSubtraction);
        CHECK(shape.SignedDistance(point) != shape2.SignedDistance(point));
    }
}
