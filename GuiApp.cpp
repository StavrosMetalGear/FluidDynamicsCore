// ════════════════════════════════════════════════════════════════════════════
//  GuiApp.cpp — ImGui/ImPlot GUI for FluidDynamicsCore (50 simulations)
// ════════════════════════════════════════════════════════════════════════════

#include "GuiApp.h"
#include "FluidConstants.h"
#include "NumericalSolverFD.h"
#include "imgui.h"
#include "implot.h"
#include <sstream>
#include <iomanip>
#include <cmath>

using namespace std;

// ── Constructor ─────────────────────────────────────────────────────────────
GuiApp::GuiApp()
    : fluid("Water", FluidConstants::rho_water, FluidConstants::mu_water, 0.01, 1.0)
{}

// ── Plot helpers ────────────────────────────────────────────────────────────
void GuiApp::clearPlot() { curves.clear(); }

void GuiApp::addCurve(const string& label, const vector<double>& x,
                      const vector<double>& y) {
    curves.push_back({label, x, y});
}

// ── Main render ─────────────────────────────────────────────────────────────
void GuiApp::render() {
    static const char* names[] = {
        " 1  Hydrostatics",
        " 2  Bernoulli Equation",
        " 3  Euler Equations (1D)",
        " 4  Navier-Stokes (2D Cavity)",
        " 5  Poiseuille Flow",
        " 6  Couette Flow",
        " 7  Stokes Flow (Creeping)",
        " 8  Potential Flow",
        " 9  Stream Functions",
        "10  Vorticity Dynamics",
        "11  Surface Gravity Waves",
        "12  Shallow Water Equations",
        "13  Kelvin-Helmholtz Instability",
        "14  Rayleigh-Taylor Instability",
        "15  Rayleigh-Benard Convection",
        "16  Taylor-Couette Flow",
        "17  Capillary Waves",
        "18  Solitons (KdV)",
        "19  Shock Waves (Burgers)",
        "20  Riemann Problem",
        "21  Speed of Sound",
        "22  Isentropic Flow",
        "23  Normal Shocks",
        "24  Oblique Shocks",
        "25  Prandtl-Meyer Expansion",
        "26  Nozzle Flow (de Laval)",
        "27  Fanno Flow",
        "28  Rayleigh Flow",
        "29  Reynolds Number Regimes",
        "30  Kolmogorov Cascade",
        "31  Mixing Length Theory",
        "32  Boundary Layers (Blasius)",
        "33  Drag on a Sphere",
        "34  Pipe Flow (Moody)",
        "35  Heat Convection",
        "36  Mass Diffusion (Fick)",
        "37  Magnetohydrodynamics",
        "38  Alfven Waves",
        "39  Plasma Beta",
        "40  Accretion Disk Model",
        "41  Jeans Instability",
        "42  Bondi Accretion",
        "43  Viscous Disk (Shakura-Sunyaev)",
        "44  Relativistic Fluid",
        "45  Lattice Boltzmann (Toy)",
        "46  SPH (Toy)",
        "47  Vortex Methods",
        "48  Multiphase Flow",
        "49  Non-Newtonian Fluids",
        "50  Microfluidics"
    };

    // ── Left sidebar ────────────────────────────────────────────────────
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(280, ImGui::GetIO().DisplaySize.y), ImGuiCond_Once);
    ImGui::Begin("Simulations##Sidebar", nullptr,
                 ImGuiWindowFlags_NoCollapse);
    for (int i = 0; i < 50; ++i) {
        bool selected = (selectedSim == i);
        if (ImGui::Selectable(names[i], selected))
            selectedSim = i;
    }
    ImGui::End();

    // ── Center panel ────────────────────────────────────────────────────
    ImGui::SetNextWindowPos(ImVec2(290, 0), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x - 290,
                                     ImGui::GetIO().DisplaySize.y), ImGuiCond_Once);
    ImGui::Begin("FluidDynamicsCore##Main", nullptr, ImGuiWindowFlags_NoCollapse);

    switch (selectedSim) {
        case  0: renderSim01_Hydrostatics(); break;
        case  1: renderSim02_Bernoulli(); break;
        case  2: renderSim03_EulerEquations(); break;
        case  3: renderSim04_NavierStokes2D(); break;
        case  4: renderSim05_PoiseuilleFlow(); break;
        case  5: renderSim06_CouetteFlow(); break;
        case  6: renderSim07_StokesFlow(); break;
        case  7: renderSim08_PotentialFlow(); break;
        case  8: renderSim09_StreamFunctions(); break;
        case  9: renderSim10_VorticityDynamics(); break;
        case 10: renderSim11_SurfaceGravityWaves(); break;
        case 11: renderSim12_ShallowWater(); break;
        case 12: renderSim13_KelvinHelmholtz(); break;
        case 13: renderSim14_RayleighTaylor(); break;
        case 14: renderSim15_RayleighBenard(); break;
        case 15: renderSim16_TaylorCouette(); break;
        case 16: renderSim17_CapillaryWaves(); break;
        case 17: renderSim18_Solitons(); break;
        case 18: renderSim19_BurgersShock(); break;
        case 19: renderSim20_RiemannProblem(); break;
        case 20: renderSim21_SpeedOfSound(); break;
        case 21: renderSim22_IsentropicFlow(); break;
        case 22: renderSim23_NormalShocks(); break;
        case 23: renderSim24_ObliqueShocks(); break;
        case 24: renderSim25_PrandtlMeyer(); break;
        case 25: renderSim26_NozzleFlow(); break;
        case 26: renderSim27_FannoFlow(); break;
        case 27: renderSim28_RayleighFlow(); break;
        case 28: renderSim29_ReynoldsRegimes(); break;
        case 29: renderSim30_KolmogorovCascade(); break;
        case 30: renderSim31_MixingLength(); break;
        case 31: renderSim32_BlasiusBL(); break;
        case 32: renderSim33_DragSphere(); break;
        case 33: renderSim34_PipeFlowMoody(); break;
        case 34: renderSim35_HeatConvection(); break;
        case 35: renderSim36_MassDiffusion(); break;
        case 36: renderSim37_MHD(); break;
        case 37: renderSim38_AlfvenWaves(); break;
        case 38: renderSim39_PlasmaBeta(); break;
        case 39: renderSim40_AccretionDisk(); break;
        case 40: renderSim41_JeansInstability(); break;
        case 41: renderSim42_BondiAccretion(); break;
        case 42: renderSim43_ViscousDisk(); break;
        case 43: renderSim44_RelativisticFluid(); break;
        case 44: renderSim45_LatticeBoltzmann(); break;
        case 45: renderSim46_SPH(); break;
        case 46: renderSim47_VortexMethods(); break;
        case 47: renderSim48_MultiphaseFlow(); break;
        case 48: renderSim49_NonNewtonian(); break;
        case 49: renderSim50_Microfluidics(); break;
    }

    // ── Plot area ───────────────────────────────────────────────────────
    if (!curves.empty() && ImPlot::BeginPlot("##FluidPlot", ImVec2(-1, 300))) {
        for (auto& c : curves)
            if (!c.x.empty())
                ImPlot::PlotLine(c.label.c_str(), c.x.data(), c.y.data(),
                                 (int)c.x.size());
        ImPlot::EndPlot();
    }

    // ── Result text ─────────────────────────────────────────────────────
    if (!resultText.empty())
        ImGui::TextWrapped("%s", resultText.c_str());

    ImGui::End();
}

// ════════════════════════════════════════════════════════════════════════════
//  SIMULATION STUBS  (1–50)
//  Each contains: Theory header, mode combo, parameter inputs,
//  Compute/Export buttons.  Physics calls are wired; GUI layout is stub.
// ════════════════════════════════════════════════════════════════════════════

// ── 1. Hydrostatics ─────────────────────────────────────────────────────────
void GuiApp::renderSim01_Hydrostatics() {
    if (ImGui::CollapsingHeader("Theory: Hydrostatics")) {
        ImGui::TextWrapped(
            "Hydrostatic pressure increases linearly with depth:\n"
            "  P(h) = P0 + rho * g * h\n\n"
            "Archimedes' principle: buoyant force equals weight of displaced fluid:\n"
            "  F_b = rho_fluid * g * V_submerged\n\n"
            "A floating object displaces fluid equal to its own weight.");
    }
    static int mode = 0;
    const char* modes[] = {"Pressure Profile", "Buoyancy", "Submerged Fraction"};
    ImGui::Combo("Mode##01", &mode, modes, 3);
    static double g = 9.81, p0 = 101325.0, hMax = 100.0;
    static int numPoints = 200;
    ImGui::InputDouble("g (m/s^2)##01", &g);
    ImGui::InputDouble("P0 (Pa)##01", &p0);
    ImGui::InputDouble("h_max (m)##01", &hMax);
    ImGui::SliderInt("Points##01", &numPoints, 50, 1000);
    if (ImGui::Button("Compute##01")) {
        clearPlot();
        ostringstream oss;
        // TODO: implement compute logic for each mode
        oss << "Hydrostatics — mode " << mode << "\n";
        resultText = oss.str();
    }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##01")) {
        fluid.exportHydrostaticsCSV("hydrostatics.csv", g, p0, numPoints, hMax);
    }
}

// ── 2. Bernoulli Equation ───────────────────────────────────────────────────
void GuiApp::renderSim02_Bernoulli() {
    if (ImGui::CollapsingHeader("Theory: Bernoulli Equation")) {
        ImGui::TextWrapped(
            "For steady, inviscid, incompressible flow along a streamline:\n"
            "  P + 0.5*rho*v^2 + rho*g*h = const\n\n"
            "Applications: Venturi meter, Pitot tube, Torricelli's theorem.\n"
            "  v_Torricelli = sqrt(2*g*h)");
    }
    static int mode = 0;
    const char* modes[] = {"Pressure vs Velocity", "Venturi Effect", "Pitot Tube", "Torricelli"};
    ImGui::Combo("Mode##02", &mode, modes, 4);
    static double p1 = 101325, v1 = 1.0, h1 = 0.0;
    ImGui::InputDouble("P1 (Pa)##02", &p1);
    ImGui::InputDouble("v1 (m/s)##02", &v1);
    ImGui::InputDouble("h1 (m)##02", &h1);
    if (ImGui::Button("Compute##02")) {
        clearPlot();
        ostringstream oss;
        oss << "Bernoulli — mode " << mode << "\n";
        resultText = oss.str();
    }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##02")) {
        fluid.exportBernoulliCSV("bernoulli.csv", p1, v1, h1, FluidConstants::g, 200);
    }
}

// ── 3. Euler Equations ──────────────────────────────────────────────────────
void GuiApp::renderSim03_EulerEquations() {
    if (ImGui::CollapsingHeader("Theory: Euler Equations")) {
        ImGui::TextWrapped(
            "The Euler equations describe inviscid compressible flow:\n"
            "  d(rho)/dt + div(rho*v) = 0         (continuity)\n"
            "  d(rho*v)/dt + div(rho*v*v + P*I) = 0  (momentum)\n"
            "  d(E)/dt + div((E+P)*v) = 0         (energy)\n\n"
            "Solved here with Lax-Friedrichs scheme for 1D Riemann problems.");
    }
    static double rhoL = 1.0, uL = 0.0, pL = 1.0;
    static double rhoR = 0.125, uR = 0.0, pR = 0.1;
    static double gamma = 1.4, tFinal = 0.2;
    static int nx = 400;
    ImGui::InputDouble("rhoL##03", &rhoL); ImGui::SameLine(); ImGui::InputDouble("rhoR##03", &rhoR);
    ImGui::InputDouble("uL##03", &uL); ImGui::SameLine(); ImGui::InputDouble("uR##03", &uR);
    ImGui::InputDouble("pL##03", &pL); ImGui::SameLine(); ImGui::InputDouble("pR##03", &pR);
    ImGui::InputDouble("gamma##03", &gamma);
    ImGui::InputDouble("t_final##03", &tFinal);
    if (ImGui::Button("Compute##03")) {
        clearPlot();
        ostringstream oss;
        oss << "Euler 1D — Sod shock tube\n";
        resultText = oss.str();
    }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##03")) {
        auto state = FluidSystem::solveEuler1D(rhoL, uL, pL, rhoR, uR, pR, gamma, 0, 1, nx, tFinal, 0.5);
        fluid.exportEuler1DCSV("euler_1d.csv", state);
    }
}

// ── 4. Navier-Stokes (2D Cavity) ───────────────────────────────────────────
void GuiApp::renderSim04_NavierStokes2D() {
    if (ImGui::CollapsingHeader("Theory: Navier-Stokes Equations (2D)")) {
        ImGui::TextWrapped(
            "The incompressible Navier-Stokes equations:\n"
            "  div(v) = 0\n"
            "  d(v)/dt + (v.grad)v = -grad(P)/rho + nu*laplacian(v)\n\n"
            "Lid-driven cavity: top wall moves at u=1, all other walls no-slip.\n"
            "Solved with fractional step method (pressure-Poisson).");
    }
    static int nx = 41, ny = 41, maxIter = 500;
    static double Re = 100.0, dt = 0.001;
    ImGui::SliderInt("nx##04", &nx, 11, 101);
    ImGui::SliderInt("ny##04", &ny, 11, 101);
    ImGui::InputDouble("Re##04", &Re);
    ImGui::InputDouble("dt##04", &dt);
    ImGui::SliderInt("Max Iter##04", &maxIter, 100, 5000);
    if (ImGui::Button("Compute##04")) {
        clearPlot();
        ostringstream oss;
        oss << "Navier-Stokes 2D Cavity — Re = " << Re << "\n";
        resultText = oss.str();
    }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##04")) {
        auto field = fluid.solveNavierStokes2DCavity(nx, ny, Re, dt, maxIter);
        fluid.exportNavierStokes2DCSV("navier_stokes_2d.csv", field);
    }
}

// ── 5. Poiseuille Flow ──────────────────────────────────────────────────────
void GuiApp::renderSim05_PoiseuilleFlow() {
    if (ImGui::CollapsingHeader("Theory: Poiseuille Flow")) {
        ImGui::TextWrapped(
            "Steady, fully-developed, laminar flow in a pipe:\n"
            "  u(r) = -(dP/dx)/(4*mu) * (R^2 - r^2)\n\n"
            "  Q = pi*R^4*(-dP/dx) / (8*mu)  (Hagen-Poiseuille law)\n"
            "  tau_wall = -dP/dx * R / 2");
    }
    static int mode = 0;
    const char* modes[] = {"Pipe (circular)", "Channel (planar)"};
    ImGui::Combo("Mode##05", &mode, modes, 2);
    static double dpdx = -100.0, R = 0.01;
    static int numPoints = 200;
    ImGui::InputDouble("dP/dx (Pa/m)##05", &dpdx);
    ImGui::InputDouble("R or h (m)##05", &R);
    if (ImGui::Button("Compute##05")) {
        clearPlot();
        ostringstream oss;
        oss << "Poiseuille Flow\n";
        oss << "  Q = " << FluidSystem::poiseuilleFlowRate(dpdx, fluid.mu, R) << " m^3/s\n";
        oss << "  u_max = " << FluidSystem::poiseuilleMaxVelocity(dpdx, fluid.mu, R) << " m/s\n";
        oss << "  tau_wall = " << FluidSystem::poiseuilleWallShear(dpdx, R) << " Pa\n";
        resultText = oss.str();
    }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##05")) {
        fluid.exportPoiseuilleCSV("poiseuille.csv", dpdx, R, numPoints);
    }
}

// ── 6. Couette Flow ─────────────────────────────────────────────────────────
void GuiApp::renderSim06_CouetteFlow() {
    if (ImGui::CollapsingHeader("Theory: Couette Flow")) {
        ImGui::TextWrapped(
            "Flow between two parallel plates, top moving at U:\n"
            "  u(y) = U*y/h + (-dP/dx)/(2*mu) * y*(h-y)\n\n"
            "Pure Couette (no pressure gradient): linear profile u = U*y/h.\n"
            "With adverse pressure gradient: backflow near bottom wall.");
    }
    static double Utop = 1.0, h = 0.01, dpdx = 0.0;
    static int numPoints = 200;
    ImGui::InputDouble("U_top (m/s)##06", &Utop);
    ImGui::InputDouble("h (m)##06", &h);
    ImGui::InputDouble("dP/dx##06", &dpdx);
    if (ImGui::Button("Compute##06")) {
        clearPlot();
        resultText = "Couette Flow computed.\n";
    }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##06")) {
        fluid.exportCouetteCSV("couette.csv", Utop, h, dpdx, numPoints);
    }
}

// ── 7. Stokes Flow ──────────────────────────────────────────────────────────
void GuiApp::renderSim07_StokesFlow() {
    if (ImGui::CollapsingHeader("Theory: Stokes Flow (Creeping Flow)")) {
        ImGui::TextWrapped(
            "At Re << 1, inertial terms are negligible:\n"
            "  grad P = mu * laplacian(v)\n\n"
            "Stokes drag on a sphere: F = 6*pi*mu*R*U\n"
            "  Cd = 24/Re\n"
            "Terminal velocity: V_t = 2*(rho_p - rho_f)*g*R^2 / (9*mu)");
    }
    static double Rs = 0.001, Us = 0.01;
    ImGui::InputDouble("R (m)##07", &Rs);
    ImGui::InputDouble("U (m/s)##07", &Us);
    if (ImGui::Button("Compute##07")) {
        clearPlot();
        ostringstream oss;
        double F = FluidSystem::stokesDragForce(fluid.mu, Rs, Us);
        oss << "Stokes drag = " << F << " N\n";
        resultText = oss.str();
    }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##07")) {
        fluid.exportStokesFlowCSV("stokes_flow.csv", Rs, Us, 100);
    }
}

// ── 8. Potential Flow ───────────────────────────────────────────────────────
void GuiApp::renderSim08_PotentialFlow() {
    if (ImGui::CollapsingHeader("Theory: Potential Flow")) {
        ImGui::TextWrapped(
            "Irrotational, inviscid flow: v = grad(phi)\n"
            "  laplacian(phi) = 0  (Laplace equation)\n\n"
            "Elementary flows: uniform, source/sink, vortex, doublet.\n"
            "Flow around cylinder: superposition of uniform + doublet.\n"
            "  Cp = 1 - 4*sin^2(theta)  on cylinder surface (d'Alembert paradox).");
    }
    static int mode = 0;
    const char* modes[] = {"Cylinder", "Source/Sink", "Vortex", "Rankine Oval"};
    ImGui::Combo("Mode##08", &mode, modes, 4);
    static double Uf = 1.0, Rf = 0.5;
    ImGui::InputDouble("U_inf##08", &Uf);
    ImGui::InputDouble("R##08", &Rf);
    if (ImGui::Button("Compute##08")) {
        clearPlot();
        resultText = "Potential Flow computed.\n";
    }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##08")) {
        fluid.exportPotentialFlowCSV("potential_flow.csv", Uf, Rf, 50, 50);
    }
}

// ── 9. Stream Functions ─────────────────────────────────────────────────────
void GuiApp::renderSim09_StreamFunctions() {
    if (ImGui::CollapsingHeader("Theory: Stream Functions")) {
        ImGui::TextWrapped(
            "For 2D incompressible flow, the stream function psi satisfies:\n"
            "  u = d(psi)/dy,  v = -d(psi)/dx\n\n"
            "Streamlines are contours of constant psi.\n"
            "Volume flow between two streamlines = psi_2 - psi_1.");
    }
    static int type = 0;
    const char* types[] = {"Uniform", "Source", "Vortex", "Doublet", "Rankine Oval"};
    ImGui::Combo("Type##09", &type, types, 5);
    static double param1 = 1.0, param2 = 1.0;
    ImGui::InputDouble("Param 1##09", &param1);
    ImGui::InputDouble("Param 2##09", &param2);
    if (ImGui::Button("Compute##09")) { clearPlot(); resultText = "Stream functions computed.\n"; }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##09")) {
        fluid.exportStreamFunctionCSV("stream_functions.csv", type, param1, param2, 50, 50);
    }
}

// ── 10. Vorticity Dynamics ──────────────────────────────────────────────────
void GuiApp::renderSim10_VorticityDynamics() {
    if (ImGui::CollapsingHeader("Theory: Vorticity Dynamics")) {
        ImGui::TextWrapped(
            "Vorticity: omega = curl(v)\n"
            "Vorticity transport: D(omega)/Dt = (omega.grad)v + nu*laplacian(omega)\n\n"
            "Kelvin's circulation theorem: dGamma/dt = 0 (inviscid, barotropic).\n"
            "Rankine vortex: solid-body core + irrotational exterior.\n"
            "Lamb-Oseen vortex: viscous decay of a point vortex.");
    }
    static int mode = 0;
    const char* modes[] = {"Rankine Vortex", "Lamb-Oseen Vortex", "Point Vortices"};
    ImGui::Combo("Mode##10", &mode, modes, 3);
    static double gamma10 = 1.0, R10 = 0.1, nu10 = 1e-6;
    ImGui::InputDouble("Gamma##10", &gamma10);
    ImGui::InputDouble("R (core)##10", &R10);
    ImGui::InputDouble("nu##10", &nu10);
    if (ImGui::Button("Compute##10")) { clearPlot(); resultText = "Vorticity dynamics computed.\n"; }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##10")) {
        fluid.exportVorticityCSV("vorticity.csv", mode, gamma10, R10, nu10, 200);
    }
}

// ── 11. Surface Gravity Waves ───────────────────────────────────────────────
void GuiApp::renderSim11_SurfaceGravityWaves() {
    if (ImGui::CollapsingHeader("Theory: Surface Gravity Waves")) {
        ImGui::TextWrapped(
            "Dispersion relation: omega^2 = g*k*tanh(k*h)\n"
            "Deep water (kh >> 1): omega = sqrt(g*k), c = sqrt(g/k)\n"
            "Shallow water (kh << 1): c = sqrt(g*h)\n\n"
            "Group velocity: c_g = d(omega)/dk\n"
            "Wave energy density: E = 0.5*rho*g*A^2");
    }
    static double A = 0.5, k = 1.0, h = 10.0;
    ImGui::InputDouble("Amplitude A##11", &A);
    ImGui::InputDouble("Wavenumber k##11", &k);
    ImGui::InputDouble("Depth h##11", &h);
    if (ImGui::Button("Compute##11")) { clearPlot(); resultText = "Surface waves computed.\n"; }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##11")) {
        fluid.exportSurfaceGravityWavesCSV("surface_waves.csv", A, k, h, FluidConstants::g, 200, 20);
    }
}

// ── 12. Shallow Water Equations ─────────────────────────────────────────────
void GuiApp::renderSim12_ShallowWater() {
    if (ImGui::CollapsingHeader("Theory: Shallow Water Equations")) {
        ImGui::TextWrapped(
            "1D shallow water (Saint-Venant):\n"
            "  d(h)/dt + d(hu)/dx = 0\n"
            "  d(hu)/dt + d(hu^2 + 0.5*g*h^2)/dx = 0\n\n"
            "Dam-break problem: discontinuous initial water height.\n"
            "Supports rarefaction waves and hydraulic jumps.");
    }
    static double hL = 2.0, hR = 1.0, uL12 = 0.0, uR12 = 0.0, tF = 0.5;
    ImGui::InputDouble("h_L##12", &hL); ImGui::SameLine(); ImGui::InputDouble("h_R##12", &hR);
    ImGui::InputDouble("t_final##12", &tF);
    if (ImGui::Button("Compute##12")) { clearPlot(); resultText = "Shallow water computed.\n"; }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##12")) {
        auto st = FluidSystem::solveShallowWater1D(hL, hR, uL12, uR12, FluidConstants::g, -5, 5, 400, tF, 0.5);
        fluid.exportShallowWaterCSV("shallow_water.csv", st);
    }
}

// ── 13. Kelvin-Helmholtz Instability ────────────────────────────────────────
void GuiApp::renderSim13_KelvinHelmholtz() {
    if (ImGui::CollapsingHeader("Theory: Kelvin-Helmholtz Instability")) {
        ImGui::TextWrapped(
            "Shear instability at the interface of two fluids with different velocities.\n"
            "Growth rate: sigma = sqrt(rho1*rho2*k^2*dU^2/(rho1+rho2)^2 - g*k*(rho2-rho1)/(rho1+rho2) - sigma_s*k^3/(rho1+rho2))\n\n"
            "Surface tension stabilizes short wavelengths.\n"
            "Gravity stabilizes if heavier fluid is below.");
    }
    static double rho1 = 1.0, rho2 = 1.2, dU = 2.0, sigma13 = 0.01;
    ImGui::InputDouble("rho1##13", &rho1); ImGui::SameLine(); ImGui::InputDouble("rho2##13", &rho2);
    ImGui::InputDouble("Delta U##13", &dU);
    ImGui::InputDouble("sigma##13", &sigma13);
    if (ImGui::Button("Compute##13")) { clearPlot(); resultText = "K-H instability computed.\n"; }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##13")) {
        fluid.exportKHInstabilityCSV("kh_instability.csv", rho1, rho2, dU, sigma13, FluidConstants::g, 200);
    }
}

// ── 14. Rayleigh-Taylor Instability ─────────────────────────────────────────
void GuiApp::renderSim14_RayleighTaylor() {
    if (ImGui::CollapsingHeader("Theory: Rayleigh-Taylor Instability")) {
        ImGui::TextWrapped(
            "Heavy fluid above lighter fluid under gravity is unstable.\n"
            "Atwood number: At = (rho2 - rho1) / (rho2 + rho1)\n"
            "Growth rate: sigma = sqrt(At*g*k - sigma*k^3/(rho1+rho2))\n\n"
            "Critical wavelength: lambda_c = 2*pi*sqrt(sigma/((rho2-rho1)*g))");
    }
    static double rho1_14 = 1.0, rho2_14 = 3.0, sigma14 = 0.01;
    ImGui::InputDouble("rho1##14", &rho1_14); ImGui::SameLine(); ImGui::InputDouble("rho2##14", &rho2_14);
    ImGui::InputDouble("sigma##14", &sigma14);
    if (ImGui::Button("Compute##14")) { clearPlot(); resultText = "R-T instability computed.\n"; }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##14")) {
        fluid.exportRTInstabilityCSV("rt_instability.csv", rho1_14, rho2_14, FluidConstants::g, sigma14, 200);
    }
}

// ── 15. Rayleigh-Bénard Convection ──────────────────────────────────────────
void GuiApp::renderSim15_RayleighBenard() {
    if (ImGui::CollapsingHeader("Theory: Rayleigh-Benard Convection")) {
        ImGui::TextWrapped(
            "Thermal convection between heated bottom and cooled top plates.\n"
            "Rayleigh number: Ra = g*beta*dT*L^3 / (nu*alpha)\n"
            "Critical Ra ≈ 1707.76 (rigid-rigid boundaries).\n\n"
            "For Ra > Ra_c: convection rolls develop.\n"
            "Nusselt number: Nu(Ra, Pr) describes heat transfer enhancement.");
    }
    static double dT = 10.0, Lrb = 0.01;
    ImGui::InputDouble("Delta T (K)##15", &dT);
    ImGui::InputDouble("L (m)##15", &Lrb);
    if (ImGui::Button("Compute##15")) { clearPlot(); resultText = "R-B convection computed.\n"; }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##15")) {
        double alpha = FluidConstants::k_water / (FluidConstants::rho_water * FluidConstants::cp_water);
        fluid.exportRayleighBenardCSV("rayleigh_benard.csv", FluidConstants::g, FluidConstants::beta_water,
                                       FluidConstants::nu_water, alpha, Lrb, 200);
    }
}

// ── 16. Taylor-Couette Flow ─────────────────────────────────────────────────
void GuiApp::renderSim16_TaylorCouette() {
    if (ImGui::CollapsingHeader("Theory: Taylor-Couette Flow")) {
        ImGui::TextWrapped(
            "Flow between two coaxial rotating cylinders.\n"
            "Laminar (Couette) solution: v_theta(r) = A*r + B/r\n"
            "Taylor number: Ta = Omega1^2 * R1 * d^3 / nu^2\n"
            "Above critical Ta: Taylor vortices appear.");
    }
    static double omega1 = 10.0, omega2 = 0.0, R1tc = 0.05, R2tc = 0.06;
    ImGui::InputDouble("Omega1 (rad/s)##16", &omega1);
    ImGui::InputDouble("Omega2 (rad/s)##16", &omega2);
    ImGui::InputDouble("R1 (m)##16", &R1tc);
    ImGui::InputDouble("R2 (m)##16", &R2tc);
    if (ImGui::Button("Compute##16")) { clearPlot(); resultText = "Taylor-Couette computed.\n"; }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##16")) {
        fluid.exportTaylorCouetteCSV("taylor_couette.csv", omega1, omega2, R1tc, R2tc, 200);
    }
}

// ── 17. Capillary Waves ─────────────────────────────────────────────────────
void GuiApp::renderSim17_CapillaryWaves() {
    if (ImGui::CollapsingHeader("Theory: Capillary Waves")) {
        ImGui::TextWrapped(
            "Surface tension dominated waves:\n"
            "  omega^2 = g*k + sigma*k^3/rho\n\n"
            "Capillary length: l_c = sqrt(sigma/(rho*g))\n"
            "Minimum phase speed at k = sqrt(rho*g/sigma).\n"
            "Young-Laplace pressure: dP = 2*sigma/R (droplet).");
    }
    static double sigma17 = FluidConstants::sigma_water;
    ImGui::InputDouble("sigma (N/m)##17", &sigma17);
    if (ImGui::Button("Compute##17")) { clearPlot(); resultText = "Capillary waves computed.\n"; }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##17")) {
        fluid.exportCapillaryWavesCSV("capillary_waves.csv", sigma17, fluid.rho, FluidConstants::g, 200);
    }
}

// ── 18. Solitons (KdV) ─────────────────────────────────────────────────────
void GuiApp::renderSim18_Solitons() {
    if (ImGui::CollapsingHeader("Theory: Solitons (KdV Equation)")) {
        ImGui::TextWrapped(
            "Korteweg-de Vries equation: u_t + 6*u*u_x + u_xxx = 0\n\n"
            "Soliton solution: u(x,t) = (c/2) * sech^2(sqrt(c/2)*(x - c*t))\n"
            "Taller solitons travel faster. Solitons survive collisions.");
    }
    static double c18 = 2.0;
    ImGui::InputDouble("Soliton speed c##18", &c18);
    if (ImGui::Button("Compute##18")) { clearPlot(); resultText = "KdV soliton computed.\n"; }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##18")) {
        fluid.exportKdVCSV("kdv_soliton.csv", c18, -20, 20, 500, 50, 5.0);
    }
}

// ── 19. Shock Waves (Burgers) ───────────────────────────────────────────────
void GuiApp::renderSim19_BurgersShock() {
    if (ImGui::CollapsingHeader("Theory: Burgers Equation")) {
        ImGui::TextWrapped(
            "Inviscid Burgers: u_t + u*u_x = 0  (nonlinear hyperbolic)\n"
            "Viscous Burgers: u_t + u*u_x = nu*u_xx\n\n"
            "Shock speed (Rankine-Hugoniot): s = (u_L + u_R)/2\n"
            "Models shock formation and viscous smoothing.");
    }
    static int mode19 = 0;
    const char* modes19[] = {"Inviscid", "Viscous"};
    ImGui::Combo("Mode##19", &mode19, modes19, 2);
    static double uL19 = 1.0, uR19 = 0.0, nu19 = 0.01, tF19 = 0.5;
    ImGui::InputDouble("u_L##19", &uL19); ImGui::SameLine(); ImGui::InputDouble("u_R##19", &uR19);
    ImGui::InputDouble("nu##19", &nu19);
    ImGui::InputDouble("t_final##19", &tF19);
    if (ImGui::Button("Compute##19")) { clearPlot(); resultText = "Burgers computed.\n"; }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##19")) {
        FluidSystem::BurgersState st;
        if (mode19 == 0) st = FluidSystem::solveBurgersInviscid(uL19, uR19, -5, 5, 500, tF19, 0.5);
        else st = FluidSystem::solveBurgersViscous(uL19, uR19, nu19, -5, 5, 500, tF19, 0.001);
        fluid.exportBurgersCSV("burgers.csv", st);
    }
}

// ── 20. Riemann Problem ─────────────────────────────────────────────────────
void GuiApp::renderSim20_RiemannProblem() {
    if (ImGui::CollapsingHeader("Theory: Riemann Problem")) {
        ImGui::TextWrapped(
            "Exact solution to 1D Euler equations with piecewise-constant ICs.\n"
            "Solution contains: left wave (shock or rarefaction), contact discontinuity,\n"
            "right wave (shock or rarefaction).\n\n"
            "Classic test: Sod problem (rhoL=1, pL=1, rhoR=0.125, pR=0.1).");
    }
    static double rhoL20 = 1.0, uL20 = 0.0, pL20 = 1.0;
    static double rhoR20 = 0.125, uR20 = 0.0, pR20 = 0.1;
    static double gamma20 = 1.4, t20 = 0.2;
    ImGui::InputDouble("rhoL##20", &rhoL20); ImGui::SameLine(); ImGui::InputDouble("rhoR##20", &rhoR20);
    ImGui::InputDouble("pL##20", &pL20); ImGui::SameLine(); ImGui::InputDouble("pR##20", &pR20);
    ImGui::InputDouble("t##20", &t20);
    if (ImGui::Button("Compute##20")) { clearPlot(); resultText = "Riemann problem solved.\n"; }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##20")) {
        auto sol = FluidSystem::solveRiemannExact(rhoL20, uL20, pL20, rhoR20, uR20, pR20, gamma20, 0, 1, 500, t20);
        fluid.exportRiemannCSV("riemann.csv", sol);
    }
}

// ── 21–50: Stubs with theory headers ────────────────────────────────────────

void GuiApp::renderSim21_SpeedOfSound() {
    if (ImGui::CollapsingHeader("Theory: Speed of Sound")) {
        ImGui::TextWrapped("Ideal gas: c = sqrt(gamma * R_specific * T)\nLiquid: c = sqrt(K/rho)\nMach number: Ma = v/c. Mach angle: mu = arcsin(1/M).");
    }
    static double gamma21 = 1.4, Tmin21 = 200, Tmax21 = 400;
    ImGui::InputDouble("gamma##21", &gamma21);
    ImGui::InputDouble("T_min (K)##21", &Tmin21); ImGui::SameLine(); ImGui::InputDouble("T_max (K)##21", &Tmax21);
    if (ImGui::Button("Compute##21")) { clearPlot(); resultText = "Speed of sound computed.\n"; }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##21")) { fluid.exportSpeedOfSoundCSV("speed_of_sound.csv", gamma21, FluidConstants::R_air, Tmin21, Tmax21, 200); }
}

void GuiApp::renderSim22_IsentropicFlow() {
    if (ImGui::CollapsingHeader("Theory: Isentropic Flow")) {
        ImGui::TextWrapped("Compressible isentropic relations:\nP/P0 = (1 + (g-1)/2 * M^2)^(-g/(g-1))\nT/T0, rho/rho0, A/A*  similarly.");
    }
    static double gamma22 = 1.4, Mmax22 = 5.0;
    ImGui::InputDouble("gamma##22", &gamma22); ImGui::InputDouble("M_max##22", &Mmax22);
    if (ImGui::Button("Compute##22")) { clearPlot(); resultText = "Isentropic flow computed.\n"; }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##22")) { fluid.exportIsentropicFlowCSV("isentropic.csv", gamma22, Mmax22, 200); }
}

void GuiApp::renderSim23_NormalShocks() {
    if (ImGui::CollapsingHeader("Theory: Normal Shock Relations")) {
        ImGui::TextWrapped("Jump conditions across a normal shock:\nM2, p2/p1, rho2/rho1, T2/T1, p02/p01 as functions of M1 and gamma.");
    }
    static double gamma23 = 1.4, M1min23 = 1.0, M1max23 = 5.0;
    ImGui::InputDouble("gamma##23", &gamma23);
    if (ImGui::Button("Compute##23")) { clearPlot(); resultText = "Normal shocks computed.\n"; }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##23")) { fluid.exportNormalShockCSV("normal_shocks.csv", gamma23, M1min23, M1max23, 200); }
}

void GuiApp::renderSim24_ObliqueShocks() {
    if (ImGui::CollapsingHeader("Theory: Oblique Shock Waves")) {
        ImGui::TextWrapped("Oblique shock: flow turns by deflection angle theta.\nShock angle beta found from theta-beta-M relation.\nWeak and strong shock solutions exist for theta < theta_max.");
    }
    static double M1_24 = 3.0, gamma24 = 1.4;
    ImGui::InputDouble("M1##24", &M1_24); ImGui::InputDouble("gamma##24", &gamma24);
    if (ImGui::Button("Compute##24")) { clearPlot(); resultText = "Oblique shocks computed.\n"; }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##24")) { fluid.exportObliqueShockCSV("oblique_shocks.csv", M1_24, gamma24, 100); }
}

void GuiApp::renderSim25_PrandtlMeyer() {
    if (ImGui::CollapsingHeader("Theory: Prandtl-Meyer Expansion")) {
        ImGui::TextWrapped("Isentropic expansion around a convex corner.\nPrandtl-Meyer function: nu(M) = sqrt((g+1)/(g-1)) * arctan(...) - arctan(...)\nM2 found from nu(M2) = nu(M1) + delta_theta.");
    }
    static double gamma25 = 1.4, Mmax25 = 5.0;
    ImGui::InputDouble("gamma##25", &gamma25);
    if (ImGui::Button("Compute##25")) { clearPlot(); resultText = "Prandtl-Meyer computed.\n"; }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##25")) { fluid.exportPrandtlMeyerCSV("prandtl_meyer.csv", gamma25, Mmax25, 200); }
}

void GuiApp::renderSim26_NozzleFlow() {
    if (ImGui::CollapsingHeader("Theory: De Laval Nozzle")) {
        ImGui::TextWrapped("Converging-diverging nozzle: subsonic in convergent section, sonic at throat (M=1),\nsupersonic in divergent section (design condition).\nA/A* = f(M, gamma) from isentropic area-Mach relation.");
    }
    static double gamma26 = 1.4, p0_26 = 101325, T0_26 = 300;
    ImGui::InputDouble("gamma##26", &gamma26);
    ImGui::InputDouble("P0 (Pa)##26", &p0_26); ImGui::InputDouble("T0 (K)##26", &T0_26);
    if (ImGui::Button("Compute##26")) { clearPlot(); resultText = "Nozzle flow computed.\n"; }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##26")) {
        auto res = fluid.solveDeLavalNozzle(gamma26, p0_26, T0_26, FluidConstants::R_air, 0.01, 0.02, 200);
        fluid.exportNozzleFlowCSV("nozzle_flow.csv", res);
    }
}

void GuiApp::renderSim27_FannoFlow() {
    if (ImGui::CollapsingHeader("Theory: Fanno Flow")) {
        ImGui::TextWrapped("Adiabatic flow with friction in a constant-area duct.\nFriction drives flow toward Mach 1 (choking).\nfL*/D, T/T*, p/p*, p0/p0* as functions of M.");
    }
    static double gamma27 = 1.4, Mmax27 = 3.0;
    ImGui::InputDouble("gamma##27", &gamma27);
    if (ImGui::Button("Compute##27")) { clearPlot(); resultText = "Fanno flow computed.\n"; }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##27")) { fluid.exportFannoFlowCSV("fanno_flow.csv", gamma27, Mmax27, 200); }
}

void GuiApp::renderSim28_RayleighFlow() {
    if (ImGui::CollapsingHeader("Theory: Rayleigh Flow")) {
        ImGui::TextWrapped("Frictionless flow with heat addition in a constant-area duct.\nHeat drives flow toward Mach 1.\nT/T*, p/p*, T0/T0*, p0/p0* as functions of M.");
    }
    static double gamma28 = 1.4, Mmax28 = 3.0;
    ImGui::InputDouble("gamma##28", &gamma28);
    if (ImGui::Button("Compute##28")) { clearPlot(); resultText = "Rayleigh flow computed.\n"; }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##28")) { fluid.exportRayleighFlowCSV("rayleigh_flow.csv", gamma28, Mmax28, 200); }
}

void GuiApp::renderSim29_ReynoldsRegimes() {
    if (ImGui::CollapsingHeader("Theory: Reynolds Number Regimes")) {
        ImGui::TextWrapped("Re = rho*U*L/mu. Flow regimes: creeping (<1), laminar (<2300), transitional (2300-4000), turbulent (>4000).\nDrag coefficient for sphere, Darcy friction factor for pipes.");
    }
    if (ImGui::Button("Compute##29")) { clearPlot(); resultText = "Reynolds regimes computed.\n"; }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##29")) { fluid.exportReynoldsRegimesCSV("reynolds_regimes.csv", 200); }
}

void GuiApp::renderSim30_KolmogorovCascade() {
    if (ImGui::CollapsingHeader("Theory: Kolmogorov Energy Cascade")) {
        ImGui::TextWrapped("Turbulent energy cascade: energy injected at large scales, cascades to small scales.\nInertial subrange: E(k) = C_K * epsilon^(2/3) * k^(-5/3)\nKolmogorov scales: eta = (nu^3/epsilon)^(1/4), tau_eta = (nu/epsilon)^(1/2).");
    }
    static double nu30 = 1e-6, eps30 = 1.0, CK = 1.5;
    ImGui::InputDouble("nu##30", &nu30); ImGui::InputDouble("epsilon##30", &eps30);
    if (ImGui::Button("Compute##30")) { clearPlot(); resultText = "Kolmogorov cascade computed.\n"; }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##30")) { fluid.exportKolmogorovCSV("kolmogorov.csv", nu30, eps30, CK, 200); }
}

void GuiApp::renderSim31_MixingLength() {
    if (ImGui::CollapsingHeader("Theory: Mixing Length / Log Law")) {
        ImGui::TextWrapped("Prandtl mixing length: l_m = kappa*y (von Karman const kappa~0.41).\nTurbulent viscosity: nu_t = l_m^2 * |du/dy|.\nLog law: u+ = (1/kappa)*ln(y+) + B,  B ≈ 5.0.\nViscous sublayer (y+ < 5): u+ = y+.");
    }
    static double Uinf31 = 10.0, nu31 = 1.5e-5;
    ImGui::InputDouble("U_inf (m/s)##31", &Uinf31); ImGui::InputDouble("nu##31", &nu31);
    if (ImGui::Button("Compute##31")) { clearPlot(); resultText = "Mixing length computed.\n"; }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##31")) { fluid.exportMixingLengthCSV("mixing_length.csv", Uinf31, nu31, 200); }
}

void GuiApp::renderSim32_BlasiusBL() {
    if (ImGui::CollapsingHeader("Theory: Blasius Boundary Layer")) {
        ImGui::TextWrapped("Laminar flat-plate BL. Blasius equation: f''' + 0.5*f*f'' = 0.\nBL thickness: delta = 5*x/sqrt(Re_x).\nSkin friction: Cf = 0.664/sqrt(Re_x).\nf''(0) = 0.33206 (shooting method).");
    }
    static double Uinf32 = 10.0, nu32 = 1.5e-5, xMax32 = 1.0;
    ImGui::InputDouble("U_inf##32", &Uinf32); ImGui::InputDouble("nu##32", &nu32);
    ImGui::InputDouble("x_max##32", &xMax32);
    if (ImGui::Button("Compute##32")) { clearPlot(); resultText = "Blasius BL computed.\n"; }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##32")) { fluid.exportBlasiusCSV("blasius.csv", Uinf32, nu32, xMax32, 200); }
}

void GuiApp::renderSim33_DragSphere() {
    if (ImGui::CollapsingHeader("Theory: Drag on a Sphere")) {
        ImGui::TextWrapped("Cd(Re) correlation: Stokes (24/Re) at low Re, ~0.44 for 1e3<Re<2e5,\ndrag crisis at Re~2e5. Terminal velocity from force balance.\nOseen correction: Cd = 24/Re * (1 + 3Re/16).");
    }
    static double rhoP33 = 2700.0, d33 = 0.01;
    ImGui::InputDouble("rho_particle##33", &rhoP33); ImGui::InputDouble("d (m)##33", &d33);
    if (ImGui::Button("Compute##33")) { clearPlot(); resultText = "Drag sphere computed.\n"; }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##33")) { fluid.exportDragSphereCSV("drag_sphere.csv", fluid.rho, rhoP33, d33, fluid.mu, FluidConstants::g, 200); }
}

void GuiApp::renderSim34_PipeFlowMoody() {
    if (ImGui::CollapsingHeader("Theory: Pipe Flow (Moody Diagram)")) {
        ImGui::TextWrapped("Darcy-Weisbach: hL = f*L/D * V^2/(2g).\nLaminar: f = 64/Re.\nTurbulent: Colebrook equation: 1/sqrt(f) = -2*log10(eps/D/3.7 + 2.51/(Re*sqrt(f))).\nMoody diagram: f vs Re for various eps/D.");
    }
    if (ImGui::Button("Compute##34")) { clearPlot(); resultText = "Moody diagram computed.\n"; }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##34")) { fluid.exportMoodyDiagramCSV("moody.csv", 200, 8); }
}

void GuiApp::renderSim35_HeatConvection() {
    if (ImGui::CollapsingHeader("Theory: Forced Convection Heat Transfer")) {
        ImGui::TextWrapped("Nusselt number correlations:\n  Flat plate laminar: Nu = 0.332*Re^0.5*Pr^(1/3)\n  Turbulent: Nu = 0.0296*Re^0.8*Pr^(1/3)\n  Pipe (Dittus-Boelter): Nu = 0.023*Re^0.8*Pr^0.4\n  h = Nu*k/L");
    }
    static double Pr35 = FluidConstants::Pr_water, Remax35 = 1e6;
    ImGui::InputDouble("Pr##35", &Pr35);
    if (ImGui::Button("Compute##35")) { clearPlot(); resultText = "Heat convection computed.\n"; }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##35")) { fluid.exportHeatConvectionCSV("heat_convection.csv", Pr35, Remax35, 200); }
}

void GuiApp::renderSim36_MassDiffusion() {
    if (ImGui::CollapsingHeader("Theory: Mass Diffusion (Fick's Law)")) {
        ImGui::TextWrapped("Fick's first law: J = -D * dC/dx.\nFick's second law: dC/dt = D * d^2C/dx^2.\n1D Gaussian solution: C(x,t) = C0/sqrt(4*pi*D*t) * exp(-x^2/(4Dt)).\nSchmidt number: Sc = nu/D. Sherwood number: Sh = 0.332*Re^0.5*Sc^(1/3).");
    }
    static double D36 = 1e-9, C0_36 = 1.0;
    ImGui::InputDouble("D (m^2/s)##36", &D36); ImGui::InputDouble("C0##36", &C0_36);
    if (ImGui::Button("Compute##36")) { clearPlot(); resultText = "Diffusion computed.\n"; }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##36")) { fluid.exportDiffusionCSV("diffusion.csv", C0_36, D36, 0.01, 200, 10, 100.0); }
}

void GuiApp::renderSim37_MHD() {
    if (ImGui::CollapsingHeader("Theory: Magnetohydrodynamics (MHD)")) {
        ImGui::TextWrapped("MHD couples fluid dynamics with Maxwell's equations.\nAlfven speed: v_A = B/sqrt(mu0*rho).\nMagnetic pressure: P_B = B^2/(2*mu0).\nHartmann number: Ha = B*L*sqrt(sigma/mu).\nMagnetic Reynolds number: Rm = U*L/eta.");
    }
    static double B37 = 1e-4;
    ImGui::InputDouble("B (T)##37", &B37);
    if (ImGui::Button("Compute##37")) { clearPlot(); resultText = "MHD computed.\n"; }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##37")) {
        auto st = fluid.solveMHD1D(1.0, 0.125, 1.0, 0.1, B37, 1.0, B37, -1.0, 0, 1, 400, 0.2);
        fluid.exportMHDCSV("mhd.csv", st);
    }
}

void GuiApp::renderSim38_AlfvenWaves() {
    if (ImGui::CollapsingHeader("Theory: Alfven Waves")) {
        ImGui::TextWrapped("Transverse MHD waves propagating along magnetic field lines.\nDispersion: omega = k * v_A.\nGroup velocity = Alfven speed v_A.\nIncompressible and non-dispersive.");
    }
    static double B38 = 1e-4, rho38 = 1e3, dB38 = 1e-6;
    ImGui::InputDouble("B0 (T)##38", &B38); ImGui::InputDouble("dB##38", &dB38);
    if (ImGui::Button("Compute##38")) { clearPlot(); resultText = "Alfven waves computed.\n"; }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##38")) { fluid.exportAlfvenWavesCSV("alfven_waves.csv", B38, rho38, FluidConstants::mu_0, dB38, 10, 100); }
}

void GuiApp::renderSim39_PlasmaBeta() {
    if (ImGui::CollapsingHeader("Theory: Plasma Beta & Parameters")) {
        ImGui::TextWrapped("Plasma beta: beta = 2*mu0*p / B^2.\nbeta > 1: pressure-dominated. beta < 1: magnetically-dominated.\nDebye length: lambda_D = sqrt(eps0*kT/(n*q^2)).\nPlasma frequency: omega_p = sqrt(n*q^2/(m*eps0)). Gyroradius: r_g = m*v_perp/(q*B).");
    }
    static double T39 = 1e6, n39 = 1e18, Bmin39 = 1e-5, Bmax39 = 1e-2;
    ImGui::InputDouble("T (K)##39", &T39); ImGui::InputDouble("n (m^-3)##39", &n39);
    if (ImGui::Button("Compute##39")) { clearPlot(); resultText = "Plasma parameters computed.\n"; }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##39")) { fluid.exportPlasmaBetaCSV("plasma_beta.csv", T39, n39, Bmin39, Bmax39, 200); }
}

void GuiApp::renderSim40_AccretionDisk() {
    if (ImGui::CollapsingHeader("Theory: Accretion Disk Model")) {
        ImGui::TextWrapped("Keplerian disk: v_K = sqrt(GM/r), Omega_K = sqrt(GM/r^3).\nDisk temperature: T ~ (3GMdot/(8*pi*r^3*sigma_SB))^(1/4).\nEddington luminosity: L_Edd = 4*pi*G*M*m_p*c / sigma_T.");
    }
    static double M40 = 2e30, Mdot40 = 1e18, rMin40 = 1e7, rMax40 = 1e10;
    ImGui::InputDouble("M (kg)##40", &M40); ImGui::InputDouble("Mdot (kg/s)##40", &Mdot40);
    if (ImGui::Button("Compute##40")) { clearPlot(); resultText = "Accretion disk computed.\n"; }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##40")) { fluid.exportAccretionDiskCSV("accretion_disk.csv", M40, Mdot40, rMin40, rMax40, 200); }
}

void GuiApp::renderSim41_JeansInstability() {
    if (ImGui::CollapsingHeader("Theory: Jeans Instability")) {
        ImGui::TextWrapped("Gravitational instability in a self-gravitating gas.\nJeans length: lambda_J = c_s * sqrt(pi/(G*rho)).\nJeans mass: M_J = (4/3)*pi*rho*(lambda_J/2)^3.\nFree-fall time: t_ff = sqrt(3*pi/(32*G*rho)).");
    }
    static double cs41 = 200.0, rho41 = 1e-20;
    double G41 = 6.674e-11;
    ImGui::InputDouble("c_s (m/s)##41", &cs41); ImGui::InputDouble("rho (kg/m^3)##41", &rho41);
    if (ImGui::Button("Compute##41")) { clearPlot(); resultText = "Jeans instability computed.\n"; }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##41")) { fluid.exportJeansInstabilityCSV("jeans.csv", cs41, G41, rho41, 200); }
}

void GuiApp::renderSim42_BondiAccretion() {
    if (ImGui::CollapsingHeader("Theory: Bondi Accretion")) {
        ImGui::TextWrapped("Spherically symmetric accretion onto a point mass.\nBondi radius: r_B = GM/c_s^2.\nAccretion rate: Mdot ~ 4*pi*rho_inf*r_B^2*c_s.\nTransonic solution: subsonic at infinity, supersonic near center.");
    }
    static double M42 = 2e30, cs42 = 1e4, rho42 = 1e-20, gamma42 = 5.0/3.0;
    ImGui::InputDouble("M (kg)##42", &M42); ImGui::InputDouble("c_s##42", &cs42);
    if (ImGui::Button("Compute##42")) { clearPlot(); resultText = "Bondi accretion computed.\n"; }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##42")) { fluid.exportBondiAccretionCSV("bondi.csv", M42, cs42, rho42, 6.674e-11, gamma42, 200); }
}

void GuiApp::renderSim43_ViscousDisk() {
    if (ImGui::CollapsingHeader("Theory: Viscous Disk (Shakura-Sunyaev)")) {
        ImGui::TextWrapped("Shakura-Sunyaev alpha prescription: nu = alpha * c_s * H.\nDisk scale height: H = c_s / Omega_K.\nViscous timescale: t_visc = R^2/nu.\nSurface density evolution: diffusion equation in radius.");
    }
    static double alpha43 = 0.1, M43 = 2e30, Mdot43 = 1e18;
    ImGui::InputDouble("alpha_SS##43", &alpha43); ImGui::InputDouble("M (kg)##43", &M43);
    if (ImGui::Button("Compute##43")) { clearPlot(); resultText = "Viscous disk computed.\n"; }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##43")) { fluid.exportViscousDiskCSV("viscous_disk.csv", alpha43, M43, Mdot43, 1e7, 1e10, 200); }
}

void GuiApp::renderSim44_RelativisticFluid() {
    if (ImGui::CollapsingHeader("Theory: Relativistic Fluid Dynamics")) {
        ImGui::TextWrapped("Relativistic enthalpy: w = (rho*c^2 + e + P)/rho.\nLorentz factor: Gamma = 1/sqrt(1 - v^2/c^2).\nRelativistic sound speed < c.\nTaub adiabat for relativistic shocks.");
    }
    static double gamma44 = 5.0/3.0;
    ImGui::InputDouble("gamma_EOS##44", &gamma44);
    if (ImGui::Button("Compute##44")) { clearPlot(); resultText = "Relativistic fluid computed.\n"; }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##44")) { fluid.exportRelativisticFluidCSV("relativistic_fluid.csv", gamma44, FluidConstants::c_light, 200); }
}

void GuiApp::renderSim45_LatticeBoltzmann() {
    if (ImGui::CollapsingHeader("Theory: Lattice Boltzmann Method")) {
        ImGui::TextWrapped("Mesoscopic method: particle distribution functions on a lattice.\nD2Q9 lattice for 2D flow. BGK collision operator: f_out = f - (f-f_eq)/tau.\nRecovers Navier-Stokes in low-Ma limit. nu = c_s^2*(tau - 0.5)*dt.");
    }
    static int nx45 = 51, ny45 = 51, iter45 = 1000;
    static double tau45 = 0.8, uLid45 = 0.1;
    ImGui::SliderInt("nx##45", &nx45, 21, 101);
    ImGui::InputDouble("tau##45", &tau45); ImGui::InputDouble("u_lid##45", &uLid45);
    ImGui::SliderInt("Iterations##45", &iter45, 100, 10000);
    if (ImGui::Button("Compute##45")) { clearPlot(); resultText = "LBM computed.\n"; }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##45")) {
        auto st = fluid.solveLBM_D2Q9(nx45, ny45, tau45, uLid45, iter45);
        fluid.exportLBMCSV("lbm_cavity.csv", st);
    }
}

void GuiApp::renderSim46_SPH() {
    if (ImGui::CollapsingHeader("Theory: Smoothed Particle Hydrodynamics")) {
        ImGui::TextWrapped("Lagrangian meshless method.\nField quantities interpolated via kernel functions: A(r) = sum_j m_j/rho_j * A_j * W(r-r_j, h).\nCubic spline kernel. Momentum equation with pressure gradient.\nToy dam-break scenario.");
    }
    static int nPart46 = 100, steps46 = 50;
    static double h46 = 0.02, dt46 = 0.001;
    ImGui::SliderInt("Particles##46", &nPart46, 25, 400);
    ImGui::SliderInt("Steps##46", &steps46, 10, 500);
    if (ImGui::Button("Compute##46")) { clearPlot(); resultText = "SPH computed.\n"; }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##46")) {
        auto parts = fluid.solveSPH_DamBreak(nPart46, h46, dt46, steps46, 20.0, 1000.0);
        fluid.exportSPHCSV("sph_dambreak.csv", parts);
    }
}

void GuiApp::renderSim47_VortexMethods() {
    if (ImGui::CollapsingHeader("Theory: Vortex Methods")) {
        ImGui::TextWrapped("Lagrangian representation of vorticity.\nBiot-Savart law: v(x) = sum -Gamma_j/(2*pi) * (x-x_j)_perp / |x-x_j|^2.\nBlob regularization: |x|^2 + eps^2 prevents singularity.\nVortex sheet roll-up, leapfrogging vortex pairs.");
    }
    static int nVort47 = 20, steps47 = 100;
    static double dt47 = 0.01, eps47 = 0.1;
    ImGui::SliderInt("Vortices##47", &nVort47, 2, 50);
    ImGui::InputDouble("dt##47", &dt47); ImGui::InputDouble("epsilon##47", &eps47);
    if (ImGui::Button("Compute##47")) { clearPlot(); resultText = "Vortex methods computed.\n"; }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##47")) {
        // Create vortex sheet
        std::vector<FluidSystem::VortexState> vorts(nVort47);
        for (int i = 0; i < nVort47; ++i) { vorts[i] = {(double)i/(nVort47-1) * 2.0 - 1.0, 0.0, 0.1}; }
        auto evolved = fluid.evolvePointVortices(vorts, dt47, steps47);
        fluid.exportVortexMethodCSV("vortex_method.csv", evolved);
    }
}

void GuiApp::renderSim48_MultiphaseFlow() {
    if (ImGui::CollapsingHeader("Theory: Multiphase Flow")) {
        ImGui::TextWrapped("Two-phase gas-liquid flow in pipes.\nVoid fraction: alpha = Qg/(Qg+Ql).\nDrift-flux model: Ug = C0*Um + Ugj.\nSlip ratio: S = (rhoL/rhoG)^(1/3).\nLockhart-Martinelli two-phase multiplier.");
    }
    static double rhoL48 = 1000, rhoG48 = 1.2, muL48 = 1e-3, muG48 = 1.8e-5;
    ImGui::InputDouble("rho_L##48", &rhoL48); ImGui::InputDouble("rho_G##48", &rhoG48);
    if (ImGui::Button("Compute##48")) { clearPlot(); resultText = "Multiphase flow computed.\n"; }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##48")) { fluid.exportMultiphaseFlowCSV("multiphase.csv", rhoL48, rhoG48, muL48, muG48, 200); }
}

void GuiApp::renderSim49_NonNewtonian() {
    if (ImGui::CollapsingHeader("Theory: Non-Newtonian Fluids")) {
        ImGui::TextWrapped("Power law: mu_eff = K * gammaDot^(n-1). n<1 shear-thinning, n>1 shear-thickening.\nBingham: mu_eff = mu_p + tau_y/gammaDot. Plug flow in center.\nCarreau model: mu = mu_inf + (mu_0 - mu_inf)*(1 + (lambda*gammaDot)^2)^((n-1)/2).");
    }
    static int model49 = 0;
    const char* models[] = {"Power Law", "Bingham", "Carreau"};
    ImGui::Combo("Model##49", &model49, models, 3);
    static double p1_49 = 0.1, p2_49 = 0.5, p3_49 = 1.0;
    ImGui::InputDouble("Param1 (K/tau_y/mu0)##49", &p1_49);
    ImGui::InputDouble("Param2 (n/mu_p/muInf)##49", &p2_49);
    ImGui::InputDouble("Param3 (lambda)##49", &p3_49);
    if (ImGui::Button("Compute##49")) { clearPlot(); resultText = "Non-Newtonian computed.\n"; }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##49")) { fluid.exportNonNewtonianCSV("non_newtonian.csv", model49, p1_49, p2_49, p3_49, 200); }
}

void GuiApp::renderSim50_Microfluidics() {
    if (ImGui::CollapsingHeader("Theory: Microfluidics")) {
        ImGui::TextWrapped("Low-Re flows in micro-channels (1-1000 um).\nHydraulic diameter: D_h = 2*w*h/(w+h).\nCapillary number: Ca = mu*U/sigma.\nKnudsen number: Kn = lambda/L (slip flow if Kn > 0.01).\nElectro-osmotic flow: u_eo = -eps*zeta*E/mu.\nMixing time ~ w^2/D (diffusion-limited).");
    }
    static double w50 = 100e-6, h50 = 50e-6, L50 = 0.01;
    ImGui::InputDouble("w (m)##50", &w50); ImGui::InputDouble("h (m)##50", &h50);
    ImGui::InputDouble("L (m)##50", &L50);
    if (ImGui::Button("Compute##50")) { clearPlot(); resultText = "Microfluidics computed.\n"; }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##50")) { fluid.exportMicrofluidicsCSV("microfluidics.csv", fluid.mu, FluidConstants::sigma_water, w50, h50, L50, 200); }
}
