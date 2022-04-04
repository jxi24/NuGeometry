#pragma once

#include "geom/Material.hh"
#include "geom/Shape.hh"

namespace NuGeom {

class PhysicalVolume;

class LogicalVolume {
    public:
        LogicalVolume() = default;
        LogicalVolume(Material material, std::shared_ptr<Shape> shape)
            : m_material{std::move(material)}, m_shape{std::move(shape)} {}

        Material GetMaterial() const { return m_material; }
        Shape* GetShape() const { return m_shape.get(); }
        LogicalVolume* Mother() const { return m_mother.get(); }
        std::vector<std::shared_ptr<PhysicalVolume>> Daughters() const { return m_daughters; }
        void SetMother(std::shared_ptr<LogicalVolume> mother) { m_mother = mother; }
        void AddDaughter(std::shared_ptr<PhysicalVolume> daughter) { m_daughters.push_back(daughter); }
        double Volume() const;
        double Mass() const;

    private:
        double DaughterVolumes() const;
        double DaughterMass() const;

        Material m_material;
        std::shared_ptr<Shape> m_shape;
        std::vector<std::shared_ptr<PhysicalVolume>> m_daughters;
        std::shared_ptr<LogicalVolume> m_mother = nullptr;
};

class PhysicalVolume {
    public:
        PhysicalVolume() = default;
        PhysicalVolume(std::shared_ptr<LogicalVolume> volume, Transform3D trans, Transform3D rot)
            : m_volume{std::move(volume)} {

            m_transform = (rot*trans);
        }

        LogicalVolume* GetLogicalVolume() const { return m_volume.get(); }
        Transform3D GetTransform() const { return m_transform; }
        LogicalVolume* Mother() const { return m_volume -> Mother(); }
        void SetMother(std::shared_ptr<LogicalVolume> mother) { m_volume -> SetMother(mother); }
        std::vector<std::shared_ptr<PhysicalVolume>> Daughters() const { return m_volume -> Daughters(); }
        double SignedDistance(const Vector3D &in_point) const {
            auto point = TransformPoint(in_point);
            return m_volume -> GetShape() -> SignedDistance(point);
        }

    private:
        Vector3D TransformPoint(const Vector3D &point) const {
            return m_transform.Apply(point);
        }

        std::shared_ptr<LogicalVolume> m_volume;
        Transform3D m_transform;
};

}
