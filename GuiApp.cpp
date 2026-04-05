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
        if (mode == 0) {
            vector<double> hVec(numPoints), pVec(numPoints);
            for (int i = 0; i < numPoints; ++i) {
                hVec[i] = hMax * i / (numPoints - 1.0);
                pVec[i] = FluidSystem::hydrostaticPressure(fluid.rho, g, hVec[i], p0);
            }
            addCurve("P(h)", hVec, pVec);
            oss << "Hydrostatic Pressure Profile\n";
            oss << "  rho = " << fluid.rho << " kg/m^3\n";
            oss << "  P(0) = " << p0 << " Pa\n";
            oss << "  P(" << hMax << ") = " << pVec.back() << " Pa\n";
        } else if (mode == 1) {
            vector<double> vVec(numPoints), fVec(numPoints);
            double vMax = 0.01;
            for (int i = 0; i < numPoints; ++i) {
                vVec[i] = vMax * (i + 1.0) / numPoints;
                fVec[i] = FluidSystem::buoyantForce(fluid.rho, g, vVec[i]);
            }
            addCurve("F_b(V)", vVec, fVec);
            oss << "Buoyancy Force\n";
            oss << "  rho_fluid = " << fluid.rho << " kg/m^3\n";
            oss << "  F_b(1 m^3) = " << FluidSystem::buoyantForce(fluid.rho, g, 1.0) << " N\n";
        } else {
            vector<double> rhoVec(numPoints), fracVec(numPoints);
            for (int i = 0; i < numPoints; ++i) {
                rhoVec[i] = fluid.rho * 2.0 * (i + 1.0) / numPoints;
                fracVec[i] = FluidSystem::submergedFraction(rhoVec[i], fluid.rho);
            }
            addCurve("Submerged fraction", rhoVec, fracVec);
            oss << "Submerged Fraction\n";
            oss << "  rho_fluid = " << fluid.rho << " kg/m^3\n";
            oss << "  rho_obj = rho_fluid => fraction = " << FluidSystem::submergedFraction(fluid.rho, fluid.rho) << "\n";
        }
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
        if (mode == 0) {
            int N = 200;
            vector<double> vVec(N), pVec(N);
            double vMax = 50.0;
            for (int i = 0; i < N; ++i) {
                double v2 = vMax * i / (N - 1.0);
                vVec[i] = v2;
                pVec[i] = FluidSystem::bernoulliPressure(p1, fluid.rho, v1, v2, h1, 0.0, FluidConstants::g);
            }
            addCurve("P2(v2)", vVec, pVec);
            oss << "Bernoulli: P vs Velocity\n";
            oss << "  P1 = " << p1 << " Pa, v1 = " << v1 << " m/s\n";
        } else if (mode == 1) {
            double A1 = 0.01, A2 = 0.005;
            double v2 = FluidSystem::venturiVelocity(A1, A2, v1);
            double p2 = FluidSystem::bernoulliPressure(p1, fluid.rho, v1, v2, 0, 0, 0);
            oss << "Venturi Effect\n";
            oss << "  A1 = " << A1 << " m^2, A2 = " << A2 << " m^2\n";
            oss << "  v1 = " << v1 << " m/s => v2 = " << v2 << " m/s\n";
            oss << "  P1 = " << p1 << " Pa => P2 = " << p2 << " Pa\n";
            int N = 200;
            vector<double> aVec(N), vOut(N);
            for (int i = 0; i < N; ++i) {
                aVec[i] = A1 * (0.1 + 0.9 * (N - 1 - i) / (N - 1.0));
                vOut[i] = FluidSystem::venturiVelocity(A1, aVec[i], v1);
            }
            addCurve("v2(A2)", aVec, vOut);
        } else if (mode == 2) {
            double pStag = p1 + 0.5 * fluid.rho * v1 * v1;
            double vPitot = FluidSystem::pitotVelocity(pStag, p1, fluid.rho);
            oss << "Pitot Tube\n";
            oss << "  P_stag = " << pStag << " Pa, P_static = " << p1 << " Pa\n";
            oss << "  v = " << vPitot << " m/s\n";
        } else {
            int N = 200;
            vector<double> hVec(N), vVec(N);
            for (int i = 0; i < N; ++i) {
                hVec[i] = 10.0 * (i + 1.0) / N;
                vVec[i] = FluidSystem::torricelliVelocity(FluidConstants::g, hVec[i]);
            }
            addCurve("v(h)", hVec, vVec);
            oss << "Torricelli's Theorem\n";
            oss << "  v = sqrt(2*g*h)\n";
            oss << "  v(1m) = " << FluidSystem::torricelliVelocity(FluidConstants::g, 1.0) << " m/s\n";
        }
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
        auto state = FluidSystem::solveEuler1D(rhoL, uL, pL, rhoR, uR, pR, gamma, 0, 1, nx, tFinal, 0.5);
        addCurve("Density", state.x, state.rhoField);
        addCurve("Velocity", state.x, state.u);
        addCurve("Pressure", state.x, state.p);
        oss << "Euler 1D Riemann Problem\n";
        oss << "  Left:  rho=" << rhoL << ", u=" << uL << ", p=" << pL << "\n";
        oss << "  Right: rho=" << rhoR << ", u=" << uR << ", p=" << pR << "\n";
        oss << "  gamma=" << gamma << ", t=" << tFinal << ", nx=" << nx << "\n";
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
        auto field = fluid.solveNavierStokes2DCavity(nx, ny, Re, dt, maxIter);
        vector<double> yVec(ny), uCenter(ny);
        int midX = nx / 2;
        for (int j = 0; j < ny; ++j) {
            yVec[j] = j * field.dy;
            uCenter[j] = field.u[j * nx + midX];
        }
        addCurve("u(y) centerline", yVec, uCenter);
        vector<double> xVec(nx), vCenter(nx);
        int midY = ny / 2;
        for (int i = 0; i < nx; ++i) {
            xVec[i] = i * field.dx;
            vCenter[i] = field.v[midY * nx + i];
        }
        addCurve("v(x) centerline", xVec, vCenter);
        oss << "Navier-Stokes 2D Lid-Driven Cavity\n";
        oss << "  Re = " << Re << ", grid = " << nx << "x" << ny << "\n";
        oss << "  u_max at center = " << *max_element(uCenter.begin(), uCenter.end()) << "\n";
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
        double Q = FluidSystem::poiseuilleFlowRate(dpdx, fluid.mu, R);
        double uMax = FluidSystem::poiseuilleMaxVelocity(dpdx, fluid.mu, R);
        double tauW = FluidSystem::poiseuilleWallShear(dpdx, R);
        oss << "Poiseuille Flow\n";
        oss << "  Q = " << Q << " m^3/s\n";
        oss << "  u_max = " << uMax << " m/s\n";
        oss << "  tau_wall = " << tauW << " Pa\n";
        if (mode == 0) {
            vector<double> rVec(numPoints), uVec(numPoints);
            for (int i = 0; i < numPoints; ++i) {
                rVec[i] = -R + 2.0 * R * i / (numPoints - 1.0);
                uVec[i] = FluidSystem::poiseuilleVelocity(dpdx, fluid.mu, R, fabs(rVec[i]));
            }
            addCurve("u(r) pipe", rVec, uVec);
        } else {
            vector<double> yVec(numPoints), uVec(numPoints);
            for (int i = 0; i < numPoints; ++i) {
                yVec[i] = R * i / (numPoints - 1.0);
                uVec[i] = FluidSystem::poiseuilleChannelVelocity(dpdx, fluid.mu, R, yVec[i]);
            }
            addCurve("u(y) channel", yVec, uVec);
        }
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
        ostringstream oss;
        vector<double> yVec(numPoints), uVec(numPoints), tauVec(numPoints);
        for (int i = 0; i < numPoints; ++i) {
            yVec[i] = h * i / (numPoints - 1.0);
            uVec[i] = FluidSystem::couetteVelocity(Utop, h, yVec[i], dpdx, fluid.mu);
            tauVec[i] = FluidSystem::couetteShearStress(fluid.mu, Utop, h, dpdx, yVec[i]);
        }
        addCurve("u(y)", yVec, uVec);
        addCurve("tau(y)", yVec, tauVec);
        oss << "Couette Flow\n";
        oss << "  U_top = " << Utop << " m/s, h = " << h << " m\n";
        oss << "  dP/dx = " << dpdx << " Pa/m\n";
        oss << "  u(h/2) = " << FluidSystem::couetteVelocity(Utop, h, h/2, dpdx, fluid.mu) << " m/s\n";
        oss << "  tau_wall_bottom = " << FluidSystem::couetteShearStress(fluid.mu, Utop, h, dpdx, 0) << " Pa\n";
        resultText = oss.str();
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
        double Re_s = fluid.rho * Us * 2 * Rs / fluid.mu;
        double Cd = FluidSystem::stokesDragCoefficient(Re_s);
        double vT = FluidSystem::stokesTerminalVelocity(2700, fluid.rho, FluidConstants::g, fluid.mu, Rs);
        oss << "Stokes Flow\n";
        oss << "  Drag = " << F << " N\n";
        oss << "  Re = " << Re_s << ", Cd = " << Cd << "\n";
        oss << "  Terminal velocity (rhoP=2700) = " << vT << " m/s\n";
        int N = 200;
        vector<double> reVec(N), cdVec(N);
        for (int i = 0; i < N; ++i) {
            reVec[i] = 0.01 + 1.0 * i;
            cdVec[i] = FluidSystem::stokesDragCoefficient(reVec[i]);
        }
        addCurve("Cd(Re) Stokes", reVec, cdVec);
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
        ostringstream oss;
        if (mode == 0) {
            int N = 200;
            vector<double> thetaVec(N), cpVec(N);
            for (int i = 0; i < N; ++i) {
                double theta = 2.0 * M_PI * i / (N - 1.0);
                thetaVec[i] = theta * 180.0 / M_PI;
                cpVec[i] = 1.0 - 4.0 * sin(theta) * sin(theta);
            }
            addCurve("Cp(theta)", thetaVec, cpVec);
            oss << "Potential Flow Around Cylinder\n";
            oss << "  U_inf = " << Uf << " m/s, R = " << Rf << " m\n";
            oss << "  Cp_min = -3 (at theta = 90, 270 deg)\n";
            oss << "  D'Alembert paradox: zero drag in inviscid flow\n";
        } else if (mode == 1) {
            int N = 200;
            double m = 1.0;
            vector<double> rVec(N), vrVec(N);
            for (int i = 0; i < N; ++i) {
                double r = 0.1 + 5.0 * i / (N - 1.0);
                rVec[i] = r;
                auto [u, v] = FluidSystem::sourceFlow(m, r, 0.0, 0.0, 0.0);
                vrVec[i] = sqrt(u * u + v * v);
            }
            addCurve("|v|(r) source", rVec, vrVec);
            oss << "Source Flow, m = " << m << "\n";
        } else if (mode == 2) {
            int N = 200;
            double gammaV = 1.0;
            vector<double> rVec(N), vtVec(N);
            for (int i = 0; i < N; ++i) {
                double r = 0.1 + 5.0 * i / (N - 1.0);
                rVec[i] = r;
                auto [u, v] = FluidSystem::vortexFlow(gammaV, r, 0.0, 0.0, 0.0);
                vtVec[i] = sqrt(u * u + v * v);
            }
            addCurve("|v|(r) vortex", rVec, vtVec);
            oss << "Vortex Flow, Gamma = " << gammaV << "\n";
        } else {
            oss << "Rankine Oval (superposition of uniform + source/sink pair)\n";
            oss << "  U = " << Uf << ", R = " << Rf << "\n";
            int N = 200;
            double m = 1.0, a = 1.0;
            vector<double> xVec(N), psiVec(N);
            for (int i = 0; i < N; ++i) {
                double x = -3.0 + 6.0 * i / (N - 1.0);
                xVec[i] = x;
                psiVec[i] = FluidSystem::streamFunctionRankineOval(Uf, m, a, x, 0.5);
            }
            addCurve("psi(x,y=0.5)", xVec, psiVec);
        }
        resultText = oss.str();
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
    if (ImGui::Button("Compute##09")) {
        clearPlot();
        ostringstream oss;
        int N = 200;
        vector<double> xVec(N), psiVec(N);
        for (int i = 0; i < N; ++i) {
            double x = -5.0 + 10.0 * i / (N - 1.0);
            xVec[i] = x;
            switch (type) {
                case 0: psiVec[i] = FluidSystem::streamFunctionUniform(param1, x, 1.0); break;
                case 1: psiVec[i] = FluidSystem::streamFunctionSource(param1, x, 1.0); break;
                case 2: psiVec[i] = FluidSystem::streamFunctionVortex(param1, x, 1.0); break;
                case 3: psiVec[i] = FluidSystem::streamFunctionDoublet(param1, x, 1.0); break;
                case 4: psiVec[i] = FluidSystem::streamFunctionRankineOval(param1, param2, 1.0, x, 1.0); break;
            }
        }
        addCurve("psi(x, y=1)", xVec, psiVec);
        oss << "Stream Function (type " << type << ")\n";
        oss << "  Evaluated along y = 1\n";
        oss << "  param1 = " << param1 << ", param2 = " << param2 << "\n";
        resultText = oss.str();
    }
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
    if (ImGui::Button("Compute##10")) {
        clearPlot();
        ostringstream oss;
        int N = 200;
        if (mode == 0) {
            vector<double> rVec(N), vVec(N);
            for (int i = 0; i < N; ++i) {
                rVec[i] = 5.0 * R10 * (i + 1.0) / N;
                vVec[i] = FluidSystem::rankineVortexVelocity(gamma10, R10, rVec[i]);
            }
            addCurve("v_theta(r) Rankine", rVec, vVec);
            oss << "Rankine Vortex\n";
            oss << "  Gamma = " << gamma10 << ", R_core = " << R10 << "\n";
            oss << "  v_max = " << FluidSystem::rankineVortexVelocity(gamma10, R10, R10) << " m/s\n";
        } else if (mode == 1) {
            double t = 1.0;
            vector<double> rVec(N), vVec(N);
            for (int i = 0; i < N; ++i) {
                rVec[i] = 5.0 * R10 * (i + 1.0) / N;
                vVec[i] = FluidSystem::lambOseenVortexVelocity(gamma10, nu10, rVec[i], t);
            }
            addCurve("v_theta(r) Lamb-Oseen", rVec, vVec);
            oss << "Lamb-Oseen Vortex (t = " << t << ")\n";
            oss << "  Gamma = " << gamma10 << ", nu = " << nu10 << "\n";
        } else {
            vector<FluidSystem::VortexState> vortices = {{-0.5, 0, gamma10}, {0.5, 0, -gamma10}};
            auto result = fluid.evolvePointVortices(vortices, 0.01, 100);
            vector<double> xVec(result.size()), yVec(result.size());
            for (size_t i = 0; i < result.size(); ++i) { xVec[i] = result[i].x; yVec[i] = result[i].y; }
            addCurve("Vortex trajectory", xVec, yVec);
            oss << "Point Vortex Pair\n";
            oss << "  Gamma = +/-" << gamma10 << "\n";
        }
        resultText = oss.str();
    }
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
    if (ImGui::Button("Compute##11")) {
        clearPlot();
        ostringstream oss;
        int N = 200;
        double g_val = FluidConstants::g;
        double omega = FluidSystem::generalDispersion(k, g_val, h);
        double cPhase = omega / k;
        double cGroup = FluidSystem::waveGroupVelocity(k, g_val, h);
        double E = FluidSystem::waveEnergy(fluid.rho, g_val, A);
        oss << "Surface Gravity Waves\n";
        oss << "  omega = " << omega << " rad/s\n";
        oss << "  c_phase = " << cPhase << " m/s\n";
        oss << "  c_group = " << cGroup << " m/s\n";
        oss << "  Wave energy = " << E << " J/m^2\n";
        oss << "  Shallow water speed = " << FluidSystem::shallowWaterSpeed(g_val, h) << " m/s\n";
        vector<double> xVec(N), etaVec(N);
        for (int i = 0; i < N; ++i) {
            double x = 4.0 * M_PI / k * i / (N - 1.0);
            xVec[i] = x;
            etaVec[i] = FluidSystem::stokesSecondOrderEta(A, k, omega, x, 0.0, h);
        }
        addCurve("eta(x, t=0)", xVec, etaVec);
        resultText = oss.str();
    }
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
    if (ImGui::Button("Compute##12")) {
        clearPlot();
        ostringstream oss;
        auto st = FluidSystem::solveShallowWater1D(hL, hR, uL12, uR12, FluidConstants::g, -5, 5, 400, tF, 0.5);
        addCurve("h(x)", st.x, st.h);
        addCurve("u(x)", st.x, st.u);
        oss << "Shallow Water 1D Dam Break\n";
        oss << "  h_L = " << hL << ", h_R = " << hR << "\n";
        oss << "  t = " << tF << "\n";
        double hMin = *min_element(st.h.begin(), st.h.end());
        double hMax_val = *max_element(st.h.begin(), st.h.end());
        oss << "  h range: [" << hMin << ", " << hMax_val << "]\n";
        resultText = oss.str();
    }
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
    if (ImGui::Button("Compute##13")) {
        clearPlot();
        ostringstream oss;
        int N = 200;
        vector<double> kVec(N), sigmaVec(N);
        for (int i = 0; i < N; ++i) {
            double kv = 0.1 + 50.0 * i / (N - 1.0);
            kVec[i] = kv;
            sigmaVec[i] = FluidSystem::khGrowthRate(rho1, rho2, 0, dU, kv, FluidConstants::g, sigma13);
        }
        addCurve("sigma(k)", kVec, sigmaVec);
        double kCrit = 2.0 * M_PI / FluidSystem::khCriticalWavelength(rho1, rho2, dU, sigma13);
        oss << "Kelvin-Helmholtz Instability\n";
        oss << "  rho1 = " << rho1 << ", rho2 = " << rho2 << "\n";
        oss << "  Delta U = " << dU << " m/s\n";
        oss << "  Critical wavelength = " << FluidSystem::khCriticalWavelength(rho1, rho2, dU, sigma13) << " m\n";
        resultText = oss.str();
    }
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
    if (ImGui::Button("Compute##14")) {
        clearPlot();
        ostringstream oss;
        int N = 200;
        vector<double> kVec(N), sigmaVec(N);
        for (int i = 0; i < N; ++i) {
            double kv = 0.1 + 100.0 * i / (N - 1.0);
            kVec[i] = kv;
            sigmaVec[i] = FluidSystem::rtGrowthRate(rho1_14, rho2_14, FluidConstants::g, kv, sigma14);
        }
        addCurve("sigma(k)", kVec, sigmaVec);
        double At = FluidSystem::attwoodNumber(rho1_14, rho2_14);
        double lambdaC = FluidSystem::rtCriticalWavelength(rho1_14, rho2_14, FluidConstants::g, sigma14);
        oss << "Rayleigh-Taylor Instability\n";
        oss << "  rho1 = " << rho1_14 << ", rho2 = " << rho2_14 << "\n";
        oss << "  Atwood number = " << At << "\n";
        oss << "  Critical wavelength = " << lambdaC << " m\n";
        resultText = oss.str();
    }
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
    if (ImGui::Button("Compute##15")) {
        clearPlot();
        ostringstream oss;
        double alpha_th = FluidConstants::k_water / (FluidConstants::rho_water * FluidConstants::cp_water);
        double Ra = FluidSystem::rayleighNumber(FluidConstants::g, FluidConstants::beta_water, dT,
                                                 Lrb, FluidConstants::nu_water, alpha_th);
        double Ra_c = FluidSystem::criticalRayleighNumber();
        double Pr = FluidConstants::Pr_water;
        double Nu = FluidSystem::nusseltNumberRB(Ra, Pr);
        oss << "Rayleigh-Benard Convection\n";
        oss << "  Ra = " << Ra << "\n";
        oss << "  Ra_critical = " << Ra_c << "\n";
        oss << "  Nu = " << Nu << "\n";
        oss << (Ra > Ra_c ? "  Convection active!\n" : "  Below onset — conduction only\n");
        int N = 200;
        vector<double> raVec(N), nuVec(N);
        for (int i = 0; i < N; ++i) {
            raVec[i] = 1000.0 + (1e8 - 1000.0) * i / (N - 1.0);
            nuVec[i] = FluidSystem::nusseltNumberRB(raVec[i], Pr);
        }
        addCurve("Nu(Ra)", raVec, nuVec);
        resultText = oss.str();
    }
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
    if (ImGui::Button("Compute##16")) {
        clearPlot();
        ostringstream oss;
        int N = 200;
        vector<double> rVec(N), vVec(N);
        for (int i = 0; i < N; ++i) {
            rVec[i] = R1tc + (R2tc - R1tc) * i / (N - 1.0);
            vVec[i] = FluidSystem::taylorCouetteVelocity(omega1, omega2, R1tc, R2tc, rVec[i]);
        }
        addCurve("v_theta(r)", rVec, vVec);
        double Ta = FluidSystem::taylorNumber(omega1, R1tc, R2tc, FluidConstants::nu_water);
        double Ta_c = FluidSystem::criticalTaylorNumber();
        oss << "Taylor-Couette Flow\n";
        oss << "  Omega1 = " << omega1 << ", Omega2 = " << omega2 << " rad/s\n";
        oss << "  R1 = " << R1tc << ", R2 = " << R2tc << " m\n";
        oss << "  Taylor number = " << Ta << "\n";
        oss << "  Critical Ta = " << Ta_c << "\n";
        oss << (Ta > Ta_c ? "  Taylor vortices expected!\n" : "  Stable Couette flow\n");
        resultText = oss.str();
    }
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
    if (ImGui::Button("Compute##17")) {
        clearPlot();
        ostringstream oss;
        double g_val = FluidConstants::g;
        double lc = FluidSystem::capillaryLength(sigma17, fluid.rho, g_val);
        double cMin = FluidSystem::capillaryMinPhaseSpeed(sigma17, fluid.rho, g_val);
        oss << "Capillary Waves\n";
        oss << "  sigma = " << sigma17 << " N/m\n";
        oss << "  Capillary length = " << lc << " m\n";
        oss << "  Min phase speed = " << cMin << " m/s\n";
        oss << "  Young-Laplace dP (R=1mm) = " << FluidSystem::youngLaplaceDropletPressure(sigma17, 0.001) << " Pa\n";
        int N = 200;
        vector<double> kVec(N), omegaVec(N);
        for (int i = 0; i < N; ++i) {
            kVec[i] = 1.0 + 1000.0 * i / (N - 1.0);
            omegaVec[i] = FluidSystem::capillaryWaveDispersion(kVec[i], sigma17, fluid.rho, g_val);
        }
        addCurve("omega(k)", kVec, omegaVec);
        resultText = oss.str();
    }
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
    if (ImGui::Button("Compute##18")) {
        clearPlot();
        ostringstream oss;
        int N = 500;
        double xMin = -20, xMax = 20;
        vector<double> xVec(N), uVec(N);
        for (int i = 0; i < N; ++i) {
            xVec[i] = xMin + (xMax - xMin) * i / (N - 1.0);
            uVec[i] = FluidSystem::kdvSoliton(c18, xVec[i], 0.0, 0.0);
        }
        addCurve("u(x, t=0)", xVec, uVec);
        oss << "KdV Soliton\n";
        oss << "  Speed c = " << c18 << "\n";
        oss << "  Amplitude = " << FluidSystem::kdvSolitonAmplitude(c18) << "\n";
        oss << "  Width = " << FluidSystem::kdvSolitonWidth(c18) << "\n";
        resultText = oss.str();
    }
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
    if (ImGui::Button("Compute##19")) {
        clearPlot();
        ostringstream oss;
        FluidSystem::BurgersState st;
        if (mode19 == 0) {
            st = FluidSystem::solveBurgersInviscid(uL19, uR19, -5, 5, 500, tF19, 0.5);
            oss << "Inviscid Burgers Equation\n";
        } else {
            st = FluidSystem::solveBurgersViscous(uL19, uR19, nu19, -5, 5, 500, tF19, 0.001);
            oss << "Viscous Burgers Equation (nu = " << nu19 << ")\n";
        }
        addCurve("u(x)", st.x, st.u);
        double sShock = FluidSystem::burgersExactShockSpeed(uL19, uR19);
        oss << "  u_L = " << uL19 << ", u_R = " << uR19 << "\n";
        oss << "  Shock speed = " << sShock << " m/s\n";
        oss << "  t = " << tF19 << "\n";
        resultText = oss.str();
    }
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
    if (ImGui::Button("Compute##20")) {
        clearPlot();
        ostringstream oss;
        auto sol = FluidSystem::solveRiemannExact(rhoL20, uL20, pL20, rhoR20, uR20, pR20, gamma20, 0, 1, 500, t20);
        addCurve("Density", sol.x, sol.rhoField);
        addCurve("Velocity", sol.x, sol.u);
        addCurve("Pressure", sol.x, sol.p);
        oss << "Riemann Problem (Exact Solver)\n";
        oss << "  Left:  rho=" << rhoL20 << ", u=" << uL20 << ", p=" << pL20 << "\n";
        oss << "  Right: rho=" << rhoR20 << ", u=" << uR20 << ", p=" << pR20 << "\n";
        oss << "  p* = " << sol.pStar << ", u* = " << sol.uStar << "\n";
        resultText = oss.str();
    }
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
    if (ImGui::Button("Compute##21")) {
        clearPlot();
        ostringstream oss;
        int N = 200;
        vector<double> tVec(N), cVec(N);
        for (int i = 0; i < N; ++i) {
            tVec[i] = Tmin21 + (Tmax21 - Tmin21) * i / (N - 1.0);
            cVec[i] = FluidSystem::speedOfSoundIdealGas(gamma21, FluidConstants::R_air, tVec[i]);
        }
        addCurve("c(T)", tVec, cVec);
        double cStd = FluidSystem::speedOfSoundIdealGas(gamma21, FluidConstants::R_air, FluidConstants::T_std);
        double cLiq = FluidSystem::speedOfSoundLiquid(FluidConstants::Kbulk_water, FluidConstants::rho_water);
        oss << "Speed of Sound\n";
        oss << "  Air at 20C: c = " << cStd << " m/s\n";
        oss << "  Water: c = " << cLiq << " m/s\n";
        oss << "  Mach angle at M=2: " << FluidSystem::machAngle(2.0) * 180.0 / M_PI << " deg\n";
        resultText = oss.str();
    }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##21")) { fluid.exportSpeedOfSoundCSV("speed_of_sound.csv", gamma21, FluidConstants::R_air, Tmin21, Tmax21, 200); }
}

void GuiApp::renderSim22_IsentropicFlow() {
    if (ImGui::CollapsingHeader("Theory: Isentropic Flow")) {
        ImGui::TextWrapped("Compressible isentropic relations:\nP/P0 = (1 + (g-1)/2 * M^2)^(-g/(g-1))\nT/T0, rho/rho0, A/A*  similarly.");
    }
    static double gamma22 = 1.4, Mmax22 = 5.0;
    ImGui::InputDouble("gamma##22", &gamma22); ImGui::InputDouble("M_max##22", &Mmax22);
    if (ImGui::Button("Compute##22")) {
        clearPlot();
        ostringstream oss;
        int N = 200;
        vector<double> mVec(N), pRatio(N), tRatio(N), rhoRatio(N), aRatio(N);
        for (int i = 0; i < N; ++i) {
            double M = 0.01 + Mmax22 * i / (N - 1.0);
            mVec[i] = M;
            pRatio[i] = FluidSystem::isentropicPressureRatio(M, gamma22);
            tRatio[i] = FluidSystem::isentropicTemperatureRatio(M, gamma22);
            rhoRatio[i] = FluidSystem::isentropicDensityRatio(M, gamma22);
            aRatio[i] = FluidSystem::isentropicAreaRatio(M, gamma22);
        }
        addCurve("P/P0", mVec, pRatio);
        addCurve("T/T0", mVec, tRatio);
        addCurve("rho/rho0", mVec, rhoRatio);
        oss << "Isentropic Flow Relations (gamma = " << gamma22 << ")\n";
        oss << "  At M=1: P/P0 = " << FluidSystem::isentropicPressureRatio(1.0, gamma22) << "\n";
        oss << "  At M=1: T/T0 = " << FluidSystem::isentropicTemperatureRatio(1.0, gamma22) << "\n";
        oss << "  At M=2: A/A* = " << FluidSystem::isentropicAreaRatio(2.0, gamma22) << "\n";
        resultText = oss.str();
    }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##22")) { fluid.exportIsentropicFlowCSV("isentropic.csv", gamma22, Mmax22, 200); }
}

void GuiApp::renderSim23_NormalShocks() {
    if (ImGui::CollapsingHeader("Theory: Normal Shock Relations")) {
        ImGui::TextWrapped("Jump conditions across a normal shock:\nM2, p2/p1, rho2/rho1, T2/T1, p02/p01 as functions of M1 and gamma.");
    }
    static double gamma23 = 1.4, M1min23 = 1.0, M1max23 = 5.0;
    ImGui::InputDouble("gamma##23", &gamma23);
    if (ImGui::Button("Compute##23")) {
        clearPlot();
        ostringstream oss;
        int N = 200;
        vector<double> mVec(N), m2Vec(N), pRatio(N), rhoRatio(N), tRatio(N);
        for (int i = 0; i < N; ++i) {
            double M1 = M1min23 + (M1max23 - M1min23) * i / (N - 1.0);
            mVec[i] = M1;
            auto sr = FluidSystem::normalShock(M1, gamma23);
            m2Vec[i] = sr.M2;
            pRatio[i] = sr.p2_p1;
            rhoRatio[i] = sr.rho2_rho1;
            tRatio[i] = sr.T2_T1;
        }
        addCurve("M2", mVec, m2Vec);
        addCurve("p2/p1", mVec, pRatio);
        addCurve("T2/T1", mVec, tRatio);
        auto s2 = FluidSystem::normalShock(2.0, gamma23);
        oss << "Normal Shock Relations (gamma = " << gamma23 << ")\n";
        oss << "  At M1=2: M2=" << s2.M2 << ", p2/p1=" << s2.p2_p1
            << ", T2/T1=" << s2.T2_T1 << "\n";
        oss << "  p02/p01 = " << s2.p02_p01 << "\n";
        resultText = oss.str();
    }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##23")) { fluid.exportNormalShockCSV("normal_shocks.csv", gamma23, M1min23, M1max23, 200); }
}

void GuiApp::renderSim24_ObliqueShocks() {
    if (ImGui::CollapsingHeader("Theory: Oblique Shock Waves")) {
        ImGui::TextWrapped("Oblique shock: flow turns by deflection angle theta.\nShock angle beta found from theta-beta-M relation.\nWeak and strong shock solutions exist for theta < theta_max.");
    }
    static double M1_24 = 3.0, gamma24 = 1.4;
    ImGui::InputDouble("M1##24", &M1_24); ImGui::InputDouble("gamma##24", &gamma24);
    if (ImGui::Button("Compute##24")) {
        clearPlot();
        ostringstream oss;
        int N = 100;
        double thetaMax = FluidSystem::maxDeflectionAngle(M1_24, gamma24);
        vector<double> thetaVec(N), betaWeak(N), betaStrong(N);
        for (int i = 0; i < N; ++i) {
            double theta = 0.01 + thetaMax * 0.99 * i / (N - 1.0);
            thetaVec[i] = theta * 180.0 / M_PI;
            betaWeak[i] = FluidSystem::obliqueShockBeta(M1_24, theta, gamma24, true) * 180.0 / M_PI;
            betaStrong[i] = FluidSystem::obliqueShockBeta(M1_24, theta, gamma24, false) * 180.0 / M_PI;
        }
        addCurve("beta_weak(theta)", thetaVec, betaWeak);
        addCurve("beta_strong(theta)", thetaVec, betaStrong);
        oss << "Oblique Shock Waves (M1 = " << M1_24 << ", gamma = " << gamma24 << ")\n";
        oss << "  Max deflection angle = " << thetaMax * 180.0 / M_PI << " deg\n";
        resultText = oss.str();
    }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##24")) { fluid.exportObliqueShockCSV("oblique_shocks.csv", M1_24, gamma24, 100); }
}

void GuiApp::renderSim25_PrandtlMeyer() {
    if (ImGui::CollapsingHeader("Theory: Prandtl-Meyer Expansion")) {
        ImGui::TextWrapped("Isentropic expansion around a convex corner.\nPrandtl-Meyer function: nu(M) = sqrt((g+1)/(g-1)) * arctan(...) - arctan(...)\nM2 found from nu(M2) = nu(M1) + delta_theta.");
    }
    static double gamma25 = 1.4, Mmax25 = 5.0;
    ImGui::InputDouble("gamma##25", &gamma25);
    if (ImGui::Button("Compute##25")) {
        clearPlot();
        ostringstream oss;
        int N = 200;
        vector<double> mVec(N), nuVec(N);
        for (int i = 0; i < N; ++i) {
            double M = 1.001 + (Mmax25 - 1.001) * i / (N - 1.0);
            mVec[i] = M;
            nuVec[i] = FluidSystem::prandtlMeyerFunction(M, gamma25) * 180.0 / M_PI;
        }
        addCurve("nu(M) [deg]", mVec, nuVec);
        double nuMax = FluidSystem::prandtlMeyerFunction(Mmax25, gamma25) * 180.0 / M_PI;
        double dTheta = 10.0 * M_PI / 180.0;
        double M2 = FluidSystem::expansionFanM2(2.0, dTheta, gamma25);
        oss << "Prandtl-Meyer Expansion (gamma = " << gamma25 << ")\n";
        oss << "  nu(M=" << Mmax25 << ") = " << nuMax << " deg\n";
        oss << "  M1=2, delta=10deg => M2 = " << M2 << "\n";
        resultText = oss.str();
    }
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
    if (ImGui::Button("Compute##26")) {
        clearPlot();
        ostringstream oss;
        auto res = fluid.solveDeLavalNozzle(gamma26, p0_26, T0_26, FluidConstants::R_air, 0.01, 0.02, 200);
        addCurve("Mach(x)", res.x, res.M);
        addCurve("P(x) [Pa]", res.x, res.p);
        double Mexit = res.M.back();
        double Pexit = res.p.back();
        oss << "De Laval Nozzle\n";
        oss << "  P0 = " << p0_26 << " Pa, T0 = " << T0_26 << " K\n";
        oss << "  A_throat = 0.01 m^2, A_exit = 0.02 m^2\n";
        oss << "  M_exit = " << Mexit << "\n";
        oss << "  P_exit = " << Pexit << " Pa\n";
        oss << "  Thrust coeff = " << fluid.nozzleThrustCoefficient(gamma26, Pexit / p0_26, 0.02 / 0.01) << "\n";
        resultText = oss.str();
    }
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
    if (ImGui::Button("Compute##27")) {
        clearPlot();
        ostringstream oss;
        int N = 200;
        vector<double> mVec(N), flVec(N), tRatio(N), pRatio(N);
        for (int i = 0; i < N; ++i) {
            double M = 0.1 + (Mmax27 - 0.1) * i / (N - 1.0);
            mVec[i] = M;
            flVec[i] = FluidSystem::fannoFLstarOverD(M, gamma27);
            tRatio[i] = FluidSystem::fannoTemperatureRatio(M, gamma27);
            pRatio[i] = FluidSystem::fannoPressureRatio(M, gamma27);
        }
        addCurve("fL*/D", mVec, flVec);
        addCurve("T/T*", mVec, tRatio);
        addCurve("p/p*", mVec, pRatio);
        oss << "Fanno Flow (gamma = " << gamma27 << ")\n";
        oss << "  Adiabatic duct with friction\n";
        oss << "  At M=0.5: fL*/D = " << FluidSystem::fannoFLstarOverD(0.5, gamma27) << "\n";
        oss << "  At M=2: fL*/D = " << FluidSystem::fannoFLstarOverD(2.0, gamma27) << "\n";
        resultText = oss.str();
    }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##27")) { fluid.exportFannoFlowCSV("fanno_flow.csv", gamma27, Mmax27, 200); }
}

void GuiApp::renderSim28_RayleighFlow() {
    if (ImGui::CollapsingHeader("Theory: Rayleigh Flow")) {
        ImGui::TextWrapped("Frictionless flow with heat addition in a constant-area duct.\nHeat drives flow toward Mach 1.\nT/T*, p/p*, T0/T0*, p0/p0* as functions of M.");
    }
    static double gamma28 = 1.4, Mmax28 = 3.0;
    ImGui::InputDouble("gamma##28", &gamma28);
    if (ImGui::Button("Compute##28")) {
        clearPlot();
        ostringstream oss;
        int N = 200;
        vector<double> mVec(N), tRatio(N), pRatio(N), t0Ratio(N);
        for (int i = 0; i < N; ++i) {
            double M = 0.1 + (Mmax28 - 0.1) * i / (N - 1.0);
            mVec[i] = M;
            tRatio[i] = FluidSystem::rayleighTemperatureRatio(M, gamma28);
            pRatio[i] = FluidSystem::rayleighPressureRatio(M, gamma28);
            t0Ratio[i] = FluidSystem::rayleighStagnationTemperatureRatio(M, gamma28);
        }
        addCurve("T/T*", mVec, tRatio);
        addCurve("p/p*", mVec, pRatio);
        addCurve("T0/T0*", mVec, t0Ratio);
        oss << "Rayleigh Flow (gamma = " << gamma28 << ")\n";
        oss << "  Frictionless duct with heat addition\n";
        oss << "  At M=0.5: T/T* = " << FluidSystem::rayleighTemperatureRatio(0.5, gamma28) << "\n";
        oss << "  At M=2: p/p* = " << FluidSystem::rayleighPressureRatio(2.0, gamma28) << "\n";
        resultText = oss.str();
    }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##28")) { fluid.exportRayleighFlowCSV("rayleigh_flow.csv", gamma28, Mmax28, 200); }
}

void GuiApp::renderSim29_ReynoldsRegimes() {
    if (ImGui::CollapsingHeader("Theory: Reynolds Number Regimes")) {
        ImGui::TextWrapped("Re = rho*U*L/mu. Flow regimes: creeping (<1), laminar (<2300), transitional (2300-4000), turbulent (>4000).\nDrag coefficient for sphere, Darcy friction factor for pipes.");
    }
    if (ImGui::Button("Compute##29")) {
        clearPlot();
        ostringstream oss;
        int N = 200;
        vector<double> reVec(N), cdVec(N), fVec(N);
        for (int i = 0; i < N; ++i) {
            double Re = 1.0 + 1e5 * i / (N - 1.0);
            reVec[i] = Re;
            cdVec[i] = FluidSystem::dragCoefficientSphere(Re);
            fVec[i] = (Re < 2300) ? FluidSystem::frictionFactorLaminar(Re)
                                   : FluidSystem::frictionFactorTurbulentColebrook(Re, 0.001);
        }
        addCurve("Cd_sphere(Re)", reVec, cdVec);
        addCurve("f_pipe(Re)", reVec, fVec);
        double Re_calc = FluidSystem::reynoldsNum(fluid.rho, fluid.U, fluid.L, fluid.mu);
        oss << "Reynolds Number Regimes\n";
        oss << "  Current fluid Re = " << Re_calc << "\n";
        oss << "  Regime: " << FluidSystem::flowRegime(Re_calc) << "\n";
        oss << "  Pipe critical Re = " << FluidSystem::criticalReynoldsPipe() << "\n";
        oss << "  Plate critical Re = " << FluidSystem::criticalReynoldsPlate() << "\n";
        resultText = oss.str();
    }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##29")) { fluid.exportReynoldsRegimesCSV("reynolds_regimes.csv", 200); }
}

void GuiApp::renderSim30_KolmogorovCascade() {
    if (ImGui::CollapsingHeader("Theory: Kolmogorov Energy Cascade")) {
        ImGui::TextWrapped("Turbulent energy cascade: energy injected at large scales, cascades to small scales.\nInertial subrange: E(k) = C_K * epsilon^(2/3) * k^(-5/3)\nKolmogorov scales: eta = (nu^3/epsilon)^(1/4), tau_eta = (nu/epsilon)^(1/2).");
    }
    static double nu30 = 1e-6, eps30 = 1.0, CK = 1.5;
    ImGui::InputDouble("nu##30", &nu30); ImGui::InputDouble("epsilon##30", &eps30);
    if (ImGui::Button("Compute##30")) {
        clearPlot();
        ostringstream oss;
        int N = 200;
        vector<double> kVec(N), eVec(N);
        for (int i = 0; i < N; ++i) {
            kVec[i] = 1.0 + 1e4 * i / (N - 1.0);
            eVec[i] = FluidSystem::kolmogorovEnergySpectrum(CK, eps30, kVec[i]);
        }
        addCurve("E(k)", kVec, eVec);
        double eta = FluidSystem::kolmogorovLengthScale(nu30, eps30);
        double tau_eta = FluidSystem::kolmogorovTimeScale(nu30, eps30);
        double u_eta = FluidSystem::kolmogorovVelocityScale(nu30, eps30);
        oss << "Kolmogorov Energy Cascade\n";
        oss << "  nu = " << nu30 << " m^2/s, epsilon = " << eps30 << " m^2/s^3\n";
        oss << "  Kolmogorov length eta = " << eta << " m\n";
        oss << "  Kolmogorov time tau = " << tau_eta << " s\n";
        oss << "  Kolmogorov velocity = " << u_eta << " m/s\n";
        double u_rms = 1.0;
        oss << "  Integral scale L = " << FluidSystem::integralLengthScale(u_rms, eps30) << " m\n";
        oss << "  Taylor microscale = " << FluidSystem::taylorMicroscale(u_rms, nu30, eps30) << " m\n";
        resultText = oss.str();
    }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##30")) { fluid.exportKolmogorovCSV("kolmogorov.csv", nu30, eps30, CK, 200); }
}

void GuiApp::renderSim31_MixingLength() {
    if (ImGui::CollapsingHeader("Theory: Mixing Length / Log Law")) {
        ImGui::TextWrapped("Prandtl mixing length: l_m = kappa*y (von Karman const kappa~0.41).\nTurbulent viscosity: nu_t = l_m^2 * |du/dy|.\nLog law: u+ = (1/kappa)*ln(y+) + B,  B ≈ 5.0.\nViscous sublayer (y+ < 5): u+ = y+.");
    }
    static double Uinf31 = 10.0, nu31 = 1.5e-5;
    ImGui::InputDouble("U_inf (m/s)##31", &Uinf31); ImGui::InputDouble("nu##31", &nu31);
    if (ImGui::Button("Compute##31")) {
        clearPlot();
        ostringstream oss;
        double tauW = 0.5 * fluid.rho * Uinf31 * Uinf31 * 0.0592 / pow(fluid.rho * Uinf31 * 1.0 / fluid.mu, 0.2);
        double uTau = FluidSystem::frictionVelocity(tauW, fluid.rho);
        double deltaV = FluidSystem::viscousSublayerThickness(nu31, uTau);
        oss << "Mixing Length / Log Law\n";
        oss << "  U_inf = " << Uinf31 << " m/s, nu = " << nu31 << " m^2/s\n";
        oss << "  u_tau = " << uTau << " m/s\n";
        oss << "  Viscous sublayer = " << deltaV << " m\n";
        int N = 200;
        vector<double> ypVec(N), upVec(N);
        for (int i = 0; i < N; ++i) {
            double yp = 0.1 + 1000.0 * i / (N - 1.0);
            ypVec[i] = yp;
            if (yp < 5.0) upVec[i] = yp;
            else upVec[i] = FluidSystem::logLawVelocity(uTau, yp * nu31 / uTau, nu31, 0.41, 5.0) / uTau;
        }
        addCurve("u+(y+)", ypVec, upVec);
        resultText = oss.str();
    }
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
    if (ImGui::Button("Compute##32")) {
        clearPlot();
        ostringstream oss;
        auto profile = FluidSystem::solveBlasius(10.0, 200);
        addCurve("f'(eta) = u/U", profile.eta, profile.fp);
        auto blGrowth = fluid.blasiusBLGrowth(Uinf32, nu32, xMax32, 200);
        oss << "Blasius Boundary Layer\n";
        oss << "  U_inf = " << Uinf32 << " m/s, nu = " << nu32 << " m^2/s\n";
        oss << "  Re_x at x=" << xMax32 << ": " << Uinf32 * xMax32 / nu32 << "\n";
        oss << "  delta(" << xMax32 << ") = " << blGrowth.delta.back() << " m\n";
        oss << "  delta*(" << xMax32 << ") = " << blGrowth.delta_star.back() << " m\n";
        oss << "  theta(" << xMax32 << ") = " << blGrowth.theta.back() << " m\n";
        oss << "  Cf(" << xMax32 << ") = " << blGrowth.cf.back() << "\n";
        resultText = oss.str();
    }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##32")) { fluid.exportBlasiusCSV("blasius.csv", Uinf32, nu32, xMax32, 200); }
}

void GuiApp::renderSim33_DragSphere() {
    if (ImGui::CollapsingHeader("Theory: Drag on a Sphere")) {
        ImGui::TextWrapped("Cd(Re) correlation: Stokes (24/Re) at low Re, ~0.44 for 1e3<Re<2e5,\ndrag crisis at Re~2e5. Terminal velocity from force balance.\nOseen correction: Cd = 24/Re * (1 + 3Re/16).");
    }
    static double rhoP33 = 2700.0, d33 = 0.01;
    ImGui::InputDouble("rho_particle##33", &rhoP33); ImGui::InputDouble("d (m)##33", &d33);
    if (ImGui::Button("Compute##33")) {
        clearPlot();
        ostringstream oss;
        int N = 200;
        vector<double> reVec(N), cdVec(N);
        for (int i = 0; i < N; ++i) {
            reVec[i] = 0.1 + 1e6 * i / (N - 1.0);
            cdVec[i] = FluidSystem::cdSphereEmpirical(reVec[i]);
        }
        addCurve("Cd(Re)", reVec, cdVec);
        double vT = FluidSystem::terminalVelocitySphere(rhoP33, fluid.rho, d33, fluid.mu, FluidConstants::g);
        double Re_t = fluid.rho * vT * d33 / fluid.mu;
        double Cd_t = FluidSystem::cdSphereEmpirical(Re_t);
        double A = M_PI * d33 * d33 / 4.0;
        double Fd = FluidSystem::dragForce(Cd_t, fluid.rho, vT, A);
        oss << "Drag on a Sphere\n";
        oss << "  rho_p = " << rhoP33 << ", d = " << d33 << " m\n";
        oss << "  Terminal velocity = " << vT << " m/s\n";
        oss << "  Re_terminal = " << Re_t << "\n";
        oss << "  Cd at terminal = " << Cd_t << "\n";
        oss << "  Drag force = " << Fd << " N\n";
        resultText = oss.str();
    }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##33")) { fluid.exportDragSphereCSV("drag_sphere.csv", fluid.rho, rhoP33, d33, fluid.mu, FluidConstants::g, 200); }
}

void GuiApp::renderSim34_PipeFlowMoody() {
    if (ImGui::CollapsingHeader("Theory: Pipe Flow (Moody Diagram)")) {
        ImGui::TextWrapped("Darcy-Weisbach: hL = f*L/D * V^2/(2g).\nLaminar: f = 64/Re.\nTurbulent: Colebrook equation: 1/sqrt(f) = -2*log10(eps/D/3.7 + 2.51/(Re*sqrt(f))).\nMoody diagram: f vs Re for various eps/D.");
    }
    if (ImGui::Button("Compute##34")) {
        clearPlot();
        ostringstream oss;
        int N = 200;
        double epsValues[] = {0, 0.0001, 0.001, 0.01, 0.05};
        const char* epsLabels[] = {"f (smooth)", "f (e/D=0.0001)", "f (e/D=0.001)", "f (e/D=0.01)", "f (e/D=0.05)"};
        for (int e = 0; e < 5; ++e) {
            vector<double> reVec(N), fVec(N);
            for (int i = 0; i < N; ++i) {
                double Re = 500.0 + (1e7 - 500.0) * i / (N - 1.0);
                reVec[i] = Re;
                fVec[i] = FluidSystem::darcyFrictionFactor(Re, epsValues[e]);
            }
            addCurve(epsLabels[e], reVec, fVec);
        }
        oss << "Moody Diagram\n";
        oss << "  Laminar: f = 64/Re\n";
        oss << "  Turbulent: Colebrook equation\n";
        oss << "  f(Re=3000, smooth) = " << FluidSystem::darcyFrictionFactor(3000, 0) << "\n";
        oss << "  f(Re=1e5, e/D=0.001) = " << FluidSystem::darcyFrictionFactor(1e5, 0.001) << "\n";
        resultText = oss.str();
    }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##34")) { fluid.exportMoodyDiagramCSV("moody.csv", 200, 8); }
}

void GuiApp::renderSim35_HeatConvection() {
    if (ImGui::CollapsingHeader("Theory: Forced Convection Heat Transfer")) {
        ImGui::TextWrapped("Nusselt number correlations:\n  Flat plate laminar: Nu = 0.332*Re^0.5*Pr^(1/3)\n  Turbulent: Nu = 0.0296*Re^0.8*Pr^(1/3)\n  Pipe (Dittus-Boelter): Nu = 0.023*Re^0.8*Pr^0.4\n  h = Nu*k/L");
    }
    static double Pr35 = FluidConstants::Pr_water, Remax35 = 1e6;
    ImGui::InputDouble("Pr##35", &Pr35);
    if (ImGui::Button("Compute##35")) {
        clearPlot();
        ostringstream oss;
        int N = 200;
        vector<double> reVec(N), nuLam(N), nuTurb(N), nuCyl(N);
        for (int i = 0; i < N; ++i) {
            double Re = 100.0 + (Remax35 - 100.0) * i / (N - 1.0);
            reVec[i] = Re;
            nuLam[i] = FluidSystem::nusseltFlatPlate(Re, Pr35, false);
            nuTurb[i] = FluidSystem::nusseltFlatPlate(Re, Pr35, true);
            nuCyl[i] = FluidSystem::nusseltCylinder(Re, Pr35);
        }
        addCurve("Nu_lam plate", reVec, nuLam);
        addCurve("Nu_turb plate", reVec, nuTurb);
        addCurve("Nu cylinder", reVec, nuCyl);
        double h_conv = FluidSystem::convectiveHeatTransfer(
            FluidSystem::nusseltFlatPlate(1e5, Pr35, false), FluidConstants::k_water, 0.1);
        oss << "Forced Convection Heat Transfer\n";
        oss << "  Pr = " << Pr35 << "\n";
        oss << "  Nu_plate_lam(Re=1e5) = " << FluidSystem::nusseltFlatPlate(1e5, Pr35, false) << "\n";
        oss << "  Nu_plate_turb(Re=1e5) = " << FluidSystem::nusseltFlatPlate(1e5, Pr35, true) << "\n";
        oss << "  h at Re=1e5, L=0.1m = " << h_conv << " W/(m^2*K)\n";
        resultText = oss.str();
    }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##35")) { fluid.exportHeatConvectionCSV("heat_convection.csv", Pr35, Remax35, 200); }
}

void GuiApp::renderSim36_MassDiffusion() {
    if (ImGui::CollapsingHeader("Theory: Mass Diffusion (Fick's Law)")) {
        ImGui::TextWrapped("Fick's first law: J = -D * dC/dx.\nFick's second law: dC/dt = D * d^2C/dx^2.\n1D Gaussian solution: C(x,t) = C0/sqrt(4*pi*D*t) * exp(-x^2/(4Dt)).\nSchmidt number: Sc = nu/D. Sherwood number: Sh = 0.332*Re^0.5*Sc^(1/3).");
    }
    static double D36 = 1e-9, C0_36 = 1.0;
    ImGui::InputDouble("D (m^2/s)##36", &D36); ImGui::InputDouble("C0##36", &C0_36);
    if (ImGui::Button("Compute##36")) {
        clearPlot();
        ostringstream oss;
        int N = 200;
        double xMax = 0.01;
        double times[] = {1.0, 10.0, 100.0};
        const char* labels[] = {"C(x, t=1s)", "C(x, t=10s)", "C(x, t=100s)"};
        for (int t = 0; t < 3; ++t) {
            vector<double> xVec(N), cVec(N);
            for (int i = 0; i < N; ++i) {
                xVec[i] = -xMax + 2.0 * xMax * i / (N - 1.0);
                cVec[i] = FluidSystem::diffusionSolution1D(C0_36, xVec[i], D36, times[t]);
            }
            addCurve(labels[t], xVec, cVec);
        }
        double Sc = FluidSystem::schmidtNumber(FluidConstants::nu_water, D36);
        double Sh = FluidSystem::sherwoodNumber(1000, Sc);
        oss << "Mass Diffusion (Fick's Law)\n";
        oss << "  D = " << D36 << " m^2/s, C0 = " << C0_36 << "\n";
        oss << "  Flux at dC/dx=1: J = " << FluidSystem::fickFlux(D36, 1.0) << "\n";
        oss << "  Schmidt number = " << Sc << "\n";
        oss << "  Sherwood (Re=1000) = " << Sh << "\n";
        oss << "  Mass transfer coeff = " << FluidSystem::massTransferCoefficient(Sh, D36, 0.01) << " m/s\n";
        resultText = oss.str();
    }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##36")) { fluid.exportDiffusionCSV("diffusion.csv", C0_36, D36, 0.01, 200, 10, 100.0); }
}

void GuiApp::renderSim37_MHD() {
    if (ImGui::CollapsingHeader("Theory: Magnetohydrodynamics (MHD)")) {
        ImGui::TextWrapped("MHD couples fluid dynamics with Maxwell's equations.\nAlfven speed: v_A = B/sqrt(mu0*rho).\nMagnetic pressure: P_B = B^2/(2*mu0).\nHartmann number: Ha = B*L*sqrt(sigma/mu).\nMagnetic Reynolds number: Rm = U*L/eta.");
    }
    static double B37 = 1e-4;
    ImGui::InputDouble("B (T)##37", &B37);
    if (ImGui::Button("Compute##37")) {
        clearPlot();
        ostringstream oss;
        auto st = fluid.solveMHD1D(1.0, 0.125, 1.0, 0.1, B37, 1.0, B37, -1.0, 0, 1, 400, 0.2);
        addCurve("rho(x)", st.x, st.rhoField);
        addCurve("vx(x)", st.x, st.vx);
        addCurve("By(x)", st.x, st.By);
        double vA = FluidSystem::alfvenSpeed(B37, fluid.rho, FluidConstants::mu_0);
        double Pm = FluidSystem::magneticPressure(B37, FluidConstants::mu_0);
        double Ha = FluidSystem::hartmannNumber(B37, fluid.L, 1.0, fluid.mu);
        oss << "Magnetohydrodynamics\n";
        oss << "  B = " << B37 << " T\n";
        oss << "  Alfven speed = " << vA << " m/s\n";
        oss << "  Magnetic pressure = " << Pm << " Pa\n";
        oss << "  Hartmann number = " << Ha << "\n";
        oss << "  Rm = " << FluidSystem::magneticReynoldsNumber(fluid.U, fluid.L, 1.0) << "\n";
        resultText = oss.str();
    }
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
    if (ImGui::Button("Compute##38")) {
        clearPlot();
        ostringstream oss;
        double vA = FluidSystem::alfvenWaveGroupVelocity(B38, rho38, FluidConstants::mu_0);
        double k0 = 10.0;
        double omega0 = FluidSystem::alfvenWaveDispersion(k0, B38, rho38, FluidConstants::mu_0);
        oss << "Alfven Waves\n";
        oss << "  B0 = " << B38 << " T, rho = " << rho38 << " kg/m^3\n";
        oss << "  Alfven speed = " << vA << " m/s\n";
        oss << "  omega(k=" << k0 << ") = " << omega0 << " rad/s\n";
        int N = 200;
        vector<double> xVec(N), bVec(N);
        for (int i = 0; i < N; ++i) {
            xVec[i] = 2.0 * M_PI / k0 * 3.0 * i / (N - 1.0);
            bVec[i] = FluidSystem::alfvenWaveMagneticField(B38, dB38, k0, omega0, xVec[i], 0.0);
        }
        addCurve("B(x, t=0)", xVec, bVec);
        vector<double> kVec(N), omVec(N);
        for (int i = 0; i < N; ++i) {
            kVec[i] = 0.1 + 100.0 * i / (N - 1.0);
            omVec[i] = FluidSystem::alfvenWaveDispersion(kVec[i], B38, rho38, FluidConstants::mu_0);
        }
        addCurve("omega(k)", kVec, omVec);
        resultText = oss.str();
    }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##38")) { fluid.exportAlfvenWavesCSV("alfven_waves.csv", B38, rho38, FluidConstants::mu_0, dB38, 10, 100); }
}

void GuiApp::renderSim39_PlasmaBeta() {
    if (ImGui::CollapsingHeader("Theory: Plasma Beta & Parameters")) {
        ImGui::TextWrapped("Plasma beta: beta = 2*mu0*p / B^2.\nbeta > 1: pressure-dominated. beta < 1: magnetically-dominated.\nDebye length: lambda_D = sqrt(eps0*kT/(n*q^2)).\nPlasma frequency: omega_p = sqrt(n*q^2/(m*eps0)). Gyroradius: r_g = m*v_perp/(q*B).");
    }
    static double T39 = 1e6, n39 = 1e18, Bmin39 = 1e-5, Bmax39 = 1e-2;
    ImGui::InputDouble("T (K)##39", &T39); ImGui::InputDouble("n (m^-3)##39", &n39);
    if (ImGui::Button("Compute##39")) {
        clearPlot();
        ostringstream oss;
        double p_th = n39 * FluidConstants::k_B * T39;
        int N = 200;
        vector<double> bVec(N), betaVec(N);
        for (int i = 0; i < N; ++i) {
            bVec[i] = Bmin39 + (Bmax39 - Bmin39) * i / (N - 1.0);
            betaVec[i] = FluidSystem::plasmaBeta(p_th, bVec[i], FluidConstants::mu_0);
        }
        addCurve("beta(B)", bVec, betaVec);
        double lD = FluidSystem::debyeLength(T39, n39, FluidConstants::e_charge, FluidConstants::epsilon_0);
        double wp = FluidSystem::plasmaFrequency(n39, FluidConstants::e_charge, FluidConstants::m_electron, FluidConstants::epsilon_0);
        double vth = sqrt(2.0 * FluidConstants::k_B * T39 / FluidConstants::m_proton);
        double rg = FluidSystem::gyroRadius(FluidConstants::m_proton, vth, FluidConstants::e_charge, Bmin39);
        oss << "Plasma Parameters\n";
        oss << "  T = " << T39 << " K, n = " << n39 << " m^-3\n";
        oss << "  Thermal pressure = " << p_th << " Pa\n";
        oss << "  Debye length = " << lD << " m\n";
        oss << "  Plasma frequency = " << wp << " rad/s\n";
        oss << "  Gyroradius (proton) = " << rg << " m\n";
        resultText = oss.str();
    }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##39")) { fluid.exportPlasmaBetaCSV("plasma_beta.csv", T39, n39, Bmin39, Bmax39, 200); }
}

void GuiApp::renderSim40_AccretionDisk() {
    if (ImGui::CollapsingHeader("Theory: Accretion Disk Model")) {
        ImGui::TextWrapped("Keplerian disk: v_K = sqrt(GM/r), Omega_K = sqrt(GM/r^3).\nDisk temperature: T ~ (3GMdot/(8*pi*r^3*sigma_SB))^(1/4).\nEddington luminosity: L_Edd = 4*pi*G*M*m_p*c / sigma_T.");
    }
    static double M40 = 2e30, Mdot40 = 1e18, rMin40 = 1e7, rMax40 = 1e10;
    ImGui::InputDouble("M (kg)##40", &M40); ImGui::InputDouble("Mdot (kg/s)##40", &Mdot40);
    if (ImGui::Button("Compute##40")) {
        clearPlot();
        ostringstream oss;
        double G40 = 6.674e-11;
        int N = 200;
        vector<double> rVec(N), tVec(N), vVec(N);
        for (int i = 0; i < N; ++i) {
            rVec[i] = rMin40 + (rMax40 - rMin40) * i / (N - 1.0);
            vVec[i] = FluidSystem::keplerianVelocity(M40, rVec[i], G40);
            tVec[i] = FluidSystem::diskTemperature(Mdot40, M40, rVec[i], G40, FluidConstants::sigma_SB);
        }
        addCurve("v_K(r)", rVec, vVec);
        addCurve("T(r)", rVec, tVec);
        double sigmaT = 6.652e-29;
        double LEdd = FluidSystem::eddingtonLuminosity(M40, FluidConstants::c_light, sigmaT,
                                                        FluidConstants::m_proton, G40);
        oss << "Accretion Disk\n";
        oss << "  M = " << M40 << " kg, Mdot = " << Mdot40 << " kg/s\n";
        oss << "  T(r_min) = " << tVec.front() << " K\n";
        oss << "  v_K(r_min) = " << vVec.front() << " m/s\n";
        oss << "  L_Eddington = " << LEdd << " W\n";
        resultText = oss.str();
    }
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
    if (ImGui::Button("Compute##41")) {
        clearPlot();
        ostringstream oss;
        int N = 200;
        double lambdaJ = FluidSystem::jeansLength(cs41, G41, rho41);
        double kJ = FluidSystem::jeansWavenumber(cs41, G41, rho41);
        double MJ = FluidSystem::jeansMass(cs41, G41, rho41);
        double tff = FluidSystem::freefall_time(G41, rho41);
        vector<double> kVec(N), sigmaVec(N);
        for (int i = 0; i < N; ++i) {
            kVec[i] = 0.01 * kJ + 2.0 * kJ * i / (N - 1.0);
            sigmaVec[i] = FluidSystem::jeansGrowthRate(kVec[i], cs41, G41, rho41);
        }
        addCurve("sigma(k)", kVec, sigmaVec);
        oss << "Jeans Instability\n";
        oss << "  c_s = " << cs41 << " m/s, rho = " << rho41 << " kg/m^3\n";
        oss << "  Jeans length = " << lambdaJ << " m\n";
        oss << "  Jeans mass = " << MJ << " kg\n";
        oss << "  Jeans wavenumber = " << kJ << " 1/m\n";
        oss << "  Free-fall time = " << tff << " s\n";
        resultText = oss.str();
    }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##41")) { fluid.exportJeansInstabilityCSV("jeans.csv", cs41, G41, rho41, 200); }
}

void GuiApp::renderSim42_BondiAccretion() {
    if (ImGui::CollapsingHeader("Theory: Bondi Accretion")) {
        ImGui::TextWrapped("Spherically symmetric accretion onto a point mass.\nBondi radius: r_B = GM/c_s^2.\nAccretion rate: Mdot ~ 4*pi*rho_inf*r_B^2*c_s.\nTransonic solution: subsonic at infinity, supersonic near center.");
    }
    static double M42 = 2e30, cs42 = 1e4, rho42 = 1e-20, gamma42 = 5.0/3.0;
    ImGui::InputDouble("M (kg)##42", &M42); ImGui::InputDouble("c_s##42", &cs42);
    if (ImGui::Button("Compute##42")) {
        clearPlot();
        ostringstream oss;
        double G42 = 6.674e-11;
        double rB = FluidSystem::bondiRadius(M42, cs42, G42);
        double Mdot = FluidSystem::bondiAccretionRate(rho42, M42, cs42, G42);
        int N = 200;
        vector<double> rVec(N), machVec(N);
        for (int i = 0; i < N; ++i) {
            rVec[i] = 0.1 * rB + 10.0 * rB * i / (N - 1.0);
            machVec[i] = FluidSystem::bondiMachProfile(rVec[i], rB, gamma42);
        }
        addCurve("Mach(r)", rVec, machVec);
        oss << "Bondi Accretion\n";
        oss << "  M = " << M42 << " kg, c_s = " << cs42 << " m/s\n";
        oss << "  Bondi radius = " << rB << " m\n";
        oss << "  Accretion rate = " << Mdot << " kg/s\n";
        resultText = oss.str();
    }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##42")) { fluid.exportBondiAccretionCSV("bondi.csv", M42, cs42, rho42, 6.674e-11, gamma42, 200); }
}

void GuiApp::renderSim43_ViscousDisk() {
    if (ImGui::CollapsingHeader("Theory: Viscous Disk (Shakura-Sunyaev)")) {
        ImGui::TextWrapped("Shakura-Sunyaev alpha prescription: nu = alpha * c_s * H.\nDisk scale height: H = c_s / Omega_K.\nViscous timescale: t_visc = R^2/nu.\nSurface density evolution: diffusion equation in radius.");
    }
    static double alpha43 = 0.1, M43 = 2e30, Mdot43 = 1e18;
    ImGui::InputDouble("alpha_SS##43", &alpha43); ImGui::InputDouble("M (kg)##43", &M43);
    if (ImGui::Button("Compute##43")) {
        clearPlot();
        ostringstream oss;
        double G43 = 6.674e-11;
        int N = 200;
        double rMin43 = 1e7, rMax43 = 1e10;
        vector<double> rVec(N), hVec(N), nuVec(N);
        for (int i = 0; i < N; ++i) {
            double r = rMin43 + (rMax43 - rMin43) * i / (N - 1.0);
            rVec[i] = r;
            double Omega = FluidSystem::keplerianAngularVelocity(M43, r, G43);
            double cs_local = 1e4;
            double H = FluidSystem::diskScaleHeight(cs_local, Omega);
            hVec[i] = H;
            nuVec[i] = FluidSystem::shakuraSunyaevViscosity(alpha43, cs_local, H);
        }
        addCurve("H(r)", rVec, hVec);
        addCurve("nu(r)", rVec, nuVec);
        double Omega0 = FluidSystem::keplerianAngularVelocity(M43, rMin43, G43);
        double H0 = FluidSystem::diskScaleHeight(1e4, Omega0);
        double tVisc = FluidSystem::viscousTimescale(rMin43, FluidSystem::shakuraSunyaevViscosity(alpha43, 1e4, H0));
        oss << "Viscous Disk (Shakura-Sunyaev)\n";
        oss << "  alpha = " << alpha43 << ", M = " << M43 << " kg\n";
        oss << "  H(r_min) = " << hVec.front() << " m\n";
        oss << "  nu(r_min) = " << nuVec.front() << " m^2/s\n";
        oss << "  Viscous timescale = " << tVisc << " s\n";
        resultText = oss.str();
    }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##43")) { fluid.exportViscousDiskCSV("viscous_disk.csv", alpha43, M43, Mdot43, 1e7, 1e10, 200); }
}

void GuiApp::renderSim44_RelativisticFluid() {
    if (ImGui::CollapsingHeader("Theory: Relativistic Fluid Dynamics")) {
        ImGui::TextWrapped("Relativistic enthalpy: w = (rho*c^2 + e + P)/rho.\nLorentz factor: Gamma = 1/sqrt(1 - v^2/c^2).\nRelativistic sound speed < c.\nTaub adiabat for relativistic shocks.");
    }
    static double gamma44 = 5.0/3.0;
    ImGui::InputDouble("gamma_EOS##44", &gamma44);
    if (ImGui::Button("Compute##44")) {
        clearPlot();
        ostringstream oss;
        double c_val = FluidConstants::c_light;
        int N = 200;
        vector<double> vVec(N), gammaVec(N), csVec(N);
        for (int i = 0; i < N; ++i) {
            double v = 0.999 * c_val * i / (N - 1.0);
            vVec[i] = v / c_val;
            gammaVec[i] = FluidSystem::lorentzFactor(v, c_val);
            double rho_r = 1.0, p_r = 0.1, eps = p_r / ((gamma44 - 1.0) * rho_r);
            csVec[i] = FluidSystem::relativisticSoundSpeed(gamma44, p_r, rho_r, eps, c_val * c_val);
        }
        addCurve("Gamma(v/c)", vVec, gammaVec);
        double rho0 = 1.0, p0_r = 1.0, eps0 = p0_r / ((gamma44 - 1) * rho0);
        double h_rel = FluidSystem::relativisticEnthalpy(rho0, p0_r, eps0, c_val * c_val);
        oss << "Relativistic Fluid Dynamics\n";
        oss << "  gamma_eos = " << gamma44 << "\n";
        oss << "  Lorentz(0.9c) = " << FluidSystem::lorentzFactor(0.9 * c_val, c_val) << "\n";
        oss << "  Lorentz(0.99c) = " << FluidSystem::lorentzFactor(0.99 * c_val, c_val) << "\n";
        oss << "  Enthalpy (rho=1,p=1) = " << h_rel << "\n";
        resultText = oss.str();
    }
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
    if (ImGui::Button("Compute##45")) {
        clearPlot();
        ostringstream oss;
        auto st = fluid.solveLBM_D2Q9(nx45, ny45, tau45, uLid45, iter45);
        vector<double> yVec(ny45), uCenter(ny45);
        int midX = nx45 / 2;
        for (int j = 0; j < ny45; ++j) {
            yVec[j] = (double)j / (ny45 - 1.0);
            uCenter[j] = st.ux[j * nx45 + midX];
        }
        addCurve("ux(y) centerline", yVec, uCenter);
        double uMax_lbm = *max_element(uCenter.begin(), uCenter.end());
        double nu_lbm = (1.0 / 3.0) * (tau45 - 0.5);
        oss << "Lattice Boltzmann D2Q9\n";
        oss << "  Grid = " << nx45 << "x" << ny45 << ", tau = " << tau45 << "\n";
        oss << "  u_lid = " << uLid45 << ", iterations = " << iter45 << "\n";
        oss << "  LBM viscosity = " << nu_lbm << "\n";
        oss << "  Max centerline u = " << uMax_lbm << "\n";
        resultText = oss.str();
    }
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
    if (ImGui::Button("Compute##46")) {
        clearPlot();
        ostringstream oss;
        auto parts = fluid.solveSPH_DamBreak(nPart46, h46, dt46, steps46, 20.0, 1000.0);
        vector<double> xVec(parts.size()), yVec(parts.size());
        for (size_t i = 0; i < parts.size(); ++i) {
            xVec[i] = parts[i].x;
            yVec[i] = parts[i].y;
        }
        addCurve("Particles (x,y)", xVec, yVec);
        oss << "SPH Dam Break\n";
        oss << "  Particles = " << nPart46 << ", steps = " << steps46 << "\n";
        oss << "  h_smooth = " << h46 << ", dt = " << dt46 << "\n";
        if (!parts.empty()) {
            double xMin_s = parts[0].x, xMax_s = parts[0].x;
            for (auto& p : parts) { xMin_s = min(xMin_s, p.x); xMax_s = max(xMax_s, p.x); }
            oss << "  x range: [" << xMin_s << ", " << xMax_s << "]\n";
        }
        resultText = oss.str();
    }
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
    if (ImGui::Button("Compute##47")) {
        clearPlot();
        ostringstream oss;
        std::vector<FluidSystem::VortexState> vorts(nVort47);
        for (int i = 0; i < nVort47; ++i) {
            vorts[i] = {(double)i / (nVort47 - 1) * 2.0 - 1.0, 0.0, 0.1};
        }
        auto evolved = fluid.evolvePointVortices(vorts, dt47, steps47);
        vector<double> xVec(evolved.size()), yVec(evolved.size());
        for (size_t i = 0; i < evolved.size(); ++i) {
            xVec[i] = evolved[i].x;
            yVec[i] = evolved[i].y;
        }
        addCurve("Vortex positions", xVec, yVec);
        oss << "Vortex Methods\n";
        oss << "  N = " << nVort47 << " vortices, dt = " << dt47 << "\n";
        oss << "  Steps = " << steps47 << ", epsilon = " << eps47 << "\n";
        oss << "  Final positions plotted\n";
        resultText = oss.str();
    }
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
    if (ImGui::Button("Compute##48")) {
        clearPlot();
        ostringstream oss;
        int N = 200;
        vector<double> xVec(N), alphaVec(N), phiVec(N);
        for (int i = 0; i < N; ++i) {
            double x_q = 0.001 + 0.999 * i / (N - 1.0);
            xVec[i] = x_q;
            double S = FluidSystem::slipRatio(rhoL48, rhoG48, x_q);
            alphaVec[i] = x_q / (x_q + (1.0 - x_q) * rhoG48 / rhoL48 * S);
            phiVec[i] = FluidSystem::twoPhaseMultiplier(x_q, rhoL48, rhoG48, muL48, muG48);
        }
        addCurve("alpha(x)", xVec, alphaVec);
        addCurve("phi^2(x)", xVec, phiVec);
        double Um = FluidSystem::mixtureVelocity(0.01, 0.1, 0.001);
        double Ud = FluidSystem::driftFluxVelocity(1.2, Um, 0.35);
        oss << "Multiphase Flow\n";
        oss << "  rho_L = " << rhoL48 << ", rho_G = " << rhoG48 << "\n";
        oss << "  Mixture velocity (Qg=0.01,Ql=0.1,A=0.001) = " << Um << " m/s\n";
        oss << "  Drift flux velocity = " << Ud << " m/s\n";
        resultText = oss.str();
    }
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
    if (ImGui::Button("Compute##49")) {
        clearPlot();
        ostringstream oss;
        int N = 200;
        vector<double> gdVec(N), muVec(N);
        for (int i = 0; i < N; ++i) {
            double gd = 0.01 + 1000.0 * i / (N - 1.0);
            gdVec[i] = gd;
            if (model49 == 0)
                muVec[i] = FluidSystem::powerLawViscosity(p1_49, p2_49, gd);
            else if (model49 == 1)
                muVec[i] = FluidSystem::binghamViscosity(p1_49, p2_49, gd);
            else
                muVec[i] = FluidSystem::carreauViscosity(p1_49, p2_49, p3_49, 0.5, gd);
        }
        const char* modelNames[] = {"Power Law", "Bingham", "Carreau"};
        addCurve("mu(gammaDot)", gdVec, muVec);
        oss << "Non-Newtonian Fluid: " << modelNames[model49] << "\n";
        if (model49 == 0) {
            oss << "  K = " << p1_49 << ", n = " << p2_49 << "\n";
            oss << "  mu(100 s^-1) = " << FluidSystem::powerLawViscosity(p1_49, p2_49, 100) << " Pa*s\n";
        } else if (model49 == 1) {
            oss << "  tau_y = " << p1_49 << ", mu_p = " << p2_49 << "\n";
            double Rplug = FluidSystem::binghamPlugRadius(p1_49, 100);
            oss << "  Plug radius (dpdx=100) = " << Rplug << " m\n";
        } else {
            oss << "  mu0 = " << p1_49 << ", muInf = " << p2_49 << ", lambda = " << p3_49 << "\n";
        }
        resultText = oss.str();
    }
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
    if (ImGui::Button("Compute##50")) {
        clearPlot();
        ostringstream oss;
        double Dh = FluidSystem::hydraulicDiameter(w50, h50);
        double Rhyd = FluidSystem::microchannelResistance(fluid.mu, L50, w50, h50);
        double A_ch = w50 * h50;
        int N = 200;
        vector<double> qVec(N), reVec(N), caVec(N);
        for (int i = 0; i < N; ++i) {
            double Q = 1e-12 * (i + 1);
            qVec[i] = Q;
            double Uavg = Q / A_ch;
            reVec[i] = fluid.rho * Uavg * Dh / fluid.mu;
            caVec[i] = FluidSystem::capillaryNumber(fluid.mu, Uavg, FluidConstants::sigma_water);
        }
        addCurve("Re(Q)", qVec, reVec);
        addCurve("Ca(Q)", qVec, caVec);
        double Kn = FluidSystem::knudsenNumber(68e-9, Dh);
        double tMix = FluidSystem::mixingTimeDiffusion(w50, 1e-9);
        oss << "Microfluidics\n";
        oss << "  w = " << w50 * 1e6 << " um, h = " << h50 * 1e6 << " um\n";
        oss << "  D_h = " << Dh * 1e6 << " um\n";
        oss << "  R_hyd = " << Rhyd << " Pa*s/m^3\n";
        oss << "  Kn (air, mfp=68nm) = " << Kn << "\n";
        oss << "  Mixing time (D=1e-9) = " << tMix << " s\n";
        oss << "  Electro-osmotic (eps=80*eps0, zeta=50mV, E=1e4) = "
            << FluidSystem::electro_osmoticVelocity(80 * FluidConstants::epsilon_0, 0.05, 1e4, fluid.mu)
            << " m/s\n";
        resultText = oss.str();
    }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV##50")) { fluid.exportMicrofluidicsCSV("microfluidics.csv", fluid.mu, FluidConstants::sigma_water, w50, h50, L50, 200); }
}
