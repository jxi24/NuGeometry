#pragma once

#include "geom/Element.hh"
#include "geom/Material.hh"
#include "geom/Parser.hh"
#include "geom/World.hh"
#include "geom/Ray.hh"
#include "geom/LineSegment.hh"
#include <functional>
#include <numeric>

namespace NuGeom {

class DetectorSim {
public:
    DetectorSim() {}; // Setup the detector (user cin interface)
    DetectorSim(const std::string &filename) {
        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_file(filename.c_str());
        if(!result)
            throw std::runtime_error("GDMLParser: Invalid file");

        NuGeom::GDMLParser parse(doc);
        world = parse.GetWorld();
        m_mats = parse.GetMaterials();
    }
    bool SetCrossSectionCalculator(std::function<double(const NuGeom::Element&, double)> func) {
        m_func = func;
        return true;
    }

    std::pair<Vector3D, NuGeom::Element> GetInteraction(Vector3D position, Vector3D direction, double energy) {
        NuGeom::Ray ray(position, direction);
        auto segments = world.GetLineSegments(ray);
        auto mfp = GetMeanFreePath(energy);
        Vector3D point;
        NuGeom::Element elm;
        // Choose interaction point
        std::vector<double> probs(segments.size());
        for(const auto &path : mfp) {
            std::cout << path.first.Name() << " " << path.second << std::endl;
        }
        size_t idx = 0;
        double total_prob = std::accumulate(segments.begin(), segments.end(), 0.0, [mfp, &probs, &idx](double a, LineSegment &b) {
            std::cout << b.GetMaterial().Name() << std::endl;
            auto mean_free_path = mfp.at(b.GetMaterial());
            probs[idx] = 1-exp(-b.Length()/mean_free_path);
            std::cout << idx << " " << probs[idx] << " " << b.Length() << " " << mean_free_path << std::endl;
            return a + probs[idx++];
        });
        std::cout << "Total prob = " << total_prob << std::endl;
        // Choose element from material at interaction point
        return {point, elm};
    }

private:
    std::map<NuGeom::Material, double> GetMeanFreePath(double nu_energy) {
        std::map<NuGeom::Material, double> mfp;
        // Fill result mfp
        for(size_t i = 0; i < m_mats.size(); ++i) {
            mfp[m_mats[i]] = 1.0/(CalcCrossSection(m_mats[i], nu_energy)*m_mats[i].Density());
        }
        return mfp;
    }

    double CalcCrossSection(const NuGeom::Element &elm, double nu_energy) {
        return m_func(elm, nu_energy);
    }

    double CalcCrossSection(const NuGeom::Material &mat, double nu_energy) {
        double cross_section = 0;
        std::cout << "Evaluating for " << mat.Name() << std::endl;
        for(size_t i = 0; i < mat.NElements(); ++i) {
            m_xsec[mat].push_back(mat.MassFractions()[i]*CalcCrossSection(mat.Elements()[i], nu_energy));
            std::cout << mat.Elements()[i].Name() << " " << mat.MassFractions()[i] << " " << CalcCrossSection(mat.Elements()[i], nu_energy) << std::endl;
            cross_section += m_xsec[mat].back();
        }
        return cross_section;
    }

    NuGeom::World world;
    std::function<double(const NuGeom::Element&, double)> m_func;
    std::vector<NuGeom::Material> m_mats;
    std::map<NuGeom::Material, std::vector<double>> m_xsec;
};

}
