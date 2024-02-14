#include "geom/Interface.hh"

class CrossSectionTest {
    public:
        double Evaluate(size_t Z, size_t A, double) const {
            return static_cast<double>(Z*A)*1e-38;
        }
};

class FluxTest {
    public:
        std::pair<NuGeom::Vector3D, NuGeom::Vector3D> GetFlux(const std::vector<double>&) { return {{0, 0, -200}, {0, 0, 1}}; }
};

int main(int argc, char *argv[]) {
    if(argc != 2) {
        std::cout << "Usage:\n  prob_test <input>\n";
        return -1;
    }
    // Setup detector
    NuGeom::DetectorSim sim(argv[1]);
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
        NuGeom::Vector3D direction = neutrino.second;
        auto result = sim.GetInteraction(start, direction, 1300);
        std::cout << result.first << " " << result.second.Name() << std::endl;
        // CrossSection Tool would simulate the actual interaction
    }
}
