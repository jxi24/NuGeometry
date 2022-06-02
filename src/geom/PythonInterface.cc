#include "geom/PythonInterface.hh"
#include "geom/LineSegment.hh"
#include "geom/Parser.hh"
#include "geom/Ray.hh"
#include "geom/Vector2D.hh"
#include "geom/Vector3D.hh"
#include "geom/Vector4D.hh"
#include "geom/Transform3D.hh"
#include "geom/Shape.hh"
#include "geom/Element.hh"
#include "geom/Material.hh"

PYBIND11_MODULE(nugeom, m) {
    // XML Parser module
    py::module parser = m.def_submodule("parser", "parser");
    ParserModule(parser);

    // Math objects
    py::module math = m.def_submodule("math", "mathematical objects");
    VectorModule(math);
    TransformModule(math);

    // Shape objects
    py::module shape = m.def_submodule("shape", "shape objects");
    ShapeModule(shape);

    // Material objects
    py::module material = m.def_submodule("material", "material objects");
    ElementModule(material);
    MaterialModule(material);

    // World objects
    py::module world = m.def_submodule("world", "world objects");
    VolumeModule(world);
    WorldModule(world);
}

void ParserModule(py::module &m) {
    py::class_<NuGeom::GDMLParser>(m, "Parser")
        .def(py::init<const std::string&>())
        .def("get_world", &NuGeom::GDMLParser::GetWorld);
}

void VectorModule(py::module &m) {
    py::class_<NuGeom::Vector2D>(m, "Vector2D")
        .def(py::init<>())
        .def(py::init<double, double>())
        .def(py::init<std::array<double, 2>>())
        .def(py::init<const NuGeom::Vector2D&>())
        .def("x", [](NuGeom::Vector2D& self, double x) { self.X() = x; })
        .def("y", [](NuGeom::Vector2D& self, double y) { self.Y() = y; })
        .def("dot", &NuGeom::Vector2D::Dot)
        .def("norm2", &NuGeom::Vector2D::Norm2)
        .def("norm", &NuGeom::Vector2D::Norm)
        .def("unit", &NuGeom::Vector2D::Unit)
        .def("abs", &NuGeom::Vector2D::Abs)
        .def("max", &NuGeom::Vector2D::Max)
        .def("max_component", &NuGeom::Vector2D::MaxComponent)
        .def(double() * py::self)
        .def(py::self * double())
        .def(py::self / double())
        .def(py::self *= double())
        .def(py::self /= double())
        .def(py::self += py::self)
        .def(py::self -= py::self)
        .def(py::self + py::self)
        .def(py::self - py::self)
        .def(-py::self)
        .def(py::self == py::self)
        .def(py::self != py::self);

    py::class_<NuGeom::Vector3D>(m, "Vector3D")
        .def(py::init<>())
        .def(py::init<double, double, double>())
        .def(py::init<std::array<double, 3>>())
        .def(py::init<const NuGeom::Vector3D&>())
        .def("x", [](NuGeom::Vector3D& self, double x) { self.X() = x; })
        .def("y", [](NuGeom::Vector3D& self, double y) { self.Y() = y; })
        .def("z", [](NuGeom::Vector3D& self, double z) { self.Z() = z; })
        .def("r", [](NuGeom::Vector3D& self, double r) { self.R() = r; })
        .def("b", [](NuGeom::Vector3D& self, double g) { self.G() = g; })
        .def("g", [](NuGeom::Vector3D& self, double b) { self.B() = b; })
        .def("dot", &NuGeom::Vector3D::Dot)
        .def("cross", &NuGeom::Vector3D::Cross)
        .def("norm2", &NuGeom::Vector3D::Norm2)
        .def("norm", &NuGeom::Vector3D::Norm)
        .def("unit", &NuGeom::Vector3D::Unit)
        .def("abs", &NuGeom::Vector3D::Abs)
        .def("max", &NuGeom::Vector3D::Max)
        .def("max_component", &NuGeom::Vector3D::MaxComponent)
        .def(double() * py::self)
        .def(py::self * double())
        .def(py::self / double())
        .def(py::self *= double())
        .def(py::self /= double())
        .def(py::self += py::self)
        .def(py::self -= py::self)
        .def(py::self + py::self)
        .def(py::self - py::self)
        .def(-py::self)
        .def(py::self == py::self)
        .def(py::self != py::self);
}

void TransformModule(py::module &m) {
    py::class_<NuGeom::Transform3D>(m, "Transform")
        .def(py::init<>())
        .def(py::init<const std::array<double, 12>&>())
        .def(py::init<double, double, double, double,
                      double, double, double, double,
                      double, double, double, double>())
        .def(py::init<const NuGeom::Rotation3D&, const NuGeom::Translation3D&>())
        .def("apply", &NuGeom::Transform3D::Apply)
        .def("inverse", &NuGeom::Transform3D::Inverse)
        .def(py::self * py::self)
        .def("decompose", &NuGeom::Transform3D::Decompose);

    py::class_<NuGeom::Rotation3D>(m, "Rotation")
        .def(py::init<>())
        .def(py::init<const NuGeom::Vector3D&, double>());

    py::class_<NuGeom::RotationX3D>(m, "RotationX")
        .def(py::init<double>());

    py::class_<NuGeom::RotationY3D>(m, "RotationY")
        .def(py::init<double>());

    py::class_<NuGeom::RotationZ3D>(m, "RotationZ")
        .def(py::init<double>());

    py::class_<NuGeom::Translation3D>(m, "Translation")
        .def(py::init<>())
        .def(py::init<const NuGeom::Vector3D&>())
        .def(py::init<double, double, double>());

    py::class_<NuGeom::TranslationX3D>(m, "TranslationX")
        .def(py::init<double>());

    py::class_<NuGeom::TranslationY3D>(m, "TranslationY")
        .def(py::init<double>());

    py::class_<NuGeom::TranslationZ3D>(m, "TranslationZ")
        .def(py::init<double>());
}

void ShapeModule(py::module &m) {
    py::enum_<NuGeom::Location>(m, "Location")
        .value("interior", NuGeom::Location::kInterior)
        .value("suface", NuGeom::Location::kSurface)
        .value("exterior", NuGeom::Location::kExterior);

    py::class_<NuGeom::Shape>(m, "Shape")
        .def("contains", &NuGeom::Shape::Contains)
        .def("signed_distance", &NuGeom::Shape::SignedDistance)
        .def("set_rotation", &NuGeom::Shape::SetRotation)
        .def("set_translation", &NuGeom::Shape::SetTranslation)
        .def("volume", &NuGeom::Shape::Volume)
        .def_static("create", [](const std::string &description) {
                pugi::xml_document doc;
                doc.load_string(description.c_str());
                auto node = doc.first_child();
                auto name = node.name();
                return NuGeom::ShapeFactory::Initialize(name, node);
            }, py::return_value_policy::move);
}

void ElementModule(py::module &m) {
    py::class_<NuGeom::Element>(m, "Element")
        .def(py::init<>())
        .def(py::init<const std::string&>())
        .def(py::init<const std::string&, size_t, double, size_t>(),
             py::arg("name"), py::arg("Z"), py::arg("mass"), py::arg("A") = 0)
        .def(py::init<const std::string&, const std::string&, size_t, double, size_t>(),
             py::arg("name"), py::arg("symbol"), py::arg("Z"), py::arg("mass"), py::arg("A") = 0)
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def("name", &NuGeom::Element::Name)
        .def("symbol", &NuGeom::Element::Symbol)
        .def("nprotons", &NuGeom::Element::Z)
        .def("nnucleons", &NuGeom::Element::A)
        .def("mass", &NuGeom::Element::Mass)
        .def_static("common_elements", &NuGeom::Element::CommonElements);
}

void MaterialModule(py::module &m) {
    py::class_<NuGeom::Material>(m, "Material")
        .def(py::init<>())
        .def(py::init<const std::string&, double, size_t>())
        .def("ncomponents", &NuGeom::Material::NComponents)
        .def("elements", &NuGeom::Material::Elements)
        .def("mass_fractions", &NuGeom::Material::MassFractions)
        .def("nelements", &NuGeom::Material::NElements)
        .def("add_element", py::overload_cast<const NuGeom::Element&, int>(&NuGeom::Material::AddElement))
        .def("add_element", py::overload_cast<const NuGeom::Element&, double>(&NuGeom::Material::AddElement))
        .def("add_material", &NuGeom::Material::AddMaterial)
        .def("select_element", &NuGeom::Material::SelectElement)
        .def("density", &NuGeom::Material::Density)
        .def("name", &NuGeom::Material::Name);
}

void VolumeModule(py::module &m) {
    py::class_<NuGeom::LogicalVolume>(m, "LogicalVolume")
        .def(py::init<>())
        .def(py::init<NuGeom::Material, std::shared_ptr<NuGeom::Shape>>())
        .def("material", &NuGeom::LogicalVolume::GetMaterial)
        .def("shape", &NuGeom::LogicalVolume::GetShape)
        .def("mother", &NuGeom::LogicalVolume::Mother)
        .def("daughters", &NuGeom::LogicalVolume::Daughters)
        .def("set_mother", &NuGeom::LogicalVolume::SetMother)
        .def("add_daughter", &NuGeom::LogicalVolume::AddDaughter)
        .def("volume", &NuGeom::LogicalVolume::Volume)
        .def("mass", &NuGeom::LogicalVolume::Mass);

    py::class_<NuGeom::PhysicalVolume>(m, "PhysicalVolume")
        .def(py::init<>())
        .def(py::init<std::shared_ptr<NuGeom::LogicalVolume>, NuGeom::Transform3D, NuGeom::Transform3D>())
        .def("logical_volume", &NuGeom::PhysicalVolume::GetLogicalVolume)
        .def("get_transform", &NuGeom::PhysicalVolume::GetTransform)
        .def("mother", &NuGeom::PhysicalVolume::Mother)
        .def("set_mother", &NuGeom::PhysicalVolume::SetMother)
        .def("daughters", &NuGeom::PhysicalVolume::Daughters)
        .def("signed_distance", &NuGeom::PhysicalVolume::SignedDistance);
}

void WorldModule(py::module &m) {
    py::class_<NuGeom::LineSegment>(m, "LineSegment")
        .def(py::init<>())
        .def(py::init<double, size_t>())
        .def("length", &NuGeom::LineSegment::Length)
        .def("shape_id", &NuGeom::LineSegment::ShapeID)
        .def("get_material", &NuGeom::LineSegment::GetMaterial);

    py::class_<NuGeom::Ray>(m, "Ray")
        .def(py::init<>())
        .def(py::init<const NuGeom::Vector3D&, const NuGeom::Vector3D&>())
        .def("origin", &NuGeom::Ray::Origin)
        .def("direction", &NuGeom::Ray::Direction)
        .def("propagate", &NuGeom::Ray::Propagate);

    py::class_<NuGeom::World>(m, "World")
        .def(py::init<>())
        .def(py::init<std::shared_ptr<NuGeom::LogicalVolume>>())
        .def("get_shape", &NuGeom::World::GetShape)
        .def("get_material", &NuGeom::World::GetMaterial)
        .def("in_world", &NuGeom::World::InWorld)
        .def("sphere_trace", &NuGeom::World::SphereTrace)
        .def("line_segments", &NuGeom::World::GetLineSegments);
}
