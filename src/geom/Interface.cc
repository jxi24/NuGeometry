#include "geom/Element.hh"
#include "geom/Material.hh"
#include "geom/Parser.hh"
#include "geom/World.hh"
#include "geom/Ray.hh"
#include <functional>

namespace NuGeom {

class DetectorSim {
    public:
        DetectorSim(); // Setup the detector (user cin interface)
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
            for(size_t i = 0; i < mat.NElements(); ++i) {
                m_xsec[mat].push_back(mat.MassFractions()[i]*CalcCrossSection(mat.Elements()[i], nu_energy));
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

class CrossSectionTest {
    public:
        double Evaluate(size_t Z, size_t A, double energy) const {
            return static_cast<double>(Z*A)*energy;
        }
};

class FluxTest {
    public:
        std::pair<NuGeom::Vector3D, NuGeom::Vector4D> GetFlux(const std::vector<double>&) { return {{0, 0, -2}, {energy, px, py, pz}}; }
};

int main() {
    // Setup detector
    NuGeom::DetectorSim sim;
    CrossSectionTest test;
    FluxTest flux;
    auto func = [&](const NuGeom::Element &elm, double nu_energy) {
        return test.Evaluate(elm.Z(), elm.A(), nu_energy);
    };
    sim.SetCrossSectionCalculator(func);

    // Generate rays and get interaction points
    size_t nevents = 5;
    for(size_t i = 0; i < nevents; ++i) {
        // Get random numbers
        auto neutrino = flux.GetFlux({});
        NuGeom::Vector3D start = neutrino.first;
        NuGeom::Vector3D direction = {neutrino.second[1]/neutrino.second[0],
                                      neutrino.second[2]/neutrino.second[0],
                                      neutrino.second[3]/neutrino.second[0]};
        auto result = sim.GetInteraction(start, direction, neutrino.second[0]);
        // CrossSection Tool would simulate the actual interaction
    }
}
