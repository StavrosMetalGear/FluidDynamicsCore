// ════════════════════════════════════════════════════════════════════════════
//  NumericalSolverFD.cpp — PDE solvers for FluidDynamicsCore
// ════════════════════════════════════════════════════════════════════════════

#include "NumericalSolverFD.h"
#include <cmath>
#include <fstream>
#include <algorithm>

using namespace std;

// ── 1D Advection (first-order upwind) ──────────────────────────────────────
vector<double> NumericalSolverFD::solveAdvection1D(
    const vector<double>& u0, double c, double dx, double dt, int numSteps) {
    int n = (int)u0.size();
    vector<double> u = u0, un(n);
    double r = c * dt / dx;
    for (int step = 0; step < numSteps; ++step) {
        for (int i = 1; i < n - 1; ++i) {
            if (c >= 0)
                un[i] = u[i] - r * (u[i] - u[i-1]);
            else
                un[i] = u[i] - r * (u[i+1] - u[i]);
        }
        un[0] = u[0]; un[n-1] = u[n-1];
        u = un;
    }
    return u;
}

// ── 1D Diffusion (FTCS) ───────────────────────────────────────────────────
vector<double> NumericalSolverFD::solveDiffusion1D(
    const vector<double>& u0, double D, double dx, double dt, int numSteps) {
    int n = (int)u0.size();
    vector<double> u = u0, un(n);
    double r = D * dt / (dx * dx);
    for (int step = 0; step < numSteps; ++step) {
        for (int i = 1; i < n - 1; ++i)
            un[i] = u[i] + r * (u[i+1] - 2.0 * u[i] + u[i-1]);
        un[0] = u[0]; un[n-1] = u[n-1];
        u = un;
    }
    return u;
}

// ── 1D Advection-Diffusion ────────────────────────────────────────────────
vector<double> NumericalSolverFD::solveAdvectionDiffusion1D(
    const vector<double>& u0, double c, double D, double dx, double dt,
    int numSteps) {
    int n = (int)u0.size();
    vector<double> u = u0, un(n);
    double ra = c * dt / dx;
    double rd = D * dt / (dx * dx);
    for (int step = 0; step < numSteps; ++step) {
        for (int i = 1; i < n - 1; ++i)
            un[i] = u[i] - 0.5 * ra * (u[i+1] - u[i-1])
                    + rd * (u[i+1] - 2.0 * u[i] + u[i-1]);
        un[0] = u[0]; un[n-1] = u[n-1];
        u = un;
    }
    return u;
}

// ── Lax-Wendroff ──────────────────────────────────────────────────────────
vector<double> NumericalSolverFD::laxWendroff1D(
    const vector<double>& u0, double c, double dx, double dt, int numSteps) {
    int n = (int)u0.size();
    vector<double> u = u0, un(n);
    double r = c * dt / dx;
    for (int step = 0; step < numSteps; ++step) {
        for (int i = 1; i < n - 1; ++i)
            un[i] = u[i] - 0.5 * r * (u[i+1] - u[i-1])
                    + 0.5 * r * r * (u[i+1] - 2.0 * u[i] + u[i-1]);
        un[0] = u[0]; un[n-1] = u[n-1];
        u = un;
    }
    return u;
}

// ── Crank-Nicolson for 1D diffusion ───────────────────────────────────────
vector<double> NumericalSolverFD::crankNicolsonDiffusion1D(
    const vector<double>& u0, double D, double dx, double dt, int numSteps) {
    int n = (int)u0.size();
    vector<double> u = u0;
    double r = D * dt / (2.0 * dx * dx);

    for (int step = 0; step < numSteps; ++step) {
        vector<double> a(n, -r), b(n, 1.0 + 2.0 * r), c(n, -r), d(n);
        for (int i = 1; i < n - 1; ++i)
            d[i] = r * u[i-1] + (1.0 - 2.0 * r) * u[i] + r * u[i+1];
        d[0] = u[0]; d[n-1] = u[n-1];
        b[0] = 1; c[0] = 0; a[n-1] = 0; b[n-1] = 1;
        u = thomasSolve(a, b, c, d);
    }
    return u;
}

// ── RK4 ───────────────────────────────────────────────────────────────────
NumericalSolverFD::ODEState NumericalSolverFD::rk4Step(
    double t, const ODEState& state, double dt, ODERhs rhs) {
    int n = (int)state.y.size();
    auto add = [&](const ODEState& a, const ODEState& b, double s) {
        ODEState r;
        r.y.resize(n);
        for (int i = 0; i < n; ++i) r.y[i] = a.y[i] + s * b.y[i];
        return r;
    };
    auto k1 = rhs(t, state);
    auto k2 = rhs(t + 0.5*dt, add(state, k1, 0.5*dt));
    auto k3 = rhs(t + 0.5*dt, add(state, k2, 0.5*dt));
    auto k4 = rhs(t + dt, add(state, k3, dt));

    ODEState result;
    result.y.resize(n);
    for (int i = 0; i < n; ++i)
        result.y[i] = state.y[i] + dt/6.0 * (k1.y[i] + 2*k2.y[i] + 2*k3.y[i] + k4.y[i]);
    return result;
}

vector<NumericalSolverFD::ODEState> NumericalSolverFD::integrateRK4(
    const ODEState& y0, double t0, double tEnd, double dt, ODERhs rhs) {
    vector<ODEState> trajectory;
    ODEState state = y0;
    double t = t0;
    trajectory.push_back(state);
    while (t < tEnd) {
        if (t + dt > tEnd) dt = tEnd - t;
        state = rk4Step(t, state, dt, rhs);
        t += dt;
        trajectory.push_back(state);
    }
    return trajectory;
}

// ── Thomas algorithm ──────────────────────────────────────────────────────
vector<double> NumericalSolverFD::thomasSolve(
    const vector<double>& a, const vector<double>& b,
    const vector<double>& c, const vector<double>& d) {
    int n = (int)d.size();
    vector<double> cp(n), dp(n), x(n);
    cp[0] = c[0] / b[0];
    dp[0] = d[0] / b[0];
    for (int i = 1; i < n; ++i) {
        double m = b[i] - a[i] * cp[i-1];
        cp[i] = c[i] / m;
        dp[i] = (d[i] - a[i] * dp[i-1]) / m;
    }
    x[n-1] = dp[n-1];
    for (int i = n - 2; i >= 0; --i)
        x[i] = dp[i] - cp[i] * x[i+1];
    return x;
}

// ── Poisson Jacobi 2D ────────────────────────────────────────────────────
vector<double> NumericalSolverFD::poissonJacobi2D(
    int nx, int ny, double dx, double dy,
    const vector<double>& rhs, int maxIter, double tol) {
    int sz = nx * ny;
    vector<double> u(sz, 0.0), un(sz, 0.0);
    double dx2 = dx * dx, dy2 = dy * dy;
    double denom = 2.0 * (1.0 / dx2 + 1.0 / dy2);

    for (int iter = 0; iter < maxIter; ++iter) {
        un = u;
        double maxDiff = 0;
        for (int j = 1; j < ny - 1; ++j)
            for (int i = 1; i < nx - 1; ++i) {
                int k = j * nx + i;
                u[k] = ((un[k-1] + un[k+1]) / dx2 +
                         (un[k-nx] + un[k+nx]) / dy2 - rhs[k]) / denom;
                maxDiff = max(maxDiff, fabs(u[k] - un[k]));
            }
        if (maxDiff < tol) break;
    }
    return u;
}

// ── Poisson SOR 2D ──────────────────────────────────────────────────────
vector<double> NumericalSolverFD::poissonSOR2D(
    int nx, int ny, double dx, double dy,
    const vector<double>& rhs, double omega, int maxIter, double tol) {
    int sz = nx * ny;
    vector<double> u(sz, 0.0);
    double dx2 = dx * dx, dy2 = dy * dy;
    double denom = 2.0 * (1.0 / dx2 + 1.0 / dy2);

    for (int iter = 0; iter < maxIter; ++iter) {
        double maxDiff = 0;
        for (int j = 1; j < ny - 1; ++j)
            for (int i = 1; i < nx - 1; ++i) {
                int k = j * nx + i;
                double uNew = ((u[k-1] + u[k+1]) / dx2 +
                               (u[k-nx] + u[k+nx]) / dy2 - rhs[k]) / denom;
                double diff = uNew - u[k];
                u[k] += omega * diff;
                maxDiff = max(maxDiff, fabs(omega * diff));
            }
        if (maxDiff < tol) break;
    }
    return u;
}

// ── Initial condition generators ──────────────────────────────────────────
vector<double> NumericalSolverFD::makeGaussianPulse(int numPoints, double xMin,
                                                      double xMax, double x0,
                                                      double sigma, double amp) {
    vector<double> u(numPoints);
    for (int i = 0; i < numPoints; ++i) {
        double x = xMin + (xMax - xMin) * i / (numPoints - 1.0);
        u[i] = amp * exp(-(x - x0) * (x - x0) / (2.0 * sigma * sigma));
    }
    return u;
}

vector<double> NumericalSolverFD::makeStepFunction(int numPoints, double xMin,
                                                     double xMax, double xStep,
                                                     double uL, double uR) {
    vector<double> u(numPoints);
    for (int i = 0; i < numPoints; ++i) {
        double x = xMin + (xMax - xMin) * i / (numPoints - 1.0);
        u[i] = (x < xStep) ? uL : uR;
    }
    return u;
}

vector<double> NumericalSolverFD::makeSineWave(int numPoints, double xMin,
                                                 double xMax, double amplitude,
                                                 double wavenumber) {
    vector<double> u(numPoints);
    for (int i = 0; i < numPoints; ++i) {
        double x = xMin + (xMax - xMin) * i / (numPoints - 1.0);
        u[i] = amplitude * sin(wavenumber * x);
    }
    return u;
}

// ── Export helpers ─────────────────────────────────────────────────────────
void NumericalSolverFD::exportSolution1DCSV(const string& filename,
                                               const vector<double>& x,
                                               const vector<double>& u) {
    ofstream f(filename);
    f << "x,u\n";
    for (size_t i = 0; i < x.size(); ++i)
        f << x[i] << "," << u[i] << "\n";
}

void NumericalSolverFD::exportSolution2DCSV(const string& filename,
                                               int nx, int ny, double dx, double dy,
                                               const vector<double>& u) {
    ofstream f(filename);
    f << "x,y,u\n";
    for (int j = 0; j < ny; ++j)
        for (int i = 0; i < nx; ++i)
            f << i * dx << "," << j * dy << "," << u[j * nx + i] << "\n";
}
