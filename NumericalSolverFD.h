#ifndef NUMERICAL_SOLVER_FD_H
#define NUMERICAL_SOLVER_FD_H

#include <vector>
#include <string>
#include <complex>
#include <functional>

// ════════════════════════════════════════════════════════════════════════════
//  NumericalSolverFD — PDE solvers for fluid dynamics
// ════════════════════════════════════════════════════════════════════════════

class NumericalSolverFD {
public:
    // ── 1D Advection (upwind) ───────────────────────────────────────────
    static std::vector<double> solveAdvection1D(
        const std::vector<double>& u0, double c, double dx, double dt,
        int numSteps);

    // ── 1D Diffusion (FTCS) ────────────────────────────────────────────
    static std::vector<double> solveDiffusion1D(
        const std::vector<double>& u0, double D, double dx, double dt,
        int numSteps);

    // ── 1D Advection-Diffusion ─────────────────────────────────────────
    static std::vector<double> solveAdvectionDiffusion1D(
        const std::vector<double>& u0, double c, double D,
        double dx, double dt, int numSteps);

    // ── Lax-Wendroff scheme (1D) ───────────────────────────────────────
    static std::vector<double> laxWendroff1D(
        const std::vector<double>& u0, double c, double dx, double dt,
        int numSteps);

    // ── Crank-Nicolson for 1D diffusion ────────────────────────────────
    static std::vector<double> crankNicolsonDiffusion1D(
        const std::vector<double>& u0, double D, double dx, double dt,
        int numSteps);

    // ── Runge-Kutta 4th order ODE integrator ───────────────────────────
    struct ODEState {
        std::vector<double> y;
    };
    using ODERhs = std::function<ODEState(double t, const ODEState& state)>;
    static ODEState rk4Step(double t, const ODEState& state, double dt,
                             ODERhs rhs);
    static std::vector<ODEState> integrateRK4(const ODEState& y0, double t0,
                                                double tEnd, double dt,
                                                ODERhs rhs);

    // ── Thomas algorithm (tridiagonal solver) ──────────────────────────
    static std::vector<double> thomasSolve(
        const std::vector<double>& a,  // sub-diagonal
        const std::vector<double>& b,  // main diagonal
        const std::vector<double>& c,  // super-diagonal
        const std::vector<double>& d); // RHS

    // ── Poisson solver (2D, Jacobi iteration) ──────────────────────────
    static std::vector<double> poissonJacobi2D(
        int nx, int ny, double dx, double dy,
        const std::vector<double>& rhs,
        int maxIter, double tol);

    // ── SOR (Successive Over-Relaxation) for Poisson ───────────────────
    static std::vector<double> poissonSOR2D(
        int nx, int ny, double dx, double dy,
        const std::vector<double>& rhs,
        double omega, int maxIter, double tol);

    // ── Initial condition generators ───────────────────────────────────
    static std::vector<double> makeGaussianPulse(int numPoints, double xMin,
                                                   double xMax, double x0,
                                                   double sigma, double amp);

    static std::vector<double> makeStepFunction(int numPoints, double xMin,
                                                  double xMax, double xStep,
                                                  double uL, double uR);

    static std::vector<double> makeSineWave(int numPoints, double xMin,
                                              double xMax, double amplitude,
                                              double wavenumber);

    // ── Export helpers ──────────────────────────────────────────────────
    static void exportSolution1DCSV(const std::string& filename,
                                      const std::vector<double>& x,
                                      const std::vector<double>& u);

    static void exportSolution2DCSV(const std::string& filename,
                                      int nx, int ny, double dx, double dy,
                                      const std::vector<double>& u);
};

#endif // NUMERICAL_SOLVER_FD_H
