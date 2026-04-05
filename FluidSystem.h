#ifndef FLUID_SYSTEM_H
#define FLUID_SYSTEM_H

#include <string>
#include <vector>
#include <complex>
#include <array>
#include <tuple>
#include <functional>
#include <utility>

// ════════════════════════════════════════════════════════════════════════
//  FluidSystem — physics engine for FluidDynamicsCore (50 simulations)
// ════════════════════════════════════════════════════════════════════════

class FluidSystem {
public:
    // ── Constructor ─────────────────────────────────────────────────────
    FluidSystem(const std::string& fluidName, double density, double viscosity,
                double characteristicLength, double characteristicVelocity);

    std::string name;
    double rho;       // kg/m^3
    double mu;        // Pa·s (dynamic viscosity)
    double L;         // characteristic length (m)
    double U;         // characteristic velocity (m/s)

    // ── Utility ─────────────────────────────────────────────────────────
    double reynoldsNumber() const;
    double kinematicViscosity() const;

    // ════════════════════════════════════════════════════════════════════
    //  RESULT STRUCTS
    // ════════════════════════════════════════════════════════════════════

    struct FlowField2D {
        int nx, ny;
        double dx, dy;
        std::vector<double> u;   // x-velocity
        std::vector<double> v;   // y-velocity
        std::vector<double> p;   // pressure
        std::vector<double> rhoField; // density
    };

    struct ShockResult {
        double M2;       // downstream Mach
        double p2_p1;    // pressure ratio
        double rho2_rho1;// density ratio
        double T2_T1;    // temperature ratio
        double p02_p01;  // stagnation pressure ratio
    };

    struct BoundaryLayerResult {
        std::vector<double> x;
        std::vector<double> delta;    // BL thickness
        std::vector<double> delta_star; // displacement thickness
        std::vector<double> theta;    // momentum thickness
        std::vector<double> cf;       // skin friction coefficient
    };

    struct VortexState {
        double x, y;        // position
        double gamma;       // circulation
    };

    struct NozzleResult {
        std::vector<double> x;
        std::vector<double> A;
        std::vector<double> M;
        std::vector<double> p;
        std::vector<double> T;
        std::vector<double> rhoField;
    };

    struct WaveResult {
        std::vector<double> x;
        std::vector<double> eta;  // surface elevation
        std::vector<double> u;    // velocity
    };

    struct MHDState {
        std::vector<double> x;
        std::vector<double> rhoField;
        std::vector<double> vx, vy, vz;
        std::vector<double> Bx, By, Bz;
        std::vector<double> p;
    };

    // ════════════════════════════════════════════════════════════════════
    //  1. HYDROSTATICS
    // ════════════════════════════════════════════════════════════════════
    static double hydrostaticPressure(double rho, double g, double h, double p0);
    static double buoyantForce(double rhoFluid, double g, double Vsub);
    static double submergedFraction(double rhoObj, double rhoFluid);
    static std::vector<double> pressureProfile(double rho, double g, double p0,
                                                int numPoints, double hMax);
    void exportHydrostaticsCSV(const std::string& filename, double g, double p0,
                               int numPoints, double hMax);

    // ════════════════════════════════════════════════════════════════════
    //  2. BERNOULLI EQUATION
    // ════════════════════════════════════════════════════════════════════
    static double bernoulliPressure(double p1, double rho, double v1, double v2,
                                     double h1, double h2, double g);
    static double bernoulliVelocity(double p1, double p2, double rho,
                                     double h1, double h2, double g);
    static double torricelliVelocity(double g, double h);
    static double venturiVelocity(double A1, double A2, double v1);
    static double pitotVelocity(double pStag, double pStatic, double rho);
    void exportBernoulliCSV(const std::string& filename, double p1, double v1,
                            double h1, double g, int numPoints);

    // ════════════════════════════════════════════════════════════════════
    //  3. EULER EQUATIONS (1D)
    // ════════════════════════════════════════════════════════════════════
    struct EulerState1D {
        std::vector<double> x;
        std::vector<double> rhoField;
        std::vector<double> u;
        std::vector<double> p;
    };
    static EulerState1D solveEuler1D(double rhoL, double uL, double pL,
                                      double rhoR, double uR, double pR,
                                      double gamma, double xMin, double xMax,
                                      int nx, double tFinal, double cfl);
    void exportEuler1DCSV(const std::string& filename, const EulerState1D& state);

    // ════════════════════════════════════════════════════════════════════
    //  4. NAVIER-STOKES (2D CAVITY)
    // ════════════════════════════════════════════════════════════════════
    FlowField2D solveNavierStokes2DCavity(int nx, int ny, double Re,
                                           double dt, int maxIter);
    void exportNavierStokes2DCSV(const std::string& filename, const FlowField2D& field);

    // ════════════════════════════════════════════════════════════════════
    //  5. POISEUILLE FLOW
    // ════════════════════════════════════════════════════════════════════
    static double poiseuilleVelocity(double dpdx, double mu, double R, double r);
    static double poiseuilleMaxVelocity(double dpdx, double mu, double R);
    static double poiseuilleFlowRate(double dpdx, double mu, double R);
    static double poiseuilleWallShear(double dpdx, double R);
    static double poiseuilleChannelVelocity(double dpdx, double mu, double h, double y);
    std::vector<double> poiseuilleProfile(double dpdx, double R, int numPoints);
    void exportPoiseuilleCSV(const std::string& filename, double dpdx, double R,
                              int numPoints);

    // ════════════════════════════════════════════════════════════════════
    //  6. COUETTE FLOW
    // ════════════════════════════════════════════════════════════════════
    static double couetteVelocity(double Utop, double h, double y, double dpdx, double mu);
    static double couetteShearStress(double mu, double Utop, double h, double dpdx, double y);
    std::vector<double> couetteProfile(double Utop, double h, double dpdx,
                                        int numPoints);
    void exportCouetteCSV(const std::string& filename, double Utop, double h,
                           double dpdx, int numPoints);

    // ════════════════════════════════════════════════════════════════════
    //  7. STOKES FLOW (CREEPING FLOW)
    // ════════════════════════════════════════════════════════════════════
    static double stokesDragForce(double mu, double R, double U);
    static double stokesDragCoefficient(double Re);
    static double stokesTerminalVelocity(double rhoP, double rhoF, double g,
                                          double mu, double R);
    static double stokesStreamfunction(double U, double R, double r, double theta);
    void exportStokesFlowCSV(const std::string& filename, double R, double U,
                              int numPoints);

    // ════════════════════════════════════════════════════════════════════
    //  8. POTENTIAL FLOW
    // ════════════════════════════════════════════════════════════════════
    struct PotentialFlowPoint {
        double x, y, u, v, phi, psi, Cp;
    };
    static std::pair<double,double> uniformFlow(double U, double alpha);
    static std::pair<double,double> sourceFlow(double m, double x, double y,
                                                double xs, double ys);
    static std::pair<double,double> vortexFlow(double gamma, double x, double y,
                                                double xv, double yv);
    static std::pair<double,double> doubletFlow(double kappa, double x, double y);
    std::vector<PotentialFlowPoint> flowAroundCylinder(double U, double R,
                                                        int nx, int ny,
                                                        double xMin, double xMax,
                                                        double yMin, double yMax);
    void exportPotentialFlowCSV(const std::string& filename, double U, double R,
                                 int nx, int ny);

    // ════════════════════════════════════════════════════════════════════
    //  9. STREAM FUNCTIONS & VISUALIZATION
    // ════════════════════════════════════════════════════════════════════
    static double streamFunctionUniform(double U, double x, double y);
    static double streamFunctionSource(double m, double x, double y);
    static double streamFunctionVortex(double gamma, double x, double y);
    static double streamFunctionDoublet(double kappa, double x, double y);
    static double streamFunctionRankineOval(double U, double m, double a,
                                             double x, double y);
    void exportStreamFunctionCSV(const std::string& filename, int type,
                                  double param1, double param2, int nx, int ny);

    // ════════════════════════════════════════════════════════════════════
    //  10. VORTICITY DYNAMICS
    // ════════════════════════════════════════════════════════════════════
    static double vortexRingVelocity(double gamma, double R);
    static double rankineVortexVelocity(double gamma, double R, double r);
    static double lambOseenVortexVelocity(double gamma, double nu, double r, double t);
    static double kelvinCirculationTheorem(double gamma0);  // dGamma/dt = 0
    std::vector<VortexState> evolvePointVortices(const std::vector<VortexState>& vortices,
                                                  double dt, int steps);
    void exportVorticityCSV(const std::string& filename, int mode,
                             double gamma, double R, double nu, int numPoints);

    // ════════════════════════════════════════════════════════════════════
    //  11. SURFACE GRAVITY WAVES
    // ════════════════════════════════════════════════════════════════════
    static double deepWaterDispersion(double k, double g);
    static double shallowWaterSpeed(double g, double h);
    static double generalDispersion(double k, double g, double h);
    static double waveGroupVelocity(double k, double g, double h);
    static double waveEnergy(double rho, double g, double A);
    static double stokesSecondOrderEta(double A, double k, double omega,
                                        double x, double t, double h);
    void exportSurfaceGravityWavesCSV(const std::string& filename, double A,
                                       double k, double h, double g, int nx, int nt);

    // ════════════════════════════════════════════════════════════════════
    //  12. SHALLOW WATER EQUATIONS
    // ════════════════════════════════════════════════════════════════════
    struct ShallowWaterState {
        std::vector<double> x;
        std::vector<double> h;  // water height
        std::vector<double> u;  // velocity
    };
    static ShallowWaterState solveShallowWater1D(double hL, double hR,
                                                   double uL, double uR,
                                                   double g, double xMin, double xMax,
                                                   int nx, double tFinal, double cfl);
    void exportShallowWaterCSV(const std::string& filename, const ShallowWaterState& state);

    // ════════════════════════════════════════════════════════════════════
    //  13. KELVIN-HELMHOLTZ INSTABILITY
    // ════════════════════════════════════════════════════════════════════
    static double khGrowthRate(double rho1, double rho2, double U1, double U2,
                                double k, double g, double sigma);
    static double khCriticalWavelength(double rho1, double rho2, double dU, double sigma);
    void exportKHInstabilityCSV(const std::string& filename, double rho1, double rho2,
                                 double dU, double sigma, double g, int numK);

    // ════════════════════════════════════════════════════════════════════
    //  14. RAYLEIGH-TAYLOR INSTABILITY
    // ════════════════════════════════════════════════════════════════════
    static double rtGrowthRate(double rho1, double rho2, double g, double k,
                                double sigma);
    static double rtCriticalWavelength(double rho1, double rho2, double g, double sigma);
    static double attwoodNumber(double rho1, double rho2);
    void exportRTInstabilityCSV(const std::string& filename, double rho1, double rho2,
                                 double g, double sigma, int numK);

    // ════════════════════════════════════════════════════════════════════
    //  15. RAYLEIGH-BÉNARD CONVECTION
    // ════════════════════════════════════════════════════════════════════
    static double rayleighNumber(double g, double beta, double dT, double L,
                                  double nu, double alpha);
    static double criticalRayleighNumber();  // 1707.76 for rigid-rigid
    static double nusseltNumberRB(double Ra, double Pr);
    static double convectionOnsetWavenumber();  // k_c ≈ 3.117
    void exportRayleighBenardCSV(const std::string& filename, double g, double beta,
                                  double nu, double alpha, double L, int numDT);

    // ════════════════════════════════════════════════════════════════════
    //  16. TAYLOR-COUETTE FLOW
    // ════════════════════════════════════════════════════════════════════
    static double taylorCouetteVelocity(double omega1, double omega2,
                                         double R1, double R2, double r);
    static double taylorNumber(double omega1, double R1, double R2, double nu);
    static double criticalTaylorNumber();  // ~1707
    void exportTaylorCouetteCSV(const std::string& filename, double omega1,
                                 double omega2, double R1, double R2, int numPoints);

    // ════════════════════════════════════════════════════════════════════
    //  17. CAPILLARY WAVES
    // ════════════════════════════════════════════════════════════════════
    static double capillaryWaveDispersion(double k, double sigma, double rho, double g);
    static double capillaryLength(double sigma, double rho, double g);
    static double capillaryMinPhaseSpeed(double sigma, double rho, double g);
    static double youngLaplaceDropletPressure(double sigma, double R);
    void exportCapillaryWavesCSV(const std::string& filename, double sigma,
                                  double rho, double g, int numK);

    // ════════════════════════════════════════════════════════════════════
    //  18. SOLITONS (KdV EQUATION)
    // ════════════════════════════════════════════════════════════════════
    static double kdvSoliton(double c, double x, double t, double x0);
    static double kdvSolitonAmplitude(double c);
    static double kdvSolitonWidth(double c);
    WaveResult solveKdV(double c1, double c2, double xMin, double xMax,
                         int nx, double tFinal, double dt);
    void exportKdVCSV(const std::string& filename, double c, double xMin, double xMax,
                       int nx, int nt, double tFinal);

    // ════════════════════════════════════════════════════════════════════
    //  19. SHOCK WAVES (BURGERS EQUATION)
    // ════════════════════════════════════════════════════════════════════
    struct BurgersState {
        std::vector<double> x;
        std::vector<double> u;
    };
    static BurgersState solveBurgersInviscid(double uL, double uR,
                                               double xMin, double xMax,
                                               int nx, double tFinal, double cfl);
    static BurgersState solveBurgersViscous(double uL, double uR, double nu,
                                              double xMin, double xMax,
                                              int nx, double tFinal, double dt);
    static double burgersExactShockSpeed(double uL, double uR);
    void exportBurgersCSV(const std::string& filename, const BurgersState& state);

    // ════════════════════════════════════════════════════════════════════
    //  20. RIEMANN PROBLEM
    // ════════════════════════════════════════════════════════════════════
    struct RiemannSolution {
        std::vector<double> x;
        std::vector<double> rhoField;
        std::vector<double> u;
        std::vector<double> p;
        double pStar;    // pressure in star region
        double uStar;    // velocity in star region
    };
    static RiemannSolution solveRiemannExact(double rhoL, double uL, double pL,
                                               double rhoR, double uR, double pR,
                                               double gamma, double xMin, double xMax,
                                               int nx, double t);
    void exportRiemannCSV(const std::string& filename, const RiemannSolution& sol);

    // ════════════════════════════════════════════════════════════════════
    //  21. SPEED OF SOUND
    // ════════════════════════════════════════════════════════════════════
    static double speedOfSoundIdealGas(double gamma, double R_specific, double T);
    static double speedOfSoundLiquid(double K, double rho);
    static double machNumber(double v, double c);
    static double machAngle(double M);
    void exportSpeedOfSoundCSV(const std::string& filename, double gamma,
                                double R_specific, double Tmin, double Tmax, int numT);

    // ════════════════════════════════════════════════════════════════════
    //  22. ISENTROPIC FLOW
    // ════════════════════════════════════════════════════════════════════
    static double isentropicPressureRatio(double M, double gamma);
    static double isentropicTemperatureRatio(double M, double gamma);
    static double isentropicDensityRatio(double M, double gamma);
    static double isentropicAreaRatio(double M, double gamma);
    static double machFromAreaRatio(double AoverAstar, double gamma, bool subsonic);
    void exportIsentropicFlowCSV(const std::string& filename, double gamma,
                                  double Mmax, int numM);

    // ════════════════════════════════════════════════════════════════════
    //  23. NORMAL SHOCKS
    // ════════════════════════════════════════════════════════════════════
    static ShockResult normalShock(double M1, double gamma);
    void exportNormalShockCSV(const std::string& filename, double gamma,
                               double M1min, double M1max, int numM);

    // ════════════════════════════════════════════════════════════════════
    //  24. OBLIQUE SHOCKS
    // ════════════════════════════════════════════════════════════════════
    static double obliqueShockBeta(double M1, double theta, double gamma, bool weak);
    static double obliqueShockMn1(double M1, double beta);
    static ShockResult obliqueShock(double M1, double beta, double gamma);
    static double maxDeflectionAngle(double M1, double gamma);
    void exportObliqueShockCSV(const std::string& filename, double M1, double gamma,
                                int numTheta);

    // ════════════════════════════════════════════════════════════════════
    //  25. PRANDTL-MEYER EXPANSION
    // ════════════════════════════════════════════════════════════════════
    static double prandtlMeyerFunction(double M, double gamma);
    static double machFromPrandtlMeyer(double nu, double gamma);
    static double expansionFanM2(double M1, double dTheta, double gamma);
    void exportPrandtlMeyerCSV(const std::string& filename, double gamma,
                                 double Mmax, int numM);

    // ════════════════════════════════════════════════════════════════════
    //  26. NOZZLE FLOW (DE LAVAL)
    // ════════════════════════════════════════════════════════════════════
    NozzleResult solveDeLavalNozzle(double gamma, double p0, double T0,
                                     double R_specific, double Athroat,
                                     double Aexit, int numX);
    double nozzleThrustCoefficient(double gamma, double pe_p0, double Ae_At);
    void exportNozzleFlowCSV(const std::string& filename, const NozzleResult& result);

    // ════════════════════════════════════════════════════════════════════
    //  27. FANNO FLOW (ADIABATIC DUCT WITH FRICTION)
    // ════════════════════════════════════════════════════════════════════
    static double fannoFLstarOverD(double M, double gamma);
    static double fannoTemperatureRatio(double M, double gamma);
    static double fannoPressureRatio(double M, double gamma);
    static double fannoStagnationPressureRatio(double M, double gamma);
    void exportFannoFlowCSV(const std::string& filename, double gamma,
                              double Mmax, int numM);

    // ════════════════════════════════════════════════════════════════════
    //  28. RAYLEIGH FLOW (FRICTIONLESS DUCT WITH HEAT ADDITION)
    // ════════════════════════════════════════════════════════════════════
    static double rayleighTemperatureRatio(double M, double gamma);
    static double rayleighPressureRatio(double M, double gamma);
    static double rayleighStagnationTemperatureRatio(double M, double gamma);
    static double rayleighStagnationPressureRatio(double M, double gamma);
    void exportRayleighFlowCSV(const std::string& filename, double gamma,
                                 double Mmax, int numM);

    // ════════════════════════════════════════════════════════════════════
    //  29. REYNOLDS NUMBER REGIMES
    // ════════════════════════════════════════════════════════════════════
    static double reynoldsNum(double rho, double U, double L, double mu);
    static std::string flowRegime(double Re);
    static double criticalReynoldsPipe();   // ~2300
    static double criticalReynoldsPlate();  // ~5e5
    static double dragCoefficientSphere(double Re);
    static double frictionFactorLaminar(double Re);
    static double frictionFactorTurbulentColebrook(double Re, double epsOverD);
    void exportReynoldsRegimesCSV(const std::string& filename, int numRe);

    // ════════════════════════════════════════════════════════════════════
    //  30. KOLMOGOROV CASCADE
    // ════════════════════════════════════════════════════════════════════
    static double kolmogorovLengthScale(double nu, double epsilon);
    static double kolmogorovTimeScale(double nu, double epsilon);
    static double kolmogorovVelocityScale(double nu, double epsilon);
    static double kolmogorovEnergySpectrum(double C_K, double epsilon, double k);
    static double integralLengthScale(double u_rms, double epsilon);
    static double taylorMicroscale(double u_rms, double nu, double epsilon);
    static double reTurbulent(double u_rms, double L_int, double nu);
    void exportKolmogorovCSV(const std::string& filename, double nu, double epsilon,
                              double C_K, int numK);

    // ════════════════════════════════════════════════════════════════════
    //  31. MIXING LENGTH THEORY
    // ════════════════════════════════════════════════════════════════════
    static double mixingLength(double kappa, double y);
    static double turbulentViscosity(double lm, double dudy);
    static double logLawVelocity(double uTau, double y, double nu,
                                  double kappa, double B);
    static double frictionVelocity(double tauW, double rho);
    static double viscousSublayerThickness(double nu, double uTau);
    BoundaryLayerResult turbulentBLProfile(double U_inf, double nu,
                                            double xMax, int numX);
    void exportMixingLengthCSV(const std::string& filename, double U_inf,
                                double nu, int numPoints);

    // ════════════════════════════════════════════════════════════════════
    //  32. BOUNDARY LAYERS (BLASIUS)
    // ════════════════════════════════════════════════════════════════════
    struct BlasiusProfile {
        std::vector<double> eta;
        std::vector<double> f;      // f(eta)
        std::vector<double> fp;     // f'(eta) = u/U_inf
        std::vector<double> fpp;    // f''(eta)
    };
    static BlasiusProfile solveBlasius(double etaMax, int numPoints);
    static double blasiusBLThickness(double x, double Re_x);
    static double blasiusDisplacementThickness(double x, double Re_x);
    static double blasiusMomentumThickness(double x, double Re_x);
    static double blasiusSkinFriction(double Re_x);
    BoundaryLayerResult blasiusBLGrowth(double U_inf, double nu, double xMax,
                                         int numX);
    void exportBlasiusCSV(const std::string& filename, double U_inf, double nu,
                           double xMax, int numPoints);

    // ════════════════════════════════════════════════════════════════════
    //  33. DRAG ON A SPHERE
    // ════════════════════════════════════════════════════════════════════
    static double dragForce(double Cd, double rho, double U, double A);
    static double cdSphereEmpirical(double Re);
    static double terminalVelocitySphere(double rhoP, double rhoF, double d,
                                          double mu, double g);
    static double oseen_dragCoefficient(double Re);
    void exportDragSphereCSV(const std::string& filename, double rhoF, double rhoP,
                              double d, double mu, double g, int numRe);

    // ════════════════════════════════════════════════════════════════════
    //  34. PIPE FLOW (MOODY DIAGRAM)
    // ════════════════════════════════════════════════════════════════════
    static double darcyFrictionFactor(double Re, double epsOverD);
    static double headLoss(double f, double L, double D, double V, double g);
    static double pressureDrop(double f, double L, double D, double rho, double V);
    static double pipeFlowRate(double D, double dpdx, double mu, double rho);
    void exportMoodyDiagramCSV(const std::string& filename, int numRe, int numEps);

    // ════════════════════════════════════════════════════════════════════
    //  35. HEAT CONVECTION
    // ════════════════════════════════════════════════════════════════════
    static double nusseltFlatPlate(double Re, double Pr, bool turbulent);
    static double nusseltCylinder(double Re, double Pr);
    static double nusseltPipeInternal(double Re, double Pr, bool turbulent);
    static double convectiveHeatTransfer(double Nu, double k, double L);
    static double thermalBLThickness(double delta, double Pr);
    void exportHeatConvectionCSV(const std::string& filename, double Pr,
                                   double Remax, int numRe);

    // ════════════════════════════════════════════════════════════════════
    //  36. MASS DIFFUSION (FICK'S LAW)
    // ════════════════════════════════════════════════════════════════════
    static double fickFlux(double D, double dcdx);
    static double diffusionSolution1D(double C0, double x, double D, double t);
    static double schmidtNumber(double nu, double D);
    static double sherwoodNumber(double Re, double Sc);
    static double massTransferCoefficient(double Sh, double D, double L);
    void exportDiffusionCSV(const std::string& filename, double C0, double D,
                              double xMax, int nx, int nt, double tMax);

    // ════════════════════════════════════════════════════════════════════
    //  37. MAGNETOHYDRODYNAMICS (MHD)
    // ════════════════════════════════════════════════════════════════════
    static double alfvenSpeed(double B, double rho, double mu0);
    static double magnetosonicSpeedFast(double cs, double vA, double theta);
    static double magnetosonicSpeedSlow(double cs, double vA, double theta);
    static double magneticPressure(double B, double mu0);
    static double magneticReynoldsNumber(double U, double L, double eta);
    static double hartmannNumber(double B, double L, double sigma, double mu);
    MHDState solveMHD1D(double rhoL, double rhoR, double pL, double pR,
                         double BxL, double ByL, double BxR, double ByR,
                         double xMin, double xMax, int nx, double tFinal);
    void exportMHDCSV(const std::string& filename, const MHDState& state);

    // ════════════════════════════════════════════════════════════════════
    //  38. ALFVÉN WAVES
    // ════════════════════════════════════════════════════════════════════
    static double alfvenWaveDispersion(double k, double B, double rho, double mu0);
    static double alfvenWaveGroupVelocity(double B, double rho, double mu0);
    static double alfvenWaveMagneticField(double B0, double dB, double k,
                                           double omega, double x, double t);
    void exportAlfvenWavesCSV(const std::string& filename, double B0, double rho,
                                double mu0, double dB, int numK, int numX);

    // ════════════════════════════════════════════════════════════════════
    //  39. PLASMA BETA
    // ════════════════════════════════════════════════════════════════════
    static double plasmaBeta(double p, double B, double mu0);
    static double debyeLength(double T, double n, double q, double eps0);
    static double plasmaFrequency(double n, double q, double m, double eps0);
    static double gyroRadius(double m, double v_perp, double q, double B);
    static double gyroFrequency(double q, double B, double m);
    void exportPlasmaBetaCSV(const std::string& filename, double T, double n,
                               double Bmin, double Bmax, int numB);

    // ════════════════════════════════════════════════════════════════════
    //  40. ACCRETION DISK MODEL
    // ════════════════════════════════════════════════════════════════════
    static double keplerianVelocity(double M, double r, double G);
    static double keplerianAngularVelocity(double M, double r, double G);
    static double viscousAccretionRate(double Sigma, double nu, double r);
    static double diskTemperature(double Mdot, double M, double r,
                                   double G, double sigma_SB);
    static double eddingtonLuminosity(double M, double c, double sigmaT,
                                       double mp, double G);
    void exportAccretionDiskCSV(const std::string& filename, double M, double Mdot,
                                  double rMin, double rMax, int numR);

    // ════════════════════════════════════════════════════════════════════
    //  41. JEANS INSTABILITY
    // ════════════════════════════════════════════════════════════════════
    static double jeansLength(double cs, double G, double rho);
    static double jeansMass(double cs, double G, double rho);
    static double jeansWavenumber(double cs, double G, double rho);
    static double jeansGrowthRate(double k, double cs, double G, double rho);
    static double freefall_time(double G, double rho);
    void exportJeansInstabilityCSV(const std::string& filename, double cs,
                                     double G, double rho, int numK);

    // ════════════════════════════════════════════════════════════════════
    //  42. BONDI ACCRETION
    // ════════════════════════════════════════════════════════════════════
    static double bondiRadius(double M, double cs, double G);
    static double bondiAccretionRate(double rho_inf, double M, double cs, double G);
    static double bondiMachProfile(double r, double r_B, double gamma);
    void exportBondiAccretionCSV(const std::string& filename, double M, double cs,
                                   double rho_inf, double G, double gamma, int numR);

    // ════════════════════════════════════════════════════════════════════
    //  43. VISCOUS DISK (SHAKURA-SUNYAEV)
    // ════════════════════════════════════════════════════════════════════
    static double shakuraSunyaevViscosity(double alpha_SS, double cs, double H);
    static double diskScaleHeight(double cs, double Omega);
    static double diskSurfaceDensityEvolution(double Sigma0, double nu, double r,
                                               double t);
    static double viscousTimescale(double R, double nu);
    void exportViscousDiskCSV(const std::string& filename, double alpha_SS,
                                double M, double Mdot, double rMin, double rMax,
                                int numR);

    // ════════════════════════════════════════════════════════════════════
    //  44. RELATIVISTIC FLUID DYNAMICS
    // ════════════════════════════════════════════════════════════════════
    static double relativisticEnthalpy(double rho, double p, double eps, double c2);
    static double relativisticSoundSpeed(double gamma, double p, double rho,
                                          double eps, double c2);
    static double lorentzFactor(double v, double c);
    static double relativisticBernoulli(double h, double gamma_L, double c);
    struct RelativisticShockResult {
        double rho2, p2, v2, gamma2;
    };
    static RelativisticShockResult relativisticNormalShock(double rho1, double p1,
                                                            double v1, double gamma_eos,
                                                            double c);
    void exportRelativisticFluidCSV(const std::string& filename, double gamma_eos,
                                      double c, int numV);

    // ════════════════════════════════════════════════════════════════════
    //  45. LATTICE BOLTZMANN (TOY MODEL)
    // ════════════════════════════════════════════════════════════════════
    struct LBMState {
        int nx, ny;
        std::vector<double> ux, uy;
        std::vector<double> rhoField;
    };
    LBMState solveLBM_D2Q9(int nx, int ny, double tau, double uLid,
                             int maxIter);
    void exportLBMCSV(const std::string& filename, const LBMState& state);

    // ════════════════════════════════════════════════════════════════════
    //  46. SPH (TOY MODEL)
    // ════════════════════════════════════════════════════════════════════
    struct SPHParticle {
        double x, y, vx, vy, rho_p, p_p, m;
    };
    static double sphKernelCubic(double r, double h);
    static double sphKernelGradient(double r, double h);
    std::vector<SPHParticle> solveSPH_DamBreak(int numParticles, double h_smooth,
                                                 double dt, int steps,
                                                 double cs, double rho0);
    void exportSPHCSV(const std::string& filename,
                        const std::vector<SPHParticle>& particles);

    // ════════════════════════════════════════════════════════════════════
    //  47. VORTEX METHODS
    // ════════════════════════════════════════════════════════════════════
    static std::pair<double,double> biotSavartVelocity(double x, double y,
                                                         const std::vector<VortexState>& vortices,
                                                         double epsilon);
    std::vector<std::vector<VortexState>> evolveVortexSheet(
        const std::vector<VortexState>& initial, double dt, int steps, double epsilon);
    void exportVortexMethodCSV(const std::string& filename,
                                 const std::vector<VortexState>& vortices);

    // ════════════════════════════════════════════════════════════════════
    //  48. MULTIPHASE FLOW
    // ════════════════════════════════════════════════════════════════════
    static double voidFraction(double Qg, double Ql, double Ug, double Ul);
    static double mixtureVelocity(double Qg, double Ql, double A);
    static double driftFluxVelocity(double C0, double Um, double Ugj);
    static double slipRatio(double rhoL, double rhoG, double x_quality);
    static double twoPhaseMultiplier(double x_quality, double rhoL, double rhoG,
                                      double muL, double muG);
    void exportMultiphaseFlowCSV(const std::string& filename, double rhoL, double rhoG,
                                   double muL, double muG, int numX);

    // ════════════════════════════════════════════════════════════════════
    //  49. NON-NEWTONIAN FLUIDS
    // ════════════════════════════════════════════════════════════════════
    static double powerLawViscosity(double K, double n, double gammaDot);
    static double binghamViscosity(double tau_y, double mu_p, double gammaDot);
    static double carreauViscosity(double mu0, double muInf, double lambda,
                                    double n, double gammaDot);
    static double powerLawPipeVelocity(double dpdx, double K, double n,
                                        double R, double r);
    static double binghamPlugRadius(double tau_y, double dpdx);
    void exportNonNewtonianCSV(const std::string& filename, int model,
                                 double param1, double param2, double param3,
                                 int numPoints);

    // ════════════════════════════════════════════════════════════════════
    //  50. MICROFLUIDICS
    // ════════════════════════════════════════════════════════════════════
    static double hydraulicDiameter(double w, double h);
    static double microchannelResistance(double mu, double L, double w, double h);
    static double capillaryNumber(double mu, double U, double sigma);
    static double deanNumber(double Re, double D, double Rc);
    static double knudsenNumber(double lambda_mfp, double L);
    static double slipVelocity(double lambda_mfp, double dudy);
    static double electro_osmoticVelocity(double eps, double zeta, double E,
                                            double mu);
    static double mixingTimeDiffusion(double w, double D);
    void exportMicrofluidicsCSV(const std::string& filename, double mu, double sigma,
                                  double w, double h, double L, int numQ);
};

#endif // FLUID_SYSTEM_H
