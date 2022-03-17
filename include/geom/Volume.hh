#pragma once

#include "geom/Material.hh"
#include "geom/Shape.hh"

namespace NuGeom {

class PhysicalVolume;

class Volume {
    public:
        Volume() = default;
        Volume(Material material, std::shared_ptr<Shape> shape)
            : m_material{std::move(material)}, m_shape{std::move(shape)} {}

        Material GetMaterial() const { return m_material; }
        Shape* GetShape() const { return m_shape.get(); }
        Volume* Mother() const { return m_mother.get(); }
        std::vector<std::shared_ptr<PhysicalVolume>> Daughters() const { return m_daughters; }
        void SetMother(std::shared_ptr<Volume> mother) { m_mother = mother; }
        void AddDaughter(std::shared_ptr<PhysicalVolume> daughter) { m_daughters.push_back(daughter); }
        double Mass() const {
            return m_shape -> Volume() * m_material.Density();
        }

    private:

        Material m_material;
        std::shared_ptr<Shape> m_shape;
        std::vector<std::shared_ptr<PhysicalVolume>> m_daughters;
        std::shared_ptr<Volume> m_mother = nullptr;
};

class PhysicalVolume {
    public:
        PhysicalVolume() = default;
        PhysicalVolume(std::shared_ptr<Volume> volume, Transform3D trans, Transform3D rot)
            : m_volume{std::move(volume)} {

            m_transform = (rot*trans);
        }

        Volume* GetVolume() const { return m_volume.get(); }
        Transform3D GetTransform() const { return m_transform; }
        Volume* Mother() const { return m_volume -> Mother(); }
        void SetMother(std::shared_ptr<Volume> mother) { m_volume -> SetMother(mother); }
        std::vector<std::shared_ptr<PhysicalVolume>> Daughters() const { return m_volume -> Daughters(); }
        double SignedDistance(const Vector3D &in_point) const {
            auto point = TransformPoint(in_point);
            return m_volume -> GetShape() -> SignedDistance(point);
        }

    private:
        Vector3D TransformPoint(const Vector3D &point) const {
            return m_transform.Apply(point);
        }

        std::shared_ptr<Volume> m_volume;
        Transform3D m_transform;
};

}
