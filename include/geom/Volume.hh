#pragma once

#include "geom/Material.hh"
#include "geom/Shape.hh"

namespace NuGeom {

class LineSegment;
class PhysicalVolume;

class LogicalVolume {
    public:
        LogicalVolume() = default;
        LogicalVolume(Material material, std::shared_ptr<Shape> shape)
            : m_material{std::move(material)}, m_shape{std::move(shape)} {}

        Material GetMaterial() const { return m_material; }
        Shape* GetShape() const { return m_shape.get(); }
        std::shared_ptr<LogicalVolume> Mother() const { return m_mother; }
        const std::vector<std::shared_ptr<PhysicalVolume>>& Daughters() const { return m_daughters; }
        void SetMother(std::shared_ptr<LogicalVolume> mother) { m_mother = mother; }
        void AddDaughter(std::shared_ptr<PhysicalVolume> daughter) { m_daughters.push_back(daughter); }
        double Volume() const;
        double Mass() const;

        bool InWorld(const Vector3D&) const { return true; }
        bool SphereTrace(const Ray&, double&, size_t&, size_t&) const;
        bool RayTrace(const Ray&, double&, std::shared_ptr<PhysicalVolume>&) const;
        void GetLineSegments(const Ray&, std::vector<LineSegment>&) const;

    private:
        double DaughterVolumes() const;
        double DaughterMass() const;
        std::pair<double, size_t> GetSDF(const Vector3D&) const { return {0, 0}; }

        Material m_material;
        std::shared_ptr<Shape> m_shape;
        std::vector<std::shared_ptr<PhysicalVolume>> m_daughters;
        std::shared_ptr<LogicalVolume> m_mother = nullptr;
        static constexpr size_t m_max_steps{512};
        static constexpr double m_epsilon{1e-4};
};

class PhysicalVolume {
    public:
        PhysicalVolume() = default;
        PhysicalVolume(std::shared_ptr<LogicalVolume> volume, Transform3D trans, Transform3D rot)
            : m_volume{std::move(volume)} {

            m_transform = (rot*trans).Inverse();
        }

        const std::shared_ptr<LogicalVolume>& GetLogicalVolume() const { return m_volume; }
        Transform3D GetTransform() const { return m_transform; }
        std::shared_ptr<LogicalVolume> LogicalMother() const { 
            return m_mother ? m_mother -> GetLogicalVolume() : m_volume -> Mother();
        }
        std::shared_ptr<PhysicalVolume> Mother() const { return m_mother; }
        void SetMother(std::shared_ptr<PhysicalVolume> mother) { m_mother = std::move(mother); }
        const std::vector<std::shared_ptr<PhysicalVolume>>& Daughters() const { return m_volume -> Daughters(); }
        double SignedDistance(const Vector3D &in_point) const {
            auto point = TransformPoint(in_point);
            return m_volume -> GetShape() -> SignedDistance(point);
        }
        double Intersect(const Ray &in_ray) const;
        bool RayTrace(const Ray &ray, double &time, std::shared_ptr<PhysicalVolume> &pvol) const {
            return m_volume -> RayTrace(ray, time, pvol);
        }
        void GetLineSegments(const Ray&, std::vector<LineSegment>&, const Transform3D&) const;

    private:
        Vector3D TransformPoint(const Vector3D &point) const {
            return m_transform.Apply(point);
        }
        Vector3D TransformPointInverse(const Vector3D &point) const {
            return m_transform.Inverse().Apply(point);
        }
        Ray TransformRay(const Ray &ray) const;
        Ray TransformRayInverse(const Ray &ray) const;
        std::shared_ptr<LogicalVolume> m_volume;
        std::shared_ptr<PhysicalVolume> m_mother;
        Transform3D m_transform;
};

}
