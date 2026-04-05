#ifndef GUI_APP_H
#define GUI_APP_H

#include "FluidSystem.h"
#include <string>
#include <vector>

// ════════════════════════════════════════════════════════════════════════════
//  GuiApp — ImGui/ImPlot GUI for FluidDynamicsCore (50 simulations)
// ════════════════════════════════════════════════════════════════════════════

class GuiApp {
public:
    GuiApp();
    void render();

private:
    // ── Plot helpers ────────────────────────────────────────────────────
    void clearPlot();
    void addCurve(const std::string& label, const std::vector<double>& x,
                  const std::vector<double>& y);

    // ── State ───────────────────────────────────────────────────────────
    int selectedSim = 0;
    std::string resultText;

    struct CurveData {
        std::string label;
        std::vector<double> x, y;
    };
    std::vector<CurveData> curves;

    FluidSystem fluid;

    // ── Simulation renderers (50) ───────────────────────────────────────
    void renderSim01_Hydrostatics();
    void renderSim02_Bernoulli();
    void renderSim03_EulerEquations();
    void renderSim04_NavierStokes2D();
    void renderSim05_PoiseuilleFlow();
    void renderSim06_CouetteFlow();
    void renderSim07_StokesFlow();
    void renderSim08_PotentialFlow();
    void renderSim09_StreamFunctions();
    void renderSim10_VorticityDynamics();
    void renderSim11_SurfaceGravityWaves();
    void renderSim12_ShallowWater();
    void renderSim13_KelvinHelmholtz();
    void renderSim14_RayleighTaylor();
    void renderSim15_RayleighBenard();
    void renderSim16_TaylorCouette();
    void renderSim17_CapillaryWaves();
    void renderSim18_Solitons();
    void renderSim19_BurgersShock();
    void renderSim20_RiemannProblem();
    void renderSim21_SpeedOfSound();
    void renderSim22_IsentropicFlow();
    void renderSim23_NormalShocks();
    void renderSim24_ObliqueShocks();
    void renderSim25_PrandtlMeyer();
    void renderSim26_NozzleFlow();
    void renderSim27_FannoFlow();
    void renderSim28_RayleighFlow();
    void renderSim29_ReynoldsRegimes();
    void renderSim30_KolmogorovCascade();
    void renderSim31_MixingLength();
    void renderSim32_BlasiusBL();
    void renderSim33_DragSphere();
    void renderSim34_PipeFlowMoody();
    void renderSim35_HeatConvection();
    void renderSim36_MassDiffusion();
    void renderSim37_MHD();
    void renderSim38_AlfvenWaves();
    void renderSim39_PlasmaBeta();
    void renderSim40_AccretionDisk();
    void renderSim41_JeansInstability();
    void renderSim42_BondiAccretion();
    void renderSim43_ViscousDisk();
    void renderSim44_RelativisticFluid();
    void renderSim45_LatticeBoltzmann();
    void renderSim46_SPH();
    void renderSim47_VortexMethods();
    void renderSim48_MultiphaseFlow();
    void renderSim49_NonNewtonian();
    void renderSim50_Microfluidics();
};

#endif // GUI_APP_H
