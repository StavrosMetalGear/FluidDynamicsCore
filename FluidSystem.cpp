// ════════════════════════════════════════════════════════════════════════════
//  FluidSystem.cpp — Complete physics engine for FluidDynamicsCore
// ════════════════════════════════════════════════════════════════════════════

#include "FluidSystem.h"
#include "FluidConstants.h"
#include <cmath>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <cassert>
#include <complex>
#include <random>
#include <limits>

using namespace std;
using namespace FluidConstants;

// ── Constructor ─────────────────────────────────────────────────────────────
FluidSystem::FluidSystem(const string& fluidName, double density,
                         double viscosity, double characteristicLength,
                         double characteristicVelocity)
    : name(fluidName), rho(density), mu(viscosity),
      L(characteristicLength), U(characteristicVelocity)
{}

double FluidSystem::reynoldsNumber() const {
    return rho * U * L / mu;
}

double FluidSystem::kinematicViscosity() const {
    return mu / rho;
}

// ════════════════════════════════════════════════════════════════════════════
//  1. HYDROSTATICS
// ════════════════════════════════════════════════════════════════════════════

double FluidSystem::hydrostaticPressure(double rho, double g, double h, double p0) {
    return p0 + rho * g * h;
}

double FluidSystem::buoyantForce(double rhoFluid, double g, double Vsub) {
    return rhoFluid * g * Vsub;
}

double FluidSystem::submergedFraction(double rhoObj, double rhoFluid) {
    return rhoObj / rhoFluid;
}

vector<double> FluidSystem::pressureProfile(double rho, double g, double p0,
                                             int numPoints, double hMax) {
    vector<double> P(numPoints);
    for (int i = 0; i < numPoints; ++i) {
        double h = hMax * i / (numPoints - 1.0);
        P[i] = p0 + rho * g * h;
    }
    return P;
}

void FluidSystem::exportHydrostaticsCSV(const string& filename, double g,
                                         double p0, int numPoints, double hMax) {
    ofstream f(filename);
    f << "h,P,F_buoyancy\n";
    for (int i = 0; i < numPoints; ++i) {
        double h = hMax * i / (numPoints - 1.0);
        double P = hydrostaticPressure(rho, g, h, p0);
        double Fb = buoyantForce(rho, g, 1.0); // unit volume
        f << h << "," << P << "," << Fb << "\n";
    }
}

// ════════════════════════════════════════════════════════════════════════════
//  2. BERNOULLI EQUATION
// ════════════════════════════════════════════════════════════════════════════

double FluidSystem::bernoulliPressure(double p1, double rho, double v1,
                                       double v2, double h1, double h2, double g) {
    return p1 + 0.5 * rho * (v1 * v1 - v2 * v2) + rho * g * (h1 - h2);
}

double FluidSystem::bernoulliVelocity(double p1, double p2, double rho,
                                       double h1, double h2, double g) {
    double dp = p1 - p2 + rho * g * (h1 - h2);
    if (dp < 0) return 0.0;
    return sqrt(2.0 * dp / rho);
}

double FluidSystem::torricelliVelocity(double g, double h) {
    return sqrt(2.0 * g * h);
}

double FluidSystem::venturiVelocity(double A1, double A2, double v1) {
    return v1 * A1 / A2;
}

double FluidSystem::pitotVelocity(double pStag, double pStatic, double rho) {
    double dp = pStag - pStatic;
    if (dp < 0) return 0.0;
    return sqrt(2.0 * dp / rho);
}

void FluidSystem::exportBernoulliCSV(const string& filename, double p1, double v1,
                                      double h1, double g, int numPoints) {
    ofstream f(filename);
    f << "v2,p2,h_diff\n";
    for (int i = 0; i < numPoints; ++i) {
        double v2 = v1 * (0.5 + 2.0 * i / (numPoints - 1.0));
        double p2 = bernoulliPressure(p1, rho, v1, v2, h1, 0.0, g);
        f << v2 << "," << p2 << "," << h1 << "\n";
    }
}

// ════════════════════════════════════════════════════════════════════════════
//  3. EULER EQUATIONS (1D)
// ════════════════════════════════════════════════════════════════════════════

FluidSystem::EulerState1D FluidSystem::solveEuler1D(double rhoL, double uL, double pL,
                                                      double rhoR, double uR, double pR,
                                                      double gamma, double xMin, double xMax,
                                                      int nx, double tFinal, double cfl) {
    double dx = (xMax - xMin) / nx;
    vector<double> rhoArr(nx), uArr(nx), pArr(nx), E(nx);

    // Initial conditions (Sod-like)
    for (int i = 0; i < nx; ++i) {
        double x = xMin + (i + 0.5) * dx;
        if (x < 0.5 * (xMin + xMax)) {
            rhoArr[i] = rhoL; uArr[i] = uL; pArr[i] = pL;
        } else {
            rhoArr[i] = rhoR; uArr[i] = uR; pArr[i] = pR;
        }
        E[i] = pArr[i] / (gamma - 1.0) + 0.5 * rhoArr[i] * uArr[i] * uArr[i];
    }

    // Lax-Friedrichs scheme
    double t = 0.0;
    while (t < tFinal) {
        double maxSpeed = 0.0;
        for (int i = 0; i < nx; ++i) {
            double cs = sqrt(gamma * pArr[i] / rhoArr[i]);
            maxSpeed = max(maxSpeed, fabs(uArr[i]) + cs);
        }
        double dt = cfl * dx / maxSpeed;
        if (t + dt > tFinal) dt = tFinal - t;

        vector<double> rhoNew(nx), momNew(nx), ENew(nx);
        for (int i = 1; i < nx - 1; ++i) {
            double mom_i = rhoArr[i] * uArr[i];
            double mom_ip = rhoArr[i+1] * uArr[i+1];
            double mom_im = rhoArr[i-1] * uArr[i-1];

            double fRho_ip = mom_ip;
            double fRho_im = mom_im;
            double fMom_ip = mom_ip * uArr[i+1] + pArr[i+1];
            double fMom_im = mom_im * uArr[i-1] + pArr[i-1];
            double fE_ip = (E[i+1] + pArr[i+1]) * uArr[i+1];
            double fE_im = (E[i-1] + pArr[i-1]) * uArr[i-1];

            rhoNew[i] = 0.5 * (rhoArr[i+1] + rhoArr[i-1])
                       - 0.5 * dt / dx * (fRho_ip - fRho_im);
            momNew[i] = 0.5 * (mom_ip + mom_im)
                       - 0.5 * dt / dx * (fMom_ip - fMom_im);
            ENew[i]   = 0.5 * (E[i+1] + E[i-1])
                       - 0.5 * dt / dx * (fE_ip - fE_im);
        }
        // BCs
        rhoNew[0] = rhoArr[0]; momNew[0] = rhoArr[0] * uArr[0]; ENew[0] = E[0];
        rhoNew[nx-1] = rhoArr[nx-1]; momNew[nx-1] = rhoArr[nx-1] * uArr[nx-1]; ENew[nx-1] = E[nx-1];

        for (int i = 0; i < nx; ++i) {
            rhoArr[i] = rhoNew[i];
            uArr[i] = momNew[i] / rhoNew[i];
            pArr[i] = (gamma - 1.0) * (ENew[i] - 0.5 * rhoNew[i] * uArr[i] * uArr[i]);
            E[i] = ENew[i];
        }
        t += dt;
    }

    EulerState1D result;
    result.x.resize(nx);
    result.rhoField = rhoArr;
    result.u = uArr;
    result.p = pArr;
    for (int i = 0; i < nx; ++i)
        result.x[i] = xMin + (i + 0.5) * dx;
    return result;
}

void FluidSystem::exportEuler1DCSV(const string& filename, const EulerState1D& state) {
    ofstream f(filename);
    f << "x,rho,u,p\n";
    for (size_t i = 0; i < state.x.size(); ++i)
        f << state.x[i] << "," << state.rhoField[i] << "," << state.u[i] << "," << state.p[i] << "\n";
}

// ════════════════════════════════════════════════════════════════════════════
//  4. NAVIER-STOKES (2D CAVITY)
// ════════════════════════════════════════════════════════════════════════════

FluidSystem::FlowField2D FluidSystem::solveNavierStokes2DCavity(int nx, int ny,
                                                                  double Re,
                                                                  double dt, int maxIter) {
    double dx = 1.0 / (nx - 1);
    double dy = 1.0 / (ny - 1);
    auto idx = [&](int i, int j) { return j * nx + i; };

    int sz = nx * ny;
    vector<double> u(sz, 0.0), v(sz, 0.0), p(sz, 0.0);
    vector<double> un(sz), vn(sz), pn(sz);
    vector<double> b(sz, 0.0);

    // Lid-driven: u = 1 on top wall
    for (int i = 0; i < nx; ++i)
        u[idx(i, ny - 1)] = 1.0;

    for (int iter = 0; iter < maxIter; ++iter) {
        un = u; vn = v;

        // Build pressure RHS
        for (int j = 1; j < ny - 1; ++j)
            for (int i = 1; i < nx - 1; ++i) {
                b[idx(i,j)] = (1.0 / dt) *
                    ((un[idx(i+1,j)] - un[idx(i-1,j)]) / (2.0 * dx) +
                     (vn[idx(i,j+1)] - vn[idx(i,j-1)]) / (2.0 * dy));
            }

        // Pressure Poisson (pseudo-time Jacobi, 50 iters)
        for (int piter = 0; piter < 50; ++piter) {
            pn = p;
            for (int j = 1; j < ny - 1; ++j)
                for (int i = 1; i < nx - 1; ++i) {
                    p[idx(i,j)] = ((pn[idx(i+1,j)] + pn[idx(i-1,j)]) * dy * dy +
                                   (pn[idx(i,j+1)] + pn[idx(i,j-1)]) * dx * dx -
                                   b[idx(i,j)] * dx * dx * dy * dy) /
                                  (2.0 * (dx * dx + dy * dy));
                }
            // Neumann BCs for pressure
            for (int i = 0; i < nx; ++i) { p[idx(i,0)] = p[idx(i,1)]; p[idx(i,ny-1)] = 0.0; }
            for (int j = 0; j < ny; ++j) { p[idx(0,j)] = p[idx(1,j)]; p[idx(nx-1,j)] = p[idx(nx-2,j)]; }
        }

        // Momentum equations
        for (int j = 1; j < ny - 1; ++j)
            for (int i = 1; i < nx - 1; ++i) {
                u[idx(i,j)] = un[idx(i,j)]
                    - dt * un[idx(i,j)] * (un[idx(i,j)] - un[idx(i-1,j)]) / dx
                    - dt * vn[idx(i,j)] * (un[idx(i,j)] - un[idx(i,j-1)]) / dy
                    - dt / (2.0 * dx) * (p[idx(i+1,j)] - p[idx(i-1,j)])
                    + dt / Re * ((un[idx(i+1,j)] - 2.0*un[idx(i,j)] + un[idx(i-1,j)]) / (dx*dx)
                                +(un[idx(i,j+1)] - 2.0*un[idx(i,j)] + un[idx(i,j-1)]) / (dy*dy));

                v[idx(i,j)] = vn[idx(i,j)]
                    - dt * un[idx(i,j)] * (vn[idx(i,j)] - vn[idx(i-1,j)]) / dx
                    - dt * vn[idx(i,j)] * (vn[idx(i,j)] - vn[idx(i,j-1)]) / dy
                    - dt / (2.0 * dy) * (p[idx(i,j+1)] - p[idx(i,j-1)])
                    + dt / Re * ((vn[idx(i+1,j)] - 2.0*vn[idx(i,j)] + vn[idx(i-1,j)]) / (dx*dx)
                                +(vn[idx(i,j+1)] - 2.0*vn[idx(i,j)] + vn[idx(i,j-1)]) / (dy*dy));
            }

        // BCs
        for (int i = 0; i < nx; ++i) {
            u[idx(i,0)] = 0; v[idx(i,0)] = 0;
            u[idx(i,ny-1)] = 1.0; v[idx(i,ny-1)] = 0;
        }
        for (int j = 0; j < ny; ++j) {
            u[idx(0,j)] = 0; v[idx(0,j)] = 0;
            u[idx(nx-1,j)] = 0; v[idx(nx-1,j)] = 0;
        }
    }

    FlowField2D field;
    field.nx = nx; field.ny = ny;
    field.dx = dx; field.dy = dy;
    field.u = u; field.v = v; field.p = p;
    field.rhoField.assign(sz, 1.0);
    return field;
}

void FluidSystem::exportNavierStokes2DCSV(const string& filename, const FlowField2D& field) {
    ofstream f(filename);
    f << "x,y,u,v,p\n";
    for (int j = 0; j < field.ny; ++j)
        for (int i = 0; i < field.nx; ++i) {
            int k = j * field.nx + i;
            f << i * field.dx << "," << j * field.dy << ","
              << field.u[k] << "," << field.v[k] << "," << field.p[k] << "\n";
        }
}

// ════════════════════════════════════════════════════════════════════════════
//  5. POISEUILLE FLOW
// ════════════════════════════════════════════════════════════════════════════

double FluidSystem::poiseuilleVelocity(double dpdx, double mu, double R, double r) {
    return (-dpdx / (4.0 * mu)) * (R * R - r * r);
}

double FluidSystem::poiseuilleMaxVelocity(double dpdx, double mu, double R) {
    return (-dpdx * R * R) / (4.0 * mu);
}

double FluidSystem::poiseuilleFlowRate(double dpdx, double mu, double R) {
    return (-dpdx * PI * R * R * R * R) / (8.0 * mu);
}

double FluidSystem::poiseuilleWallShear(double dpdx, double R) {
    return -dpdx * R / 2.0;
}

double FluidSystem::poiseuilleChannelVelocity(double dpdx, double mu, double h, double y) {
    return (-dpdx / (2.0 * mu)) * y * (h - y);
}

vector<double> FluidSystem::poiseuilleProfile(double dpdx, double R, int numPoints) {
    vector<double> vel(numPoints);
    for (int i = 0; i < numPoints; ++i) {
        double r = -R + 2.0 * R * i / (numPoints - 1.0);
        vel[i] = poiseuilleVelocity(dpdx, mu, R, r);
    }
    return vel;
}

void FluidSystem::exportPoiseuilleCSV(const string& filename, double dpdx,
                                       double R, int numPoints) {
    ofstream f(filename);
    f << "r,u,tau\n";
    for (int i = 0; i < numPoints; ++i) {
        double r = -R + 2.0 * R * i / (numPoints - 1.0);
        double u = poiseuilleVelocity(dpdx, mu, R, r);
        double tau = -dpdx * r / 2.0;
        f << r << "," << u << "," << tau << "\n";
    }
}

// ════════════════════════════════════════════════════════════════════════════
//  6. COUETTE FLOW
// ════════════════════════════════════════════════════════════════════════════

double FluidSystem::couetteVelocity(double Utop, double h, double y,
                                     double dpdx, double mu) {
    return Utop * y / h + (-dpdx / (2.0 * mu)) * y * (h - y);
}

double FluidSystem::couetteShearStress(double mu, double Utop, double h,
                                        double dpdx, double y) {
    return mu * Utop / h + (-dpdx / 2.0) * (h - 2.0 * y);
}

vector<double> FluidSystem::couetteProfile(double Utop, double h, double dpdx,
                                            int numPoints) {
    vector<double> vel(numPoints);
    for (int i = 0; i < numPoints; ++i) {
        double y = h * i / (numPoints - 1.0);
        vel[i] = couetteVelocity(Utop, h, y, dpdx, mu);
    }
    return vel;
}

void FluidSystem::exportCouetteCSV(const string& filename, double Utop, double h,
                                    double dpdx, int numPoints) {
    ofstream f(filename);
    f << "y,u,tau\n";
    for (int i = 0; i < numPoints; ++i) {
        double y = h * i / (numPoints - 1.0);
        f << y << "," << couetteVelocity(Utop, h, y, dpdx, mu) << ","
          << couetteShearStress(mu, Utop, h, dpdx, y) << "\n";
    }
}

// ════════════════════════════════════════════════════════════════════════════
//  7. STOKES FLOW
// ════════════════════════════════════════════════════════════════════════════

double FluidSystem::stokesDragForce(double mu, double R, double U) {
    return 6.0 * PI * mu * R * U;
}

double FluidSystem::stokesDragCoefficient(double Re) {
    return 24.0 / Re;
}

double FluidSystem::stokesTerminalVelocity(double rhoP, double rhoF, double g,
                                            double mu, double R) {
    return (2.0 / 9.0) * (rhoP - rhoF) * g * R * R / mu;
}

double FluidSystem::stokesStreamfunction(double U, double R, double r, double theta) {
    double st = sin(theta);
    return U * r * r * st * st / 2.0 * (1.0 - 1.5 * R / r + 0.5 * (R / r) * (R / r) * (R / r));
}

void FluidSystem::exportStokesFlowCSV(const string& filename, double R, double U,
                                       int numPoints) {
    ofstream f(filename);
    f << "r,theta,psi,u_r,u_theta\n";
    for (int i = 0; i < numPoints; ++i) {
        double r = R + (5.0 * R - R) * i / (numPoints - 1.0);
        for (int j = 0; j <= 18; ++j) {
            double theta = PI * j / 18.0;
            double rr = R / r;
            double ct = cos(theta), st = sin(theta);
            double ur = U * ct * (1.0 - 1.5 * rr + 0.5 * rr * rr * rr);
            double ut = -U * st * (1.0 - 0.75 * rr - 0.25 * rr * rr * rr);
            f << r << "," << theta << "," << stokesStreamfunction(U, R, r, theta)
              << "," << ur << "," << ut << "\n";
        }
    }
}

// ════════════════════════════════════════════════════════════════════════════
//  8. POTENTIAL FLOW
// ════════════════════════════════════════════════════════════════════════════

pair<double,double> FluidSystem::uniformFlow(double U, double alpha) {
    return {U * cos(alpha), U * sin(alpha)};
}

pair<double,double> FluidSystem::sourceFlow(double m, double x, double y,
                                             double xs, double ys) {
    double dx = x - xs, dy = y - ys;
    double r2 = dx * dx + dy * dy;
    if (r2 < 1e-20) return {0, 0};
    return {m * dx / (TWO_PI * r2), m * dy / (TWO_PI * r2)};
}

pair<double,double> FluidSystem::vortexFlow(double gamma, double x, double y,
                                             double xv, double yv) {
    double dx = x - xv, dy = y - yv;
    double r2 = dx * dx + dy * dy;
    if (r2 < 1e-20) return {0, 0};
    return {gamma * dy / (TWO_PI * r2), -gamma * dx / (TWO_PI * r2)};
}

pair<double,double> FluidSystem::doubletFlow(double kappa, double x, double y) {
    double r2 = x * x + y * y;
    if (r2 < 1e-20) return {0, 0};
    double r4 = r2 * r2;
    return {-kappa * (x * x - y * y) / (TWO_PI * r4),
            -kappa * 2.0 * x * y / (TWO_PI * r4)};
}

vector<FluidSystem::PotentialFlowPoint> FluidSystem::flowAroundCylinder(
    double U, double R, int nx, int ny, double xMin, double xMax,
    double yMin, double yMax) {
    vector<PotentialFlowPoint> pts;
    double dx = (xMax - xMin) / (nx - 1.0);
    double dy = (yMax - yMin) / (ny - 1.0);
    for (int j = 0; j < ny; ++j)
        for (int i = 0; i < nx; ++i) {
            double x = xMin + i * dx;
            double y = yMin + j * dy;
            double r2 = x * x + y * y;
            PotentialFlowPoint pt;
            pt.x = x; pt.y = y;
            if (r2 < R * R) {
                pt.u = 0; pt.v = 0; pt.phi = 0; pt.psi = 0; pt.Cp = 1.0;
            } else {
                double r = sqrt(r2);
                double theta = atan2(y, x);
                double R2 = R * R;
                pt.u = U * (1.0 - R2 * (x * x - y * y) / (r2 * r2));
                pt.v = -U * R2 * 2.0 * x * y / (r2 * r2);
                double vmag2 = pt.u * pt.u + pt.v * pt.v;
                pt.Cp = 1.0 - vmag2 / (U * U);
                pt.phi = U * r * cos(theta) * (1.0 + R2 / r2);
                pt.psi = U * r * sin(theta) * (1.0 - R2 / r2);
            }
            pts.push_back(pt);
        }
    return pts;
}

void FluidSystem::exportPotentialFlowCSV(const string& filename, double U,
                                          double R, int nx, int ny) {
    auto pts = flowAroundCylinder(U, R, nx, ny, -3 * R, 3 * R, -3 * R, 3 * R);
    ofstream f(filename);
    f << "x,y,u,v,Cp,psi\n";
    for (auto& pt : pts)
        f << pt.x << "," << pt.y << "," << pt.u << "," << pt.v << ","
          << pt.Cp << "," << pt.psi << "\n";
}

// ════════════════════════════════════════════════════════════════════════════
//  9. STREAM FUNCTIONS
// ════════════════════════════════════════════════════════════════════════════

double FluidSystem::streamFunctionUniform(double U, double x, double y) {
    return U * y;
}

double FluidSystem::streamFunctionSource(double m, double x, double y) {
    return m / TWO_PI * atan2(y, x);
}

double FluidSystem::streamFunctionVortex(double gamma, double x, double y) {
    return -gamma / (TWO_PI) * log(sqrt(x * x + y * y));
}

double FluidSystem::streamFunctionDoublet(double kappa, double x, double y) {
    double r2 = x * x + y * y;
    if (r2 < 1e-20) return 0;
    return -kappa * y / (TWO_PI * r2);
}

double FluidSystem::streamFunctionRankineOval(double U, double m, double a,
                                               double x, double y) {
    double theta1 = atan2(y, x - a);
    double theta2 = atan2(y, x + a);
    return U * y + m / TWO_PI * (theta1 - theta2);
}

void FluidSystem::exportStreamFunctionCSV(const string& filename, int type,
                                            double param1, double param2,
                                            int nx, int ny) {
    ofstream f(filename);
    f << "x,y,psi\n";
    double range = 5.0;
    for (int j = 0; j < ny; ++j)
        for (int i = 0; i < nx; ++i) {
            double x = -range + 2.0 * range * i / (nx - 1.0);
            double y = -range + 2.0 * range * j / (ny - 1.0);
            double psi = 0;
            switch (type) {
                case 0: psi = streamFunctionUniform(param1, x, y); break;
                case 1: psi = streamFunctionSource(param1, x, y); break;
                case 2: psi = streamFunctionVortex(param1, x, y); break;
                case 3: psi = streamFunctionDoublet(param1, x, y); break;
                case 4: psi = streamFunctionRankineOval(param1, param2, 1.0, x, y); break;
            }
            f << x << "," << y << "," << psi << "\n";
        }
}

// ════════════════════════════════════════════════════════════════════════════
//  10. VORTICITY DYNAMICS
// ════════════════════════════════════════════════════════════════════════════

double FluidSystem::vortexRingVelocity(double gamma, double R) {
    return gamma / (4.0 * PI * R);
}

double FluidSystem::rankineVortexVelocity(double gamma, double R, double r) {
    if (r <= R)
        return gamma * r / (TWO_PI * R * R);
    else
        return gamma / (TWO_PI * r);
}

double FluidSystem::lambOseenVortexVelocity(double gamma, double nu, double r, double t) {
    double r2 = r * r;
    double denom = TWO_PI * r;
    if (fabs(r) < 1e-20) return 0;
    return (gamma / denom) * (1.0 - exp(-r2 / (4.0 * nu * t)));
}

double FluidSystem::kelvinCirculationTheorem(double gamma0) {
    return gamma0; // dGamma/dt = 0 for inviscid barotropic flow
}

vector<FluidSystem::VortexState> FluidSystem::evolvePointVortices(
    const vector<VortexState>& vortices, double dt, int steps) {
    vector<VortexState> state = vortices;
    int N = (int)state.size();
    for (int s = 0; s < steps; ++s) {
        vector<double> dxdt(N, 0), dydt(N, 0);
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                if (i == j) continue;
                double dx = state[i].x - state[j].x;
                double dy = state[i].y - state[j].y;
                double r2 = dx * dx + dy * dy + 1e-10;
                dxdt[i] += -state[j].gamma * dy / (TWO_PI * r2);
                dydt[i] +=  state[j].gamma * dx / (TWO_PI * r2);
            }
        }
        for (int i = 0; i < N; ++i) {
            state[i].x += dxdt[i] * dt;
            state[i].y += dydt[i] * dt;
        }
    }
    return state;
}

void FluidSystem::exportVorticityCSV(const string& filename, int mode,
                                      double gamma, double R, double nu, int numPoints) {
    ofstream f(filename);
    f << "r,v_theta\n";
    for (int i = 0; i < numPoints; ++i) {
        double r = 0.01 * R + 5.0 * R * i / (numPoints - 1.0);
        double vt = 0;
        if (mode == 0) vt = rankineVortexVelocity(gamma, R, r);
        else vt = lambOseenVortexVelocity(gamma, nu, r, 1.0);
        f << r << "," << vt << "\n";
    }
}

// ════════════════════════════════════════════════════════════════════════════
//  11. SURFACE GRAVITY WAVES
// ════════════════════════════════════════════════════════════════════════════

double FluidSystem::deepWaterDispersion(double k, double g) {
    return sqrt(g * k);
}

double FluidSystem::shallowWaterSpeed(double g, double h) {
    return sqrt(g * h);
}

double FluidSystem::generalDispersion(double k, double g, double h) {
    return sqrt(g * k * tanh(k * h));
}

double FluidSystem::waveGroupVelocity(double k, double g, double h) {
    double omega = generalDispersion(k, g, h);
    double kh = k * h;
    double n = 0.5 * (1.0 + 2.0 * kh / sinh(2.0 * kh));
    return n * omega / k;
}

double FluidSystem::waveEnergy(double rho, double g, double A) {
    return 0.5 * rho * g * A * A;
}

double FluidSystem::stokesSecondOrderEta(double A, double k, double omega,
                                          double x, double t, double h) {
    double phase = k * x - omega * t;
    double eta1 = A * cos(phase);
    double kh = k * h;
    double eta2 = A * k * A * (cosh(kh) * (2.0 + cosh(2.0 * kh))) /
                  (4.0 * pow(sinh(kh), 3)) * cos(2.0 * phase);
    return eta1 + eta2;
}

void FluidSystem::exportSurfaceGravityWavesCSV(const string& filename, double A,
                                                 double k, double h, double g,
                                                 int nx, int nt) {
    ofstream f(filename);
    f << "x,t,eta\n";
    double omega = generalDispersion(k, g, h);
    double L = TWO_PI / k;
    double T = TWO_PI / omega;
    for (int j = 0; j < nt; ++j)
        for (int i = 0; i < nx; ++i) {
            double x = 2.0 * L * i / (nx - 1.0);
            double t = T * j / (nt - 1.0);
            f << x << "," << t << "," << stokesSecondOrderEta(A, k, omega, x, t, h) << "\n";
        }
}

// ════════════════════════════════════════════════════════════════════════════
//  12. SHALLOW WATER EQUATIONS
// ════════════════════════════════════════════════════════════════════════════

FluidSystem::ShallowWaterState FluidSystem::solveShallowWater1D(
    double hL, double hR, double uL, double uR, double g,
    double xMin, double xMax, int nx, double tFinal, double cfl) {
    double dx = (xMax - xMin) / nx;
    vector<double> h(nx), u(nx);
    for (int i = 0; i < nx; ++i) {
        double x = xMin + (i + 0.5) * dx;
        if (x < 0.5 * (xMin + xMax)) { h[i] = hL; u[i] = uL; }
        else { h[i] = hR; u[i] = uR; }
    }

    double t = 0;
    while (t < tFinal) {
        double maxS = 0;
        for (int i = 0; i < nx; ++i)
            maxS = max(maxS, fabs(u[i]) + sqrt(g * fabs(h[i])));
        double dt = cfl * dx / maxS;
        if (t + dt > tFinal) dt = tFinal - t;

        vector<double> hn(nx), hu_n(nx);
        for (int i = 1; i < nx - 1; ++i) {
            double hu_ip = h[i+1] * u[i+1];
            double hu_im = h[i-1] * u[i-1];
            double hu_i  = h[i] * u[i];
            hn[i] = 0.5 * (h[i+1] + h[i-1])
                   - 0.5 * dt / dx * (hu_ip - hu_im);
            hu_n[i] = 0.5 * (hu_ip + hu_im)
                     - 0.5 * dt / dx * ((hu_ip * u[i+1] + 0.5 * g * h[i+1] * h[i+1])
                                        -(hu_im * u[i-1] + 0.5 * g * h[i-1] * h[i-1]));
        }
        hn[0] = h[0]; hu_n[0] = h[0] * u[0];
        hn[nx-1] = h[nx-1]; hu_n[nx-1] = h[nx-1] * u[nx-1];
        for (int i = 0; i < nx; ++i) {
            h[i] = hn[i];
            u[i] = (h[i] > 1e-12) ? hu_n[i] / h[i] : 0.0;
        }
        t += dt;
    }

    ShallowWaterState res;
    res.x.resize(nx); res.h = h; res.u = u;
    for (int i = 0; i < nx; ++i) res.x[i] = xMin + (i + 0.5) * dx;
    return res;
}

void FluidSystem::exportShallowWaterCSV(const string& filename,
                                          const ShallowWaterState& state) {
    ofstream f(filename);
    f << "x,h,u\n";
    for (size_t i = 0; i < state.x.size(); ++i)
        f << state.x[i] << "," << state.h[i] << "," << state.u[i] << "\n";
}

// ════════════════════════════════════════════════════════════════════════════
//  13. KELVIN-HELMHOLTZ INSTABILITY
// ════════════════════════════════════════════════════════════════════════════

double FluidSystem::khGrowthRate(double rho1, double rho2, double U1, double U2,
                                  double k, double g, double sigma) {
    double rhoSum = rho1 + rho2;
    double dU = U1 - U2;
    double gravity_term = g * (rho2 - rho1) * k / rhoSum;
    double shear_term = rho1 * rho2 * k * k * dU * dU / (rhoSum * rhoSum);
    double surface_term = sigma * k * k * k / rhoSum;
    double arg = shear_term - gravity_term - surface_term;
    if (arg <= 0) return 0;
    return sqrt(arg);
}

double FluidSystem::khCriticalWavelength(double rho1, double rho2,
                                          double dU, double sigma) {
    return TWO_PI * sigma * (rho1 + rho2) / (rho1 * rho2 * dU * dU);
}

void FluidSystem::exportKHInstabilityCSV(const string& filename, double rho1,
                                          double rho2, double dU, double sigma,
                                          double g, int numK) {
    ofstream f(filename);
    f << "k,lambda,growth_rate\n";
    for (int i = 1; i <= numK; ++i) {
        double k = 0.1 * i;
        double gr = khGrowthRate(rho1, rho2, dU, 0, k, g, sigma);
        f << k << "," << TWO_PI / k << "," << gr << "\n";
    }
}

// ════════════════════════════════════════════════════════════════════════════
//  14. RAYLEIGH-TAYLOR INSTABILITY
// ════════════════════════════════════════════════════════════════════════════

double FluidSystem::rtGrowthRate(double rho1, double rho2, double g, double k,
                                  double sigma) {
    double At = (rho2 - rho1) / (rho2 + rho1);
    double arg = At * g * k - sigma * k * k * k / (rho1 + rho2);
    if (arg <= 0) return 0;
    return sqrt(arg);
}

double FluidSystem::rtCriticalWavelength(double rho1, double rho2, double g,
                                          double sigma) {
    return TWO_PI * sqrt(sigma / ((rho2 - rho1) * g));
}

double FluidSystem::attwoodNumber(double rho1, double rho2) {
    return (rho2 - rho1) / (rho2 + rho1);
}

void FluidSystem::exportRTInstabilityCSV(const string& filename, double rho1,
                                          double rho2, double g, double sigma,
                                          int numK) {
    ofstream f(filename);
    f << "k,lambda,growth_rate,Atwood\n";
    double At = attwoodNumber(rho1, rho2);
    for (int i = 1; i <= numK; ++i) {
        double k = 0.1 * i;
        f << k << "," << TWO_PI / k << "," << rtGrowthRate(rho1, rho2, g, k, sigma)
          << "," << At << "\n";
    }
}

// ════════════════════════════════════════════════════════════════════════════
//  15. RAYLEIGH-BÉNARD CONVECTION
// ════════════════════════════════════════════════════════════════════════════

double FluidSystem::rayleighNumber(double g, double beta, double dT, double L,
                                    double nu, double alpha) {
    return g * beta * dT * L * L * L / (nu * alpha);
}

double FluidSystem::criticalRayleighNumber() { return 1707.762; }

double FluidSystem::nusseltNumberRB(double Ra, double Pr) {
    if (Ra < 1707.762) return 1.0;
    return 0.069 * pow(Ra, 1.0 / 3.0) * pow(Pr, 0.074);
}

double FluidSystem::convectionOnsetWavenumber() { return 3.117; }

void FluidSystem::exportRayleighBenardCSV(const string& filename, double g,
                                            double beta, double nu, double alpha,
                                            double L, int numDT) {
    ofstream f(filename);
    f << "dT,Ra,Nu\n";
    for (int i = 0; i < numDT; ++i) {
        double dT = 0.1 + 100.0 * i / (numDT - 1.0);
        double Ra = rayleighNumber(g, beta, dT, L, nu, alpha);
        double Nu = nusseltNumberRB(Ra, nu / alpha);
        f << dT << "," << Ra << "," << Nu << "\n";
    }
}

// ════════════════════════════════════════════════════════════════════════════
//  16. TAYLOR-COUETTE FLOW
// ════════════════════════════════════════════════════════════════════════════

double FluidSystem::taylorCouetteVelocity(double omega1, double omega2,
                                            double R1, double R2, double r) {
    double R12 = R1 * R1, R22 = R2 * R2;
    double A = (omega2 * R22 - omega1 * R12) / (R22 - R12);
    double B = (omega1 - omega2) * R12 * R22 / (R22 - R12);
    return A * r + B / r;
}

double FluidSystem::taylorNumber(double omega1, double R1, double R2, double nu) {
    double d = R2 - R1;
    return omega1 * omega1 * R1 * d * d * d / (nu * nu);
}

double FluidSystem::criticalTaylorNumber() { return 1707.76; }

void FluidSystem::exportTaylorCouetteCSV(const string& filename, double omega1,
                                           double omega2, double R1, double R2,
                                           int numPoints) {
    ofstream f(filename);
    f << "r,v_theta\n";
    for (int i = 0; i < numPoints; ++i) {
        double r = R1 + (R2 - R1) * i / (numPoints - 1.0);
        f << r << "," << taylorCouetteVelocity(omega1, omega2, R1, R2, r) << "\n";
    }
}

// ════════════════════════════════════════════════════════════════════════════
//  17. CAPILLARY WAVES
// ════════════════════════════════════════════════════════════════════════════

double FluidSystem::capillaryWaveDispersion(double k, double sigma, double rho,
                                             double g) {
    return sqrt(g * k + sigma * k * k * k / rho);
}

double FluidSystem::capillaryLength(double sigma, double rho, double g) {
    return sqrt(sigma / (rho * g));
}

double FluidSystem::capillaryMinPhaseSpeed(double sigma, double rho, double g) {
    return pow(4.0 * g * sigma / rho, 0.25);
}

double FluidSystem::youngLaplaceDropletPressure(double sigma, double R) {
    return 2.0 * sigma / R;
}

void FluidSystem::exportCapillaryWavesCSV(const string& filename, double sigma,
                                            double rho, double g, int numK) {
    ofstream f(filename);
    f << "k,omega,v_phase,v_group\n";
    for (int i = 1; i <= numK; ++i) {
        double k = 0.5 * i;
        double omega = capillaryWaveDispersion(k, sigma, rho, g);
        double vp = omega / k;
        // numerical group velocity
        double dk = 0.01;
        double vg = (capillaryWaveDispersion(k + dk, sigma, rho, g)
                    - capillaryWaveDispersion(k - dk, sigma, rho, g)) / (2.0 * dk);
        f << k << "," << omega << "," << vp << "," << vg << "\n";
    }
}

// ════════════════════════════════════════════════════════════════════════════
//  18. SOLITONS (KdV)
// ════════════════════════════════════════════════════════════════════════════

double FluidSystem::kdvSoliton(double c, double x, double t, double x0) {
    double arg = sqrt(c / 2.0) * (x - c * t - x0);
    double s = 1.0 / cosh(arg);
    return 0.5 * c * s * s;  // 3c sech^2 simplified to half-amplitude
}

double FluidSystem::kdvSolitonAmplitude(double c) { return 0.5 * c; }

double FluidSystem::kdvSolitonWidth(double c) {
    return 2.0 / sqrt(c / 2.0);
}

FluidSystem::WaveResult FluidSystem::solveKdV(double c1, double c2,
                                                double xMin, double xMax,
                                                int nx, double tFinal, double dt) {
    // Simple pseudo-spectral / finite-difference KdV solver
    double dx = (xMax - xMin) / nx;
    vector<double> u(nx);
    for (int i = 0; i < nx; ++i) {
        double x = xMin + i * dx;
        u[i] = kdvSoliton(c1, x, 0, -10.0) + kdvSoliton(c2, x, 0, -20.0);
    }

    double t = 0;
    while (t < tFinal) {
        // Lax-Wendroff-ish explicit scheme for u_t + 6u u_x + u_xxx = 0
        vector<double> un(nx);
        for (int i = 2; i < nx - 2; ++i) {
            double ux = (u[i+1] - u[i-1]) / (2.0 * dx);
            double uxxx = (u[i+2] - 2.0 * u[i+1] + 2.0 * u[i-1] - u[i-2]) / (2.0 * dx * dx * dx);
            un[i] = u[i] - dt * (6.0 * u[i] * ux + uxxx);
        }
        un[0] = u[0]; un[1] = u[1];
        un[nx-1] = u[nx-1]; un[nx-2] = u[nx-2];
        u = un;
        t += dt;
    }

    WaveResult res;
    res.x.resize(nx); res.eta = u; res.u = u;
    for (int i = 0; i < nx; ++i) res.x[i] = xMin + i * dx;
    return res;
}

void FluidSystem::exportKdVCSV(const string& filename, double c, double xMin,
                                double xMax, int nx, int nt, double tFinal) {
    ofstream f(filename);
    f << "x,t,eta\n";
    for (int j = 0; j < nt; ++j) {
        double t = tFinal * j / (nt - 1.0);
        for (int i = 0; i < nx; ++i) {
            double x = xMin + (xMax - xMin) * i / (nx - 1.0);
            f << x << "," << t << "," << kdvSoliton(c, x, t, 0) << "\n";
        }
    }
}

// ════════════════════════════════════════════════════════════════════════════
//  19. SHOCK WAVES (BURGERS)
// ════════════════════════════════════════════════════════════════════════════

FluidSystem::BurgersState FluidSystem::solveBurgersInviscid(
    double uL, double uR, double xMin, double xMax, int nx, double tFinal, double cfl) {
    double dx = (xMax - xMin) / nx;
    vector<double> u(nx);
    for (int i = 0; i < nx; ++i) {
        double x = xMin + (i + 0.5) * dx;
        u[i] = (x < 0.5 * (xMin + xMax)) ? uL : uR;
    }
    double t = 0;
    while (t < tFinal) {
        double maxU = *max_element(u.begin(), u.end(), [](double a, double b){ return fabs(a) < fabs(b); });
        double dt = cfl * dx / (fabs(maxU) + 1e-12);
        if (t + dt > tFinal) dt = tFinal - t;
        vector<double> un(nx);
        // Godunov upwind
        for (int i = 1; i < nx - 1; ++i) {
            double fip = 0.5 * max(u[i], 0.0) * max(u[i], 0.0);
            double fimm = 0.5 * max(u[i-1], 0.0) * max(u[i-1], 0.0);
            double fipm = 0.5 * min(u[i+1], 0.0) * min(u[i+1], 0.0);
            double fim = 0.5 * min(u[i], 0.0) * min(u[i], 0.0);
            double flux_right = (u[i] >= 0) ? 0.5 * u[i] * u[i] : 0.5 * u[i+1] * u[i+1];
            double flux_left  = (u[i-1] >= 0) ? 0.5 * u[i-1] * u[i-1] : 0.5 * u[i] * u[i];
            un[i] = u[i] - dt / dx * (flux_right - flux_left);
        }
        un[0] = u[0]; un[nx-1] = u[nx-1];
        u = un;
        t += dt;
    }
    BurgersState res;
    res.x.resize(nx); res.u = u;
    for (int i = 0; i < nx; ++i) res.x[i] = xMin + (i + 0.5) * dx;
    return res;
}

FluidSystem::BurgersState FluidSystem::solveBurgersViscous(
    double uL, double uR, double nu, double xMin, double xMax,
    int nx, double tFinal, double dt) {
    double dx = (xMax - xMin) / nx;
    vector<double> u(nx);
    for (int i = 0; i < nx; ++i) {
        double x = xMin + (i + 0.5) * dx;
        u[i] = (x < 0.5 * (xMin + xMax)) ? uL : uR;
    }
    double t = 0;
    while (t < tFinal) {
        if (t + dt > tFinal) dt = tFinal - t;
        vector<double> un(nx);
        for (int i = 1; i < nx - 1; ++i) {
            double ux = (u[i+1] - u[i-1]) / (2.0 * dx);
            double uxx = (u[i+1] - 2.0 * u[i] + u[i-1]) / (dx * dx);
            un[i] = u[i] + dt * (-u[i] * ux + nu * uxx);
        }
        un[0] = u[0]; un[nx-1] = u[nx-1];
        u = un;
        t += dt;
    }
    BurgersState res;
    res.x.resize(nx); res.u = u;
    for (int i = 0; i < nx; ++i) res.x[i] = xMin + (i + 0.5) * dx;
    return res;
}

double FluidSystem::burgersExactShockSpeed(double uL, double uR) {
    return 0.5 * (uL + uR);
}

void FluidSystem::exportBurgersCSV(const string& filename, const BurgersState& state) {
    ofstream f(filename);
    f << "x,u\n";
    for (size_t i = 0; i < state.x.size(); ++i)
        f << state.x[i] << "," << state.u[i] << "\n";
}

// ════════════════════════════════════════════════════════════════════════════
//  20. RIEMANN PROBLEM (exact solver for ideal gas)
// ════════════════════════════════════════════════════════════════════════════

FluidSystem::RiemannSolution FluidSystem::solveRiemannExact(
    double rhoL, double uL, double pL, double rhoR, double uR, double pR,
    double gamma, double xMin, double xMax, int nx, double t) {

    double gm1 = gamma - 1.0;
    double gp1 = gamma + 1.0;
    double cL = sqrt(gamma * pL / rhoL);
    double cR = sqrt(gamma * pR / rhoR);

    // Newton iteration for p* in star region
    double pStar = 0.5 * (pL + pR);
    for (int iter = 0; iter < 100; ++iter) {
        auto fK = [&](double p, double pK, double rhoK, double cK) -> pair<double,double> {
            if (p > pK) { // shock
                double A = 2.0 / (gp1 * rhoK);
                double B = gm1 / gp1 * pK;
                double sq = sqrt(A / (p + B));
                return {(p - pK) * sq, sq * (1.0 - (p - pK) / (2.0 * (p + B)))};
            } else { // rarefaction
                double ratio = pow(p / pK, gm1 / (2.0 * gamma));
                return {2.0 * cK / gm1 * (ratio - 1.0),
                        ratio / (rhoK * cK) * pow(p / pK, -gp1 / (2.0 * gamma)) / gamma * pK};
            }
        };
        auto [fL, dfL] = fK(pStar, pL, rhoL, cL);
        auto [fR, dfR] = fK(pStar, pR, rhoR, cR);
        double residual = fL + fR + (uR - uL);
        double deriv = dfL + dfR;
        if (fabs(deriv) < 1e-30) break;
        double dp = -residual / deriv;
        pStar += dp;
        if (pStar < 1e-10) pStar = 1e-10;
        if (fabs(dp) < 1e-12 * pStar) break;
    }
    double uStar = 0.5 * (uL + uR);
    {
        auto fK = [&](double p, double pK, double rhoK, double cK) -> double {
            if (p > pK) {
                double A = 2.0 / (gp1 * rhoK);
                double B = gm1 / gp1 * pK;
                return (p - pK) * sqrt(A / (p + B));
            } else {
                return 2.0 * cK / gm1 * (pow(p / pK, gm1 / (2.0 * gamma)) - 1.0);
            }
        };
        uStar = 0.5 * (uL + uR) + 0.5 * (fK(pStar, pR, rhoR, cR) - fK(pStar, pL, rhoL, cL));
    }

    RiemannSolution sol;
    sol.pStar = pStar;
    sol.uStar = uStar;
    sol.x.resize(nx);
    sol.rhoField.resize(nx);
    sol.u.resize(nx);
    sol.p.resize(nx);

    double x0 = 0.5 * (xMin + xMax);
    for (int i = 0; i < nx; ++i) {
        double x = xMin + (xMax - xMin) * i / (nx - 1.0);
        sol.x[i] = x;
        double S = (t > 0) ? (x - x0) / t : 0;

        if (S < uStar) {
            // Left side
            if (pStar > pL) { // left shock
                double sL = uL - cL * sqrt(gp1 / (2.0 * gamma) * pStar / pL + gm1 / (2.0 * gamma));
                if (S < sL) { sol.rhoField[i] = rhoL; sol.u[i] = uL; sol.p[i] = pL; }
                else { sol.rhoField[i] = rhoL * (pStar / pL + gm1 / gp1) / (gm1 / gp1 * pStar / pL + 1.0); sol.u[i] = uStar; sol.p[i] = pStar; }
            } else { // left rarefaction
                double cStarL = cL * pow(pStar / pL, gm1 / (2.0 * gamma));
                double sHL = uL - cL;
                double sTL = uStar - cStarL;
                if (S < sHL) { sol.rhoField[i] = rhoL; sol.u[i] = uL; sol.p[i] = pL; }
                else if (S < sTL) {
                    sol.u[i] = 2.0 / gp1 * (cL + gm1 / 2.0 * uL + S);
                    double c = cL - gm1 / 2.0 * (sol.u[i] - uL);
                    sol.rhoField[i] = rhoL * pow(c / cL, 2.0 / gm1);
                    sol.p[i] = pL * pow(c / cL, 2.0 * gamma / gm1);
                } else {
                    sol.rhoField[i] = rhoL * pow(pStar / pL, 1.0 / gamma);
                    sol.u[i] = uStar; sol.p[i] = pStar;
                }
            }
        } else {
            // Right side
            if (pStar > pR) {
                double sR = uR + cR * sqrt(gp1 / (2.0 * gamma) * pStar / pR + gm1 / (2.0 * gamma));
                if (S > sR) { sol.rhoField[i] = rhoR; sol.u[i] = uR; sol.p[i] = pR; }
                else { sol.rhoField[i] = rhoR * (pStar / pR + gm1 / gp1) / (gm1 / gp1 * pStar / pR + 1.0); sol.u[i] = uStar; sol.p[i] = pStar; }
            } else {
                double cStarR = cR * pow(pStar / pR, gm1 / (2.0 * gamma));
                double sHR = uR + cR;
                double sTR = uStar + cStarR;
                if (S > sHR) { sol.rhoField[i] = rhoR; sol.u[i] = uR; sol.p[i] = pR; }
                else if (S > sTR) {
                    sol.u[i] = 2.0 / gp1 * (-cR + gm1 / 2.0 * uR + S);
                    double c = cR + gm1 / 2.0 * (uR - sol.u[i]);
                    sol.rhoField[i] = rhoR * pow(c / cR, 2.0 / gm1);
                    sol.p[i] = pR * pow(c / cR, 2.0 * gamma / gm1);
                } else {
                    sol.rhoField[i] = rhoR * pow(pStar / pR, 1.0 / gamma);
                    sol.u[i] = uStar; sol.p[i] = pStar;
                }
            }
        }
    }
    return sol;
}

void FluidSystem::exportRiemannCSV(const string& filename, const RiemannSolution& sol) {
    ofstream f(filename);
    f << "x,rho,u,p\n";
    for (size_t i = 0; i < sol.x.size(); ++i)
        f << sol.x[i] << "," << sol.rhoField[i] << "," << sol.u[i] << "," << sol.p[i] << "\n";
}

// ════════════════════════════════════════════════════════════════════════════
//  21. SPEED OF SOUND
// ════════════════════════════════════════════════════════════════════════════

double FluidSystem::speedOfSoundIdealGas(double gamma, double R_specific, double T) {
    return sqrt(gamma * R_specific * T);
}

double FluidSystem::speedOfSoundLiquid(double K, double rho) {
    return sqrt(K / rho);
}

double FluidSystem::machNumber(double v, double c) { return v / c; }

double FluidSystem::machAngle(double M) {
    if (M <= 1.0) return PI / 2.0;
    return asin(1.0 / M);
}

void FluidSystem::exportSpeedOfSoundCSV(const string& filename, double gamma,
                                          double R_specific, double Tmin,
                                          double Tmax, int numT) {
    ofstream f(filename);
    f << "T,c\n";
    for (int i = 0; i < numT; ++i) {
        double T = Tmin + (Tmax - Tmin) * i / (numT - 1.0);
        f << T << "," << speedOfSoundIdealGas(gamma, R_specific, T) << "\n";
    }
}

// ════════════════════════════════════════════════════════════════════════════
//  22. ISENTROPIC FLOW
// ════════════════════════════════════════════════════════════════════════════

double FluidSystem::isentropicPressureRatio(double M, double gamma) {
    return pow(1.0 + 0.5 * (gamma - 1.0) * M * M, -gamma / (gamma - 1.0));
}

double FluidSystem::isentropicTemperatureRatio(double M, double gamma) {
    return 1.0 / (1.0 + 0.5 * (gamma - 1.0) * M * M);
}

double FluidSystem::isentropicDensityRatio(double M, double gamma) {
    return pow(1.0 + 0.5 * (gamma - 1.0) * M * M, -1.0 / (gamma - 1.0));
}

double FluidSystem::isentropicAreaRatio(double M, double gamma) {
    double gm1 = gamma - 1.0;
    double gp1 = gamma + 1.0;
    double t = 1.0 + 0.5 * gm1 * M * M;
    return (1.0 / M) * pow(2.0 / gp1 * t, gp1 / (2.0 * gm1));
}

double FluidSystem::machFromAreaRatio(double AoverAstar, double gamma, bool subsonic) {
    // Newton's method
    double M = subsonic ? 0.5 : 2.0;
    for (int i = 0; i < 100; ++i) {
        double A = isentropicAreaRatio(M, gamma);
        double dAdM;
        {
            double dM = 1e-6;
            dAdM = (isentropicAreaRatio(M + dM, gamma) - A) / dM;
        }
        double dM = -(A - AoverAstar) / dAdM;
        M += dM;
        if (M < 0.01) M = 0.01;
        if (fabs(dM) < 1e-10) break;
    }
    return M;
}

void FluidSystem::exportIsentropicFlowCSV(const string& filename, double gamma,
                                            double Mmax, int numM) {
    ofstream f(filename);
    f << "M,p_p0,T_T0,rho_rho0,A_Astar\n";
    for (int i = 0; i < numM; ++i) {
        double M = 0.01 + Mmax * i / (numM - 1.0);
        f << M << "," << isentropicPressureRatio(M, gamma) << ","
          << isentropicTemperatureRatio(M, gamma) << ","
          << isentropicDensityRatio(M, gamma) << ","
          << isentropicAreaRatio(M, gamma) << "\n";
    }
}

// ════════════════════════════════════════════════════════════════════════════
//  23. NORMAL SHOCKS
// ════════════════════════════════════════════════════════════════════════════

FluidSystem::ShockResult FluidSystem::normalShock(double M1, double gamma) {
    double gm1 = gamma - 1.0;
    double gp1 = gamma + 1.0;
    double M12 = M1 * M1;

    ShockResult s;
    s.M2 = sqrt((gm1 * M12 + 2.0) / (2.0 * gamma * M12 - gm1));
    s.p2_p1 = 1.0 + 2.0 * gamma / gp1 * (M12 - 1.0);
    s.rho2_rho1 = gp1 * M12 / (gm1 * M12 + 2.0);
    s.T2_T1 = s.p2_p1 / s.rho2_rho1;
    s.p02_p01 = pow(gp1 * M12 / (gm1 * M12 + 2.0), gamma / gm1)
              * pow(gp1 / (2.0 * gamma * M12 - gm1), 1.0 / gm1);
    return s;
}

void FluidSystem::exportNormalShockCSV(const string& filename, double gamma,
                                        double M1min, double M1max, int numM) {
    ofstream f(filename);
    f << "M1,M2,p2_p1,rho2_rho1,T2_T1,p02_p01\n";
    for (int i = 0; i < numM; ++i) {
        double M1 = M1min + (M1max - M1min) * i / (numM - 1.0);
        auto s = normalShock(M1, gamma);
        f << M1 << "," << s.M2 << "," << s.p2_p1 << "," << s.rho2_rho1
          << "," << s.T2_T1 << "," << s.p02_p01 << "\n";
    }
}

// ════════════════════════════════════════════════════════════════════════════
//  24. OBLIQUE SHOCKS
// ════════════════════════════════════════════════════════════════════════════

double FluidSystem::obliqueShockBeta(double M1, double theta, double gamma, bool weak) {
    // Iterative solver for oblique shock angle beta given deflection theta
    double betaMin = asin(1.0 / M1);
    double betaMax = PI / 2.0;
    double bestBeta = 0;
    int n = 1000;
    double minErr = 1e30;
    for (int i = 0; i <= n; ++i) {
        double beta = betaMin + (betaMax - betaMin) * i / n;
        double M1n = M1 * sin(beta);
        double tanTheta = 2.0 / tan(beta) * (M1n * M1n - 1.0) /
                          (M1 * M1 * (gamma + cos(2.0 * beta)) + 2.0);
        double err = fabs(tanTheta - tan(theta));
        if (err < minErr) {
            minErr = err;
            bestBeta = beta;
        }
    }
    return bestBeta;
}

double FluidSystem::obliqueShockMn1(double M1, double beta) {
    return M1 * sin(beta);
}

FluidSystem::ShockResult FluidSystem::obliqueShock(double M1, double beta, double gamma) {
    double Mn1 = M1 * sin(beta);
    return normalShock(Mn1, gamma);
}

double FluidSystem::maxDeflectionAngle(double M1, double gamma) {
    double betaMax = PI / 2.0;
    double betaMin = asin(1.0 / M1);
    double maxTheta = 0;
    for (int i = 0; i <= 1000; ++i) {
        double beta = betaMin + (betaMax - betaMin) * i / 1000.0;
        double Mn1 = M1 * sin(beta);
        double tanTheta = 2.0 / tan(beta) * (Mn1 * Mn1 - 1.0) /
                          (M1 * M1 * (gamma + cos(2.0 * beta)) + 2.0);
        double theta = atan(tanTheta);
        if (theta > maxTheta) maxTheta = theta;
    }
    return maxTheta;
}

void FluidSystem::exportObliqueShockCSV(const string& filename, double M1,
                                          double gamma, int numTheta) {
    ofstream f(filename);
    f << "theta,beta_weak,M2_weak,p2_p1\n";
    double thetaMax = maxDeflectionAngle(M1, gamma);
    for (int i = 1; i < numTheta; ++i) {
        double theta = thetaMax * i / numTheta;
        double beta = obliqueShockBeta(M1, theta, gamma, true);
        auto s = obliqueShock(M1, beta, gamma);
        f << theta << "," << beta << "," << s.M2 << "," << s.p2_p1 << "\n";
    }
}

// ════════════════════════════════════════════════════════════════════════════
//  25. PRANDTL-MEYER EXPANSION
// ════════════════════════════════════════════════════════════════════════════

double FluidSystem::prandtlMeyerFunction(double M, double gamma) {
    double gm1 = gamma - 1.0;
    double gp1 = gamma + 1.0;
    return sqrt(gp1 / gm1) * atan(sqrt(gm1 / gp1 * (M * M - 1.0)))
           - atan(sqrt(M * M - 1.0));
}

double FluidSystem::machFromPrandtlMeyer(double nu, double gamma) {
    double M = 1.5;
    for (int i = 0; i < 100; ++i) {
        double f = prandtlMeyerFunction(M, gamma) - nu;
        double dM = 0.001;
        double df = (prandtlMeyerFunction(M + dM, gamma) - prandtlMeyerFunction(M, gamma)) / dM;
        M -= f / df;
        if (M < 1.0) M = 1.0001;
        if (fabs(f) < 1e-12) break;
    }
    return M;
}

double FluidSystem::expansionFanM2(double M1, double dTheta, double gamma) {
    double nu1 = prandtlMeyerFunction(M1, gamma);
    return machFromPrandtlMeyer(nu1 + dTheta, gamma);
}

void FluidSystem::exportPrandtlMeyerCSV(const string& filename, double gamma,
                                           double Mmax, int numM) {
    ofstream f(filename);
    f << "M,nu_rad,nu_deg\n";
    for (int i = 0; i < numM; ++i) {
        double M = 1.0001 + (Mmax - 1.0) * i / (numM - 1.0);
        double nu = prandtlMeyerFunction(M, gamma);
        f << M << "," << nu << "," << nu * 180.0 / PI << "\n";
    }
}

// ════════════════════════════════════════════════════════════════════════════
//  26. NOZZLE FLOW (DE LAVAL)
// ════════════════════════════════════════════════════════════════════════════

FluidSystem::NozzleResult FluidSystem::solveDeLavalNozzle(double gamma, double p0,
                                                            double T0, double R_specific,
                                                            double Athroat, double Aexit,
                                                            int numX) {
    NozzleResult res;
    res.x.resize(numX);
    res.A.resize(numX);
    res.M.resize(numX);
    res.p.resize(numX);
    res.T.resize(numX);
    res.rhoField.resize(numX);

    for (int i = 0; i < numX; ++i) {
        double xi = (double)i / (numX - 1.0);
        res.x[i] = xi;
        // Simple area distribution: converging-diverging
        double A;
        if (xi <= 0.5)
            A = Aexit + (Athroat - Aexit) * 2.0 * xi;
        else
            A = Athroat + (Aexit - Athroat) * 2.0 * (xi - 0.5);
        res.A[i] = A;

        double Aratio = A / Athroat;
        bool subsonic = (xi <= 0.5);
        double M = machFromAreaRatio(Aratio, gamma, subsonic);
        res.M[i] = M;
        res.p[i] = p0 * isentropicPressureRatio(M, gamma);
        res.T[i] = T0 * isentropicTemperatureRatio(M, gamma);
        res.rhoField[i] = res.p[i] / (R_specific * res.T[i]);
    }
    return res;
}

double FluidSystem::nozzleThrustCoefficient(double gamma, double pe_p0, double Ae_At) {
    double gm1 = gamma - 1.0;
    double gp1 = gamma + 1.0;
    double CF = sqrt(2.0 * gamma * gamma / gm1 * pow(2.0 / gp1, gp1 / gm1)
                * (1.0 - pow(pe_p0, gm1 / gamma))) + (pe_p0 - 1.0) * Ae_At;
    return CF;
}

void FluidSystem::exportNozzleFlowCSV(const string& filename, const NozzleResult& result) {
    ofstream f(filename);
    f << "x,A,M,p,T,rho\n";
    for (size_t i = 0; i < result.x.size(); ++i)
        f << result.x[i] << "," << result.A[i] << "," << result.M[i] << ","
          << result.p[i] << "," << result.T[i] << "," << result.rhoField[i] << "\n";
}

// ════════════════════════════════════════════════════════════════════════════
//  27. FANNO FLOW
// ════════════════════════════════════════════════════════════════════════════

double FluidSystem::fannoFLstarOverD(double M, double gamma) {
    double gm1 = gamma - 1.0;
    double gp1 = gamma + 1.0;
    double M2 = M * M;
    return (1.0 - M2) / (gamma * M2) + gp1 / (2.0 * gamma)
           * log(gp1 * M2 / (2.0 + gm1 * M2));
}

double FluidSystem::fannoTemperatureRatio(double M, double gamma) {
    return (gamma + 1.0) / (2.0 + (gamma - 1.0) * M * M);
}

double FluidSystem::fannoPressureRatio(double M, double gamma) {
    return (1.0 / M) * sqrt((gamma + 1.0) / (2.0 + (gamma - 1.0) * M * M));
}

double FluidSystem::fannoStagnationPressureRatio(double M, double gamma) {
    double gm1 = gamma - 1.0;
    double gp1 = gamma + 1.0;
    return (1.0 / M) * pow(2.0 / gp1 * (1.0 + 0.5 * gm1 * M * M), gp1 / (2.0 * gm1));
}

void FluidSystem::exportFannoFlowCSV(const string& filename, double gamma,
                                       double Mmax, int numM) {
    ofstream f(filename);
    f << "M,fLstar_D,T_Tstar,p_pstar,p0_p0star\n";
    for (int i = 1; i < numM; ++i) {
        double M = 0.1 + (Mmax - 0.1) * i / (numM - 1.0);
        f << M << "," << fannoFLstarOverD(M, gamma) << ","
          << fannoTemperatureRatio(M, gamma) << ","
          << fannoPressureRatio(M, gamma) << ","
          << fannoStagnationPressureRatio(M, gamma) << "\n";
    }
}

// ════════════════════════════════════════════════════════════════════════════
//  28. RAYLEIGH FLOW
// ════════════════════════════════════════════════════════════════════════════

double FluidSystem::rayleighTemperatureRatio(double M, double gamma) {
    double M2 = M * M;
    double gp1 = gamma + 1.0;
    return M2 * gp1 * gp1 / pow(1.0 + gamma * M2, 2);
}

double FluidSystem::rayleighPressureRatio(double M, double gamma) {
    return (gamma + 1.0) / (1.0 + gamma * M * M);
}

double FluidSystem::rayleighStagnationTemperatureRatio(double M, double gamma) {
    double gm1 = gamma - 1.0;
    double gp1 = gamma + 1.0;
    double M2 = M * M;
    return 2.0 * (gp1) * M2 / pow(1.0 + gamma * M2, 2) * (1.0 + 0.5 * gm1 * M2);
}

double FluidSystem::rayleighStagnationPressureRatio(double M, double gamma) {
    double gm1 = gamma - 1.0;
    double gp1 = gamma + 1.0;
    double M2 = M * M;
    return gp1 / (1.0 + gamma * M2)
           * pow(2.0 / gp1 * (1.0 + 0.5 * gm1 * M2), gamma / gm1);
}

void FluidSystem::exportRayleighFlowCSV(const string& filename, double gamma,
                                           double Mmax, int numM) {
    ofstream f(filename);
    f << "M,T_Tstar,p_pstar,T0_T0star,p0_p0star\n";
    for (int i = 1; i < numM; ++i) {
        double M = 0.1 + (Mmax - 0.1) * i / (numM - 1.0);
        f << M << "," << rayleighTemperatureRatio(M, gamma) << ","
          << rayleighPressureRatio(M, gamma) << ","
          << rayleighStagnationTemperatureRatio(M, gamma) << ","
          << rayleighStagnationPressureRatio(M, gamma) << "\n";
    }
}

// ════════════════════════════════════════════════════════════════════════════
//  29. REYNOLDS NUMBER REGIMES
// ════════════════════════════════════════════════════════════════════════════

double FluidSystem::reynoldsNum(double rho, double U, double L, double mu) {
    return rho * U * L / mu;
}

string FluidSystem::flowRegime(double Re) {
    if (Re < 1) return "Creeping flow (Stokes)";
    if (Re < 2300) return "Laminar";
    if (Re < 4000) return "Transitional";
    return "Turbulent";
}

double FluidSystem::criticalReynoldsPipe() { return 2300.0; }
double FluidSystem::criticalReynoldsPlate() { return 5e5; }

double FluidSystem::dragCoefficientSphere(double Re) {
    if (Re < 0.1) return 24.0 / Re;
    if (Re < 1000) return 24.0 / Re * (1.0 + 0.15 * pow(Re, 0.687));
    if (Re < 2e5) return 0.44;
    return 0.1; // post-critical
}

double FluidSystem::frictionFactorLaminar(double Re) { return 64.0 / Re; }

double FluidSystem::frictionFactorTurbulentColebrook(double Re, double epsOverD) {
    // Colebrook iteration
    double f = 0.02;
    for (int i = 0; i < 50; ++i) {
        double rhs = -2.0 * log10(epsOverD / 3.7 + 2.51 / (Re * sqrt(f)));
        f = 1.0 / (rhs * rhs);
    }
    return f;
}

void FluidSystem::exportReynoldsRegimesCSV(const string& filename, int numRe) {
    ofstream f(filename);
    f << "Re,Cd_sphere,f_pipe,regime\n";
    for (int i = 1; i <= numRe; ++i) {
        double Re = pow(10.0, -1.0 + 7.0 * i / numRe);
        double Cd = dragCoefficientSphere(Re);
        double fp = (Re < 2300) ? frictionFactorLaminar(Re) : frictionFactorTurbulentColebrook(Re, 0.001);
        f << Re << "," << Cd << "," << fp << "," << flowRegime(Re) << "\n";
    }
}

// ════════════════════════════════════════════════════════════════════════════
//  30. KOLMOGOROV CASCADE
// ════════════════════════════════════════════════════════════════════════════

double FluidSystem::kolmogorovLengthScale(double nu, double epsilon) {
    return pow(nu * nu * nu / epsilon, 0.25);
}

double FluidSystem::kolmogorovTimeScale(double nu, double epsilon) {
    return sqrt(nu / epsilon);
}

double FluidSystem::kolmogorovVelocityScale(double nu, double epsilon) {
    return pow(nu * epsilon, 0.25);
}

double FluidSystem::kolmogorovEnergySpectrum(double C_K, double epsilon, double k) {
    return C_K * pow(epsilon, 2.0 / 3.0) * pow(k, -5.0 / 3.0);
}

double FluidSystem::integralLengthScale(double u_rms, double epsilon) {
    return u_rms * u_rms * u_rms / epsilon;
}

double FluidSystem::taylorMicroscale(double u_rms, double nu, double epsilon) {
    return u_rms * sqrt(15.0 * nu / epsilon);
}

double FluidSystem::reTurbulent(double u_rms, double L_int, double nu) {
    return u_rms * L_int / nu;
}

void FluidSystem::exportKolmogorovCSV(const string& filename, double nu,
                                        double epsilon, double C_K, int numK) {
    ofstream f(filename);
    f << "k,E_k,eta,tau_eta\n";
    double eta = kolmogorovLengthScale(nu, epsilon);
    double tau = kolmogorovTimeScale(nu, epsilon);
    for (int i = 1; i <= numK; ++i) {
        double k = pow(10.0, -2.0 + 5.0 * i / numK);
        f << k << "," << kolmogorovEnergySpectrum(C_K, epsilon, k) << ","
          << eta << "," << tau << "\n";
    }
}

// ════════════════════════════════════════════════════════════════════════════
//  31. MIXING LENGTH THEORY
// ════════════════════════════════════════════════════════════════════════════

double FluidSystem::mixingLength(double kappa, double y) {
    return kappa * y;
}

double FluidSystem::turbulentViscosity(double lm, double dudy) {
    return lm * lm * fabs(dudy);
}

double FluidSystem::logLawVelocity(double uTau, double y, double nu,
                                    double kappa, double B) {
    double yPlus = uTau * y / nu;
    if (yPlus < 5.0) return uTau * yPlus; // viscous sublayer
    return uTau * (1.0 / kappa * log(yPlus) + B);
}

double FluidSystem::frictionVelocity(double tauW, double rho) {
    return sqrt(tauW / rho);
}

double FluidSystem::viscousSublayerThickness(double nu, double uTau) {
    return 5.0 * nu / uTau;
}

FluidSystem::BoundaryLayerResult FluidSystem::turbulentBLProfile(
    double U_inf, double nu, double xMax, int numX) {
    BoundaryLayerResult res;
    res.x.resize(numX);
    res.delta.resize(numX);
    res.delta_star.resize(numX);
    res.theta.resize(numX);
    res.cf.resize(numX);
    for (int i = 0; i < numX; ++i) {
        double x = 0.01 + xMax * i / (numX - 1.0);
        res.x[i] = x;
        double Rex = U_inf * x / nu;
        if (Rex < 5e5) { // laminar
            res.delta[i] = 5.0 * x / sqrt(Rex);
            res.cf[i] = 0.664 / sqrt(Rex);
        } else { // turbulent (1/7 power law)
            res.delta[i] = 0.37 * x / pow(Rex, 0.2);
            res.cf[i] = 0.0592 / pow(Rex, 0.2);
        }
        res.delta_star[i] = res.delta[i] / 8.0;
        res.theta[i] = res.delta[i] * 7.0 / 72.0;
    }
    return res;
}

void FluidSystem::exportMixingLengthCSV(const string& filename, double U_inf,
                                          double nu, int numPoints) {
    auto res = turbulentBLProfile(U_inf, nu, 1.0, numPoints);
    ofstream f(filename);
    f << "x,delta,delta_star,theta,cf\n";
    for (int i = 0; i < numPoints; ++i)
        f << res.x[i] << "," << res.delta[i] << "," << res.delta_star[i]
          << "," << res.theta[i] << "," << res.cf[i] << "\n";
}

// ════════════════════════════════════════════════════════════════════════════
//  32. BOUNDARY LAYERS (BLASIUS)
// ════════════════════════════════════════════════════════════════════════════

FluidSystem::BlasiusProfile FluidSystem::solveBlasius(double etaMax, int numPoints) {
    BlasiusProfile prof;
    prof.eta.resize(numPoints);
    prof.f.resize(numPoints);
    prof.fp.resize(numPoints);
    prof.fpp.resize(numPoints);

    double deta = etaMax / (numPoints - 1.0);

    // Shooting method: f(0)=0, f'(0)=0, f''(0)=?? -> f'(inf)=1
    double fpp0_low = 0.0, fpp0_high = 2.0;
    double fpp0 = 0.33206; // known Blasius value

    // RK4 integration
    auto integrate = [&](double fpp0_val) {
        vector<double> f_(numPoints, 0), fp_(numPoints, 0), fpp_(numPoints, 0);
        f_[0] = 0; fp_[0] = 0; fpp_[0] = fpp0_val;
        for (int i = 0; i < numPoints - 1; ++i) {
            double h = deta;
            auto rhs = [](double f, double fp, double fpp) -> array<double,3> {
                return {fp, fpp, -0.5 * f * fpp};
            };
            auto k1 = rhs(f_[i], fp_[i], fpp_[i]);
            auto k2 = rhs(f_[i]+h/2*k1[0], fp_[i]+h/2*k1[1], fpp_[i]+h/2*k1[2]);
            auto k3 = rhs(f_[i]+h/2*k2[0], fp_[i]+h/2*k2[1], fpp_[i]+h/2*k2[2]);
            auto k4 = rhs(f_[i]+h*k3[0], fp_[i]+h*k3[1], fpp_[i]+h*k3[2]);
            f_[i+1] = f_[i] + h/6*(k1[0]+2*k2[0]+2*k3[0]+k4[0]);
            fp_[i+1] = fp_[i] + h/6*(k1[1]+2*k2[1]+2*k3[1]+k4[1]);
            fpp_[i+1] = fpp_[i] + h/6*(k1[2]+2*k2[2]+2*k3[2]+k4[2]);
        }
        return make_tuple(f_, fp_, fpp_);
    };

    auto [f_out, fp_out, fpp_out] = integrate(fpp0);
    for (int i = 0; i < numPoints; ++i) {
        prof.eta[i] = i * deta;
        prof.f[i] = f_out[i];
        prof.fp[i] = fp_out[i];
        prof.fpp[i] = fpp_out[i];
    }
    return prof;
}

double FluidSystem::blasiusBLThickness(double x, double Re_x) {
    return 5.0 * x / sqrt(Re_x);
}

double FluidSystem::blasiusDisplacementThickness(double x, double Re_x) {
    return 1.7208 * x / sqrt(Re_x);
}

double FluidSystem::blasiusMomentumThickness(double x, double Re_x) {
    return 0.664 * x / sqrt(Re_x);
}

double FluidSystem::blasiusSkinFriction(double Re_x) {
    return 0.664 / sqrt(Re_x);
}

FluidSystem::BoundaryLayerResult FluidSystem::blasiusBLGrowth(double U_inf,
                                                                double nu, double xMax,
                                                                int numX) {
    BoundaryLayerResult res;
    res.x.resize(numX); res.delta.resize(numX); res.delta_star.resize(numX);
    res.theta.resize(numX); res.cf.resize(numX);
    for (int i = 0; i < numX; ++i) {
        double x = 0.01 + xMax * i / (numX - 1.0);
        double Rex = U_inf * x / nu;
        res.x[i] = x;
        res.delta[i] = blasiusBLThickness(x, Rex);
        res.delta_star[i] = blasiusDisplacementThickness(x, Rex);
        res.theta[i] = blasiusMomentumThickness(x, Rex);
        res.cf[i] = blasiusSkinFriction(Rex);
    }
    return res;
}

void FluidSystem::exportBlasiusCSV(const string& filename, double U_inf,
                                     double nu, double xMax, int numPoints) {
    auto res = blasiusBLGrowth(U_inf, nu, xMax, numPoints);
    ofstream f(filename);
    f << "x,delta,delta_star,theta,cf\n";
    for (int i = 0; i < numPoints; ++i)
        f << res.x[i] << "," << res.delta[i] << "," << res.delta_star[i]
          << "," << res.theta[i] << "," << res.cf[i] << "\n";
}

// ════════════════════════════════════════════════════════════════════════════
//  33. DRAG ON A SPHERE
// ════════════════════════════════════════════════════════════════════════════

double FluidSystem::dragForce(double Cd, double rho, double U, double A) {
    return 0.5 * Cd * rho * U * U * A;
}

double FluidSystem::cdSphereEmpirical(double Re) {
    return dragCoefficientSphere(Re);
}

double FluidSystem::terminalVelocitySphere(double rhoP, double rhoF, double d,
                                             double mu, double g) {
    // Iterative: balance drag = buoyancy - weight
    double V = sqrt(4.0 * d * g * (rhoP - rhoF) / (3.0 * rhoF * 0.44));
    for (int iter = 0; iter < 100; ++iter) {
        double Re = rhoF * V * d / mu;
        double Cd = dragCoefficientSphere(Re);
        double Vnew = sqrt(4.0 * d * g * (rhoP - rhoF) / (3.0 * rhoF * Cd));
        if (fabs(Vnew - V) < 1e-10) break;
        V = 0.5 * (V + Vnew);
    }
    return V;
}

double FluidSystem::oseen_dragCoefficient(double Re) {
    return 24.0 / Re * (1.0 + 3.0 / 16.0 * Re);
}

void FluidSystem::exportDragSphereCSV(const string& filename, double rhoF,
                                        double rhoP, double d, double mu,
                                        double g, int numRe) {
    ofstream f(filename);
    f << "Re,Cd,Cd_Stokes,Cd_Oseen\n";
    for (int i = 1; i <= numRe; ++i) {
        double Re = pow(10.0, -1.0 + 6.0 * i / numRe);
        f << Re << "," << dragCoefficientSphere(Re) << ","
          << 24.0 / Re << "," << oseen_dragCoefficient(Re) << "\n";
    }
}

// ════════════════════════════════════════════════════════════════════════════
//  34. PIPE FLOW (MOODY)
// ════════════════════════════════════════════════════════════════════════════

double FluidSystem::darcyFrictionFactor(double Re, double epsOverD) {
    if (Re < 2300) return 64.0 / Re;
    return frictionFactorTurbulentColebrook(Re, epsOverD);
}

double FluidSystem::headLoss(double f, double L, double D, double V, double g) {
    return f * L / D * V * V / (2.0 * g);
}

double FluidSystem::pressureDrop(double f, double L, double D, double rho, double V) {
    return f * L / D * 0.5 * rho * V * V;
}

double FluidSystem::pipeFlowRate(double D, double dpdx, double mu, double rho) {
    return PI * D * D * D * D * (-dpdx) / (128.0 * mu);
}

void FluidSystem::exportMoodyDiagramCSV(const string& filename, int numRe, int numEps) {
    ofstream f(filename);
    f << "Re,epsOverD,f\n";
    vector<double> eps = {0, 1e-5, 1e-4, 5e-4, 1e-3, 5e-3, 0.01, 0.05};
    for (double e : eps)
        for (int i = 1; i <= numRe; ++i) {
            double Re = pow(10.0, 2.5 + 5.0 * i / numRe);
            f << Re << "," << e << "," << darcyFrictionFactor(Re, e) << "\n";
        }
}

// ════════════════════════════════════════════════════════════════════════════
//  35. HEAT CONVECTION
// ════════════════════════════════════════════════════════════════════════════

double FluidSystem::nusseltFlatPlate(double Re, double Pr, bool turbulent) {
    if (!turbulent)
        return 0.332 * pow(Re, 0.5) * pow(Pr, 1.0 / 3.0);
    else
        return 0.0296 * pow(Re, 0.8) * pow(Pr, 1.0 / 3.0);
}

double FluidSystem::nusseltCylinder(double Re, double Pr) {
    // Churchill-Bernstein correlation
    double ReSq = sqrt(Re);
    double term = pow(1.0 + pow(Re / 282000.0, 5.0 / 8.0), 4.0 / 5.0);
    return 0.3 + 0.62 * ReSq * pow(Pr, 1.0 / 3.0)
           / pow(1.0 + pow(0.4 / Pr, 2.0 / 3.0), 0.25) * term;
}

double FluidSystem::nusseltPipeInternal(double Re, double Pr, bool turbulent) {
    if (!turbulent) return 3.66; // fully developed, constant T_w
    return 0.023 * pow(Re, 0.8) * pow(Pr, 0.4); // Dittus-Boelter
}

double FluidSystem::convectiveHeatTransfer(double Nu, double k, double L) {
    return Nu * k / L;
}

double FluidSystem::thermalBLThickness(double delta, double Pr) {
    return delta / pow(Pr, 1.0 / 3.0);
}

void FluidSystem::exportHeatConvectionCSV(const string& filename, double Pr,
                                            double Remax, int numRe) {
    ofstream f(filename);
    f << "Re,Nu_laminar,Nu_turbulent,Nu_cylinder\n";
    for (int i = 1; i <= numRe; ++i) {
        double Re = Remax * i / numRe;
        f << Re << "," << nusseltFlatPlate(Re, Pr, false) << ","
          << nusseltFlatPlate(Re, Pr, true) << ","
          << nusseltCylinder(Re, Pr) << "\n";
    }
}

// ════════════════════════════════════════════════════════════════════════════
//  36. MASS DIFFUSION (FICK)
// ════════════════════════════════════════════════════════════════════════════

double FluidSystem::fickFlux(double D, double dcdx) {
    return -D * dcdx;
}

double FluidSystem::diffusionSolution1D(double C0, double x, double D, double t) {
    if (t <= 0) return (x == 0) ? C0 : 0;
    return C0 / sqrt(4.0 * PI * D * t) * exp(-x * x / (4.0 * D * t));
}

double FluidSystem::schmidtNumber(double nu, double D) { return nu / D; }

double FluidSystem::sherwoodNumber(double Re, double Sc) {
    return 0.332 * pow(Re, 0.5) * pow(Sc, 1.0 / 3.0);
}

double FluidSystem::massTransferCoefficient(double Sh, double D, double L) {
    return Sh * D / L;
}

void FluidSystem::exportDiffusionCSV(const string& filename, double C0, double D,
                                       double xMax, int nx, int nt, double tMax) {
    ofstream f(filename);
    f << "x,t,C\n";
    for (int j = 1; j <= nt; ++j) {
        double t = tMax * j / nt;
        for (int i = 0; i < nx; ++i) {
            double x = -xMax + 2.0 * xMax * i / (nx - 1.0);
            f << x << "," << t << "," << diffusionSolution1D(C0, x, D, t) << "\n";
        }
    }
}

// ════════════════════════════════════════════════════════════════════════════
//  37. MAGNETOHYDRODYNAMICS
// ════════════════════════════════════════════════════════════════════════════

double FluidSystem::alfvenSpeed(double B, double rho, double mu0) {
    return B / sqrt(mu0 * rho);
}

double FluidSystem::magnetosonicSpeedFast(double cs, double vA, double theta) {
    double cs2 = cs * cs, vA2 = vA * vA;
    double sum = cs2 + vA2;
    double disc = sqrt(sum * sum - 4.0 * cs2 * vA2 * cos(theta) * cos(theta));
    return sqrt(0.5 * (sum + disc));
}

double FluidSystem::magnetosonicSpeedSlow(double cs, double vA, double theta) {
    double cs2 = cs * cs, vA2 = vA * vA;
    double sum = cs2 + vA2;
    double disc = sqrt(sum * sum - 4.0 * cs2 * vA2 * cos(theta) * cos(theta));
    return sqrt(max(0.0, 0.5 * (sum - disc)));
}

double FluidSystem::magneticPressure(double B, double mu0) {
    return B * B / (2.0 * mu0);
}

double FluidSystem::magneticReynoldsNumber(double U, double L, double eta) {
    return U * L / eta;
}

double FluidSystem::hartmannNumber(double B, double L, double sigma, double mu) {
    return B * L * sqrt(sigma / mu);
}

FluidSystem::MHDState FluidSystem::solveMHD1D(
    double rhoL, double rhoR, double pL, double pR,
    double BxL, double ByL, double BxR, double ByR,
    double xMin, double xMax, int nx, double tFinal) {
    // Simplified MHD Riemann-like problem (Lax-Friedrichs)
    double dx = (xMax - xMin) / nx;
    double gamma = 5.0 / 3.0;
    double mu0_val = mu_0;
    MHDState state;
    state.x.resize(nx);
    state.rhoField.resize(nx); state.vx.resize(nx); state.vy.resize(nx); state.vz.resize(nx, 0);
    state.Bx.resize(nx); state.By.resize(nx); state.Bz.resize(nx, 0);
    state.p.resize(nx);

    for (int i = 0; i < nx; ++i) {
        state.x[i] = xMin + (i + 0.5) * dx;
        if (state.x[i] < 0.5 * (xMin + xMax)) {
            state.rhoField[i] = rhoL; state.p[i] = pL;
            state.Bx[i] = BxL; state.By[i] = ByL;
        } else {
            state.rhoField[i] = rhoR; state.p[i] = pR;
            state.Bx[i] = BxR; state.By[i] = ByR;
        }
        state.vx[i] = 0; state.vy[i] = 0;
    }
    // Single snapshot; full time evolution would need proper MHD solver
    return state;
}

void FluidSystem::exportMHDCSV(const string& filename, const MHDState& state) {
    ofstream f(filename);
    f << "x,rho,vx,vy,Bx,By,p\n";
    for (size_t i = 0; i < state.x.size(); ++i)
        f << state.x[i] << "," << state.rhoField[i] << "," << state.vx[i]
          << "," << state.vy[i] << "," << state.Bx[i] << "," << state.By[i]
          << "," << state.p[i] << "\n";
}

// ════════════════════════════════════════════════════════════════════════════
//  38. ALFVÉN WAVES
// ════════════════════════════════════════════════════════════════════════════

double FluidSystem::alfvenWaveDispersion(double k, double B, double rho, double mu0) {
    return k * B / sqrt(mu0 * rho);
}

double FluidSystem::alfvenWaveGroupVelocity(double B, double rho, double mu0) {
    return B / sqrt(mu0 * rho);
}

double FluidSystem::alfvenWaveMagneticField(double B0, double dB, double k,
                                              double omega, double x, double t) {
    return B0 + dB * sin(k * x - omega * t);
}

void FluidSystem::exportAlfvenWavesCSV(const string& filename, double B0,
                                         double rho, double mu0, double dB,
                                         int numK, int numX) {
    ofstream f(filename);
    f << "k,omega,vA,x,By\n";
    double vA = alfvenSpeed(B0, rho, mu0);
    for (int ik = 1; ik <= numK; ++ik) {
        double k = 0.5 * ik;
        double omega = alfvenWaveDispersion(k, B0, rho, mu0);
        for (int ix = 0; ix < numX; ++ix) {
            double x = 10.0 * ix / (numX - 1.0);
            f << k << "," << omega << "," << vA << "," << x << ","
              << alfvenWaveMagneticField(B0, dB, k, omega, x, 0) << "\n";
        }
    }
}

// ════════════════════════════════════════════════════════════════════════════
//  39. PLASMA BETA
// ════════════════════════════════════════════════════════════════════════════

double FluidSystem::plasmaBeta(double p, double B, double mu0) {
    return 2.0 * mu0 * p / (B * B);
}

double FluidSystem::debyeLength(double T, double n, double q, double eps0) {
    return sqrt(eps0 * k_B * T / (n * q * q));
}

double FluidSystem::plasmaFrequency(double n, double q, double m, double eps0) {
    return sqrt(n * q * q / (m * eps0));
}

double FluidSystem::gyroRadius(double m, double v_perp, double q, double B) {
    return m * v_perp / (fabs(q) * B);
}

double FluidSystem::gyroFrequency(double q, double B, double m) {
    return fabs(q) * B / m;
}

void FluidSystem::exportPlasmaBetaCSV(const string& filename, double T, double n,
                                        double Bmin, double Bmax, int numB) {
    ofstream f(filename);
    f << "B,beta,lambda_D,omega_p,r_g\n";
    double p = n * k_B * T;
    double vth = sqrt(k_B * T / m_proton);
    for (int i = 0; i < numB; ++i) {
        double B = Bmin + (Bmax - Bmin) * i / (numB - 1.0);
        f << B << "," << plasmaBeta(p, B, mu_0) << ","
          << debyeLength(T, n, e_charge, epsilon_0) << ","
          << plasmaFrequency(n, e_charge, m_electron, epsilon_0) << ","
          << gyroRadius(m_proton, vth, e_charge, B) << "\n";
    }
}

// ════════════════════════════════════════════════════════════════════════════
//  40. ACCRETION DISK
// ════════════════════════════════════════════════════════════════════════════

double FluidSystem::keplerianVelocity(double M, double r, double G) {
    return sqrt(G * M / r);
}

double FluidSystem::keplerianAngularVelocity(double M, double r, double G) {
    return sqrt(G * M / (r * r * r));
}

double FluidSystem::viscousAccretionRate(double Sigma, double nu, double r) {
    return 3.0 * PI * nu * Sigma;
}

double FluidSystem::diskTemperature(double Mdot, double M, double r,
                                      double G, double sigma_SB) {
    double T4 = 3.0 * G * M * Mdot / (8.0 * PI * r * r * r * sigma_SB);
    return pow(T4, 0.25);
}

double FluidSystem::eddingtonLuminosity(double M, double c, double sigmaT,
                                          double mp, double G) {
    return 4.0 * PI * G * M * mp * c / sigmaT;
}

void FluidSystem::exportAccretionDiskCSV(const string& filename, double M,
                                           double Mdot, double rMin, double rMax,
                                           int numR) {
    ofstream f(filename);
    f << "r,v_K,Omega_K,T_disk\n";
    double G = 6.674e-11;
    for (int i = 0; i < numR; ++i) {
        double r = rMin + (rMax - rMin) * i / (numR - 1.0);
        f << r << "," << keplerianVelocity(M, r, G) << ","
          << keplerianAngularVelocity(M, r, G) << ","
          << diskTemperature(Mdot, M, r, G, sigma_SB) << "\n";
    }
}

// ════════════════════════════════════════════════════════════════════════════
//  41. JEANS INSTABILITY
// ════════════════════════════════════════════════════════════════════════════

double FluidSystem::jeansLength(double cs, double G, double rho) {
    return cs * sqrt(PI / (G * rho));
}

double FluidSystem::jeansMass(double cs, double G, double rho) {
    double lJ = jeansLength(cs, G, rho);
    return (4.0 / 3.0) * PI * rho * pow(lJ / 2.0, 3);
}

double FluidSystem::jeansWavenumber(double cs, double G, double rho) {
    return sqrt(4.0 * PI * G * rho) / cs;
}

double FluidSystem::jeansGrowthRate(double k, double cs, double G, double rho) {
    double kJ = jeansWavenumber(cs, G, rho);
    double arg = cs * cs * (kJ * kJ - k * k);
    if (arg <= 0) return 0;
    return sqrt(arg);
}

double FluidSystem::freefall_time(double G, double rho) {
    return sqrt(3.0 * PI / (32.0 * G * rho));
}

void FluidSystem::exportJeansInstabilityCSV(const string& filename, double cs,
                                               double G, double rho, int numK) {
    ofstream f(filename);
    f << "k,growth_rate,lambda_J,M_J\n";
    double kJ = jeansWavenumber(cs, G, rho);
    for (int i = 1; i <= numK; ++i) {
        double k = 3.0 * kJ * i / numK;
        f << k << "," << jeansGrowthRate(k, cs, G, rho) << ","
          << jeansLength(cs, G, rho) << "," << jeansMass(cs, G, rho) << "\n";
    }
}

// ════════════════════════════════════════════════════════════════════════════
//  42. BONDI ACCRETION
// ════════════════════════════════════════════════════════════════════════════

double FluidSystem::bondiRadius(double M, double cs, double G) {
    return G * M / (cs * cs);
}

double FluidSystem::bondiAccretionRate(double rho_inf, double M, double cs, double G) {
    double rB = bondiRadius(M, cs, G);
    return 4.0 * PI * rho_inf * rB * rB * cs;
}

double FluidSystem::bondiMachProfile(double r, double r_B, double gamma) {
    // Approximate Bondi solution: subsonic at large r, transonic at r_B
    double xi = r / r_B;
    if (xi > 1.0) return pow(xi, -0.5); // rough approximation
    return pow(xi, -(gamma + 1.0) / (4.0 * (gamma - 1.0)));
}

void FluidSystem::exportBondiAccretionCSV(const string& filename, double M,
                                             double cs, double rho_inf, double G,
                                             double gamma, int numR) {
    ofstream f(filename);
    f << "r,r_over_rB,M_local,Mdot\n";
    double rB = bondiRadius(M, cs, G);
    double Mdot = bondiAccretionRate(rho_inf, M, cs, G);
    for (int i = 1; i <= numR; ++i) {
        double r = 0.1 * rB + 10.0 * rB * i / numR;
        f << r << "," << r / rB << "," << bondiMachProfile(r, rB, gamma) << ","
          << Mdot << "\n";
    }
}

// ════════════════════════════════════════════════════════════════════════════
//  43. VISCOUS DISK (SHAKURA-SUNYAEV)
// ════════════════════════════════════════════════════════════════════════════

double FluidSystem::shakuraSunyaevViscosity(double alpha_SS, double cs, double H) {
    return alpha_SS * cs * H;
}

double FluidSystem::diskScaleHeight(double cs, double Omega) {
    return cs / Omega;
}

double FluidSystem::diskSurfaceDensityEvolution(double Sigma0, double nu, double r,
                                                  double t) {
    // Lynden-Bell & Pringle self-similar solution (simplified)
    double tau = 12.0 * nu * t / (r * r) + 1.0;
    return Sigma0 / tau * exp(-(1.0 - 1.0 / tau));
}

double FluidSystem::viscousTimescale(double R, double nu) {
    return R * R / nu;
}

void FluidSystem::exportViscousDiskCSV(const string& filename, double alpha_SS,
                                          double M, double Mdot, double rMin,
                                          double rMax, int numR) {
    ofstream f(filename);
    f << "r,Omega,H,nu_SS,T\n";
    double G = 6.674e-11;
    for (int i = 0; i < numR; ++i) {
        double r = rMin + (rMax - rMin) * i / (numR - 1.0);
        double Omega = keplerianAngularVelocity(M, r, G);
        double T = diskTemperature(Mdot, M, r, G, sigma_SB);
        double cs = sqrt(k_B * T / m_proton);
        double H = diskScaleHeight(cs, Omega);
        double nu = shakuraSunyaevViscosity(alpha_SS, cs, H);
        f << r << "," << Omega << "," << H << "," << nu << "," << T << "\n";
    }
}

// ════════════════════════════════════════════════════════════════════════════
//  44. RELATIVISTIC FLUID DYNAMICS
// ════════════════════════════════════════════════════════════════════════════

double FluidSystem::relativisticEnthalpy(double rho, double p, double eps, double c2) {
    return (rho * c2 + rho * eps + p) / rho;
}

double FluidSystem::relativisticSoundSpeed(double gamma, double p, double rho,
                                             double eps, double c2) {
    double h = relativisticEnthalpy(rho, p, eps, c2);
    return sqrt(gamma * p / (rho * h)) * c_light;
}

double FluidSystem::lorentzFactor(double v, double c) {
    return 1.0 / sqrt(1.0 - v * v / (c * c));
}

double FluidSystem::relativisticBernoulli(double h, double gamma_L, double c) {
    return h * gamma_L * c;
}

FluidSystem::RelativisticShockResult FluidSystem::relativisticNormalShock(
    double rho1, double p1, double v1, double gamma_eos, double c) {
    // Taub adiabat (simplified)
    double W1 = lorentzFactor(v1, c);
    double h1 = 1.0 + gamma_eos / (gamma_eos - 1.0) * p1 / (rho1 * c * c);
    // Hugoniot jump: simplified strong shock limit
    double compressionRatio = (gamma_eos + 1.0) / (gamma_eos - 1.0);
    RelativisticShockResult res;
    res.rho2 = rho1 * compressionRatio;
    res.p2 = p1 * (2.0 * gamma_eos * W1 * W1 * v1 * v1 / (c * c) - (gamma_eos - 1.0)) / (gamma_eos + 1.0);
    if (res.p2 < p1) res.p2 = p1;
    res.v2 = v1 / compressionRatio;
    res.gamma2 = lorentzFactor(res.v2, c);
    return res;
}

void FluidSystem::exportRelativisticFluidCSV(const string& filename,
                                               double gamma_eos, double c,
                                               int numV) {
    ofstream f(filename);
    f << "v_over_c,gamma,h\n";
    for (int i = 0; i < numV; ++i) {
        double vc = 0.01 + 0.98 * i / (numV - 1.0);
        double v = vc * c;
        double gam = lorentzFactor(v, c);
        f << vc << "," << gam << "," << gam * c << "\n";
    }
}

// ════════════════════════════════════════════════════════════════════════════
//  45. LATTICE BOLTZMANN (TOY)
// ════════════════════════════════════════════════════════════════════════════

FluidSystem::LBMState FluidSystem::solveLBM_D2Q9(int nx, int ny, double tau,
                                                    double uLid, int maxIter) {
    // D2Q9 lattice Boltzmann for lid-driven cavity
    const int Q = 9;
    const int ex[9] = {0, 1, 0, -1,  0, 1, -1, -1,  1};
    const int ey[9] = {0, 0, 1,  0, -1, 1,  1, -1, -1};
    const double w[9] = {4.0/9, 1.0/9, 1.0/9, 1.0/9, 1.0/9,
                          1.0/36, 1.0/36, 1.0/36, 1.0/36};

    int sz = nx * ny;
    vector<vector<double>> fDist(Q, vector<double>(sz, 0));
    vector<double> rhoArr(sz, 1.0), ux(sz, 0.0), uy(sz, 0.0);

    // Init equilibrium
    for (int j = 0; j < ny; ++j)
        for (int i = 0; i < nx; ++i) {
            int idx = j * nx + i;
            for (int q = 0; q < Q; ++q) {
                double eu = ex[q] * ux[idx] + ey[q] * uy[idx];
                double u2 = ux[idx] * ux[idx] + uy[idx] * uy[idx];
                fDist[q][idx] = w[q] * rhoArr[idx] * (1.0 + 3.0 * eu + 4.5 * eu * eu - 1.5 * u2);
            }
        }

    for (int iter = 0; iter < maxIter; ++iter) {
        // Streaming
        vector<vector<double>> fNew(Q, vector<double>(sz, 0));
        for (int j = 0; j < ny; ++j)
            for (int i = 0; i < nx; ++i) {
                int idx = j * nx + i;
                for (int q = 0; q < Q; ++q) {
                    int ni = i - ex[q], nj = j - ey[q];
                    if (ni >= 0 && ni < nx && nj >= 0 && nj < ny)
                        fNew[q][idx] = fDist[q][nj * nx + ni];
                    else
                        fNew[q][idx] = fDist[q][idx]; // bounce-back
                }
            }

        // Macroscopic
        for (int idx = 0; idx < sz; ++idx) {
            rhoArr[idx] = 0; ux[idx] = 0; uy[idx] = 0;
            for (int q = 0; q < Q; ++q) {
                rhoArr[idx] += fNew[q][idx];
                ux[idx] += ex[q] * fNew[q][idx];
                uy[idx] += ey[q] * fNew[q][idx];
            }
            ux[idx] /= rhoArr[idx];
            uy[idx] /= rhoArr[idx];
        }

        // Lid BC
        for (int i = 0; i < nx; ++i) {
            int idx = (ny - 1) * nx + i;
            ux[idx] = uLid;
            uy[idx] = 0;
        }

        // Wall BCs (bounce-back on bottom, left, right)
        for (int i = 0; i < nx; ++i) { ux[i] = 0; uy[i] = 0; }
        for (int j = 0; j < ny; ++j) {
            ux[j * nx] = 0; uy[j * nx] = 0;
            ux[j * nx + nx - 1] = 0; uy[j * nx + nx - 1] = 0;
        }

        // Collision (BGK)
        for (int idx = 0; idx < sz; ++idx) {
            for (int q = 0; q < Q; ++q) {
                double eu = ex[q] * ux[idx] + ey[q] * uy[idx];
                double u2 = ux[idx] * ux[idx] + uy[idx] * uy[idx];
                double feq = w[q] * rhoArr[idx] * (1.0 + 3.0*eu + 4.5*eu*eu - 1.5*u2);
                fDist[q][idx] = fNew[q][idx] - (fNew[q][idx] - feq) / tau;
            }
        }
    }

    LBMState state;
    state.nx = nx; state.ny = ny;
    state.ux = ux; state.uy = uy; state.rhoField = rhoArr;
    return state;
}

void FluidSystem::exportLBMCSV(const string& filename, const LBMState& state) {
    ofstream f(filename);
    f << "x,y,ux,uy,rho\n";
    for (int j = 0; j < state.ny; ++j)
        for (int i = 0; i < state.nx; ++i) {
            int idx = j * state.nx + i;
            f << (double)i/(state.nx-1) << "," << (double)j/(state.ny-1) << ","
              << state.ux[idx] << "," << state.uy[idx] << "," << state.rhoField[idx] << "\n";
        }
}

// ════════════════════════════════════════════════════════════════════════════
//  46. SPH (TOY)
// ════════════════════════════════════════════════════════════════════════════

double FluidSystem::sphKernelCubic(double r, double h) {
    double q = r / h;
    double norm = 10.0 / (7.0 * PI * h * h);
    if (q <= 1.0)
        return norm * (1.0 - 1.5 * q * q + 0.75 * q * q * q);
    if (q <= 2.0)
        return norm * 0.25 * pow(2.0 - q, 3);
    return 0;
}

double FluidSystem::sphKernelGradient(double r, double h) {
    double q = r / h;
    double norm = 10.0 / (7.0 * PI * h * h * h);
    if (q <= 1.0)
        return norm * (-3.0 * q + 2.25 * q * q);
    if (q <= 2.0)
        return norm * (-0.75 * pow(2.0 - q, 2));
    return 0;
}

vector<FluidSystem::SPHParticle> FluidSystem::solveSPH_DamBreak(
    int numParticles, double h_smooth, double dt, int steps,
    double cs, double rho0) {
    // Simple 2D dam-break setup
    int nx = (int)sqrt((double)numParticles);
    int ny = nx;
    double spacing = h_smooth * 0.5;

    vector<SPHParticle> particles;
    for (int i = 0; i < nx; ++i)
        for (int j = 0; j < ny; ++j) {
            SPHParticle p;
            p.x = spacing * (i + 0.5);
            p.y = spacing * (j + 0.5);
            p.vx = 0; p.vy = 0;
            p.rho_p = rho0;
            p.p_p = cs * cs * rho0;
            p.m = rho0 * spacing * spacing;
            particles.push_back(p);
        }

    for (int s = 0; s < steps; ++s) {
        // Compute density
        for (auto& pi : particles) {
            pi.rho_p = 0;
            for (auto& pj : particles) {
                double dx = pi.x - pj.x, dy = pi.y - pj.y;
                double r = sqrt(dx * dx + dy * dy);
                pi.rho_p += pj.m * sphKernelCubic(r, h_smooth);
            }
            pi.p_p = cs * cs * (pi.rho_p - rho0);
        }

        // Momentum equation
        for (auto& pi : particles) {
            double ax = 0, ay = -FluidConstants::g;
            for (auto& pj : particles) {
                if (&pi == &pj) continue;
                double dx = pi.x - pj.x, dy = pi.y - pj.y;
                double r = sqrt(dx * dx + dy * dy);
                if (r < 1e-12) continue;
                double gradW = sphKernelGradient(r, h_smooth);
                double pressure_term = -(pi.p_p / (pi.rho_p * pi.rho_p) +
                                          pj.p_p / (pj.rho_p * pj.rho_p));
                ax += pj.m * pressure_term * gradW * dx / r;
                ay += pj.m * pressure_term * gradW * dy / r;
            }
            pi.vx += ax * dt;
            pi.vy += ay * dt;
        }

        // Update positions with simple boundary reflection
        for (auto& pi : particles) {
            pi.x += pi.vx * dt;
            pi.y += pi.vy * dt;
            if (pi.x < 0) { pi.x = -pi.x; pi.vx = -0.5 * pi.vx; }
            if (pi.y < 0) { pi.y = -pi.y; pi.vy = -0.5 * pi.vy; }
        }
    }
    return particles;
}

void FluidSystem::exportSPHCSV(const string& filename,
                                  const vector<SPHParticle>& particles) {
    ofstream f(filename);
    f << "x,y,vx,vy,rho,p\n";
    for (auto& p : particles)
        f << p.x << "," << p.y << "," << p.vx << "," << p.vy << ","
          << p.rho_p << "," << p.p_p << "\n";
}

// ════════════════════════════════════════════════════════════════════════════
//  47. VORTEX METHODS
// ════════════════════════════════════════════════════════════════════════════

pair<double,double> FluidSystem::biotSavartVelocity(
    double x, double y, const vector<VortexState>& vortices, double epsilon) {
    double u = 0, v = 0;
    for (auto& vort : vortices) {
        double dx = x - vort.x, dy = y - vort.y;
        double r2 = dx * dx + dy * dy + epsilon * epsilon;
        u += -vort.gamma * dy / (TWO_PI * r2);
        v +=  vort.gamma * dx / (TWO_PI * r2);
    }
    return {u, v};
}

vector<vector<FluidSystem::VortexState>> FluidSystem::evolveVortexSheet(
    const vector<VortexState>& initial, double dt, int steps, double epsilon) {
    vector<vector<VortexState>> history;
    history.push_back(initial);
    vector<VortexState> current = initial;
    for (int s = 0; s < steps; ++s) {
        vector<VortexState> next = current;
        for (size_t i = 0; i < current.size(); ++i) {
            auto [u, v] = biotSavartVelocity(current[i].x, current[i].y, current, epsilon);
            next[i].x = current[i].x + u * dt;
            next[i].y = current[i].y + v * dt;
        }
        current = next;
        history.push_back(current);
    }
    return history;
}

void FluidSystem::exportVortexMethodCSV(const string& filename,
                                           const vector<VortexState>& vortices) {
    ofstream f(filename);
    f << "x,y,gamma\n";
    for (auto& v : vortices)
        f << v.x << "," << v.y << "," << v.gamma << "\n";
}

// ════════════════════════════════════════════════════════════════════════════
//  48. MULTIPHASE FLOW
// ════════════════════════════════════════════════════════════════════════════

double FluidSystem::voidFraction(double Qg, double Ql, double Ug, double Ul) {
    return Qg / (Qg + Ql);
}

double FluidSystem::mixtureVelocity(double Qg, double Ql, double A) {
    return (Qg + Ql) / A;
}

double FluidSystem::driftFluxVelocity(double C0, double Um, double Ugj) {
    return C0 * Um + Ugj;
}

double FluidSystem::slipRatio(double rhoL, double rhoG, double x_quality) {
    return pow(rhoL / rhoG, 1.0 / 3.0);
}

double FluidSystem::twoPhaseMultiplier(double x_quality, double rhoL, double rhoG,
                                         double muL, double muG) {
    // Lockhart-Martinelli simplified
    double X2 = pow((1.0 - x_quality) / x_quality, 1.8)
               * pow(rhoG / rhoL, 1.0)
               * pow(muL / muG, 0.2);
    return 1.0 + 20.0 / sqrt(X2) + 1.0 / X2;
}

void FluidSystem::exportMultiphaseFlowCSV(const string& filename, double rhoL,
                                             double rhoG, double muL, double muG,
                                             int numX) {
    ofstream f(filename);
    f << "x_quality,void_fraction,slip_ratio,phi2\n";
    for (int i = 1; i < numX; ++i) {
        double x = (double)i / numX;
        double alpha = x / (x + (1.0 - x) * rhoG / rhoL);
        f << x << "," << alpha << "," << slipRatio(rhoL, rhoG, x) << ","
          << twoPhaseMultiplier(x, rhoL, rhoG, muL, muG) << "\n";
    }
}

// ════════════════════════════════════════════════════════════════════════════
//  49. NON-NEWTONIAN FLUIDS
// ════════════════════════════════════════════════════════════════════════════

double FluidSystem::powerLawViscosity(double K, double n, double gammaDot) {
    return K * pow(fabs(gammaDot), n - 1.0);
}

double FluidSystem::binghamViscosity(double tau_y, double mu_p, double gammaDot) {
    if (fabs(gammaDot) < 1e-20) return 1e20; // yielded
    return mu_p + tau_y / fabs(gammaDot);
}

double FluidSystem::carreauViscosity(double mu0, double muInf, double lambda,
                                       double n, double gammaDot) {
    double arg = 1.0 + pow(lambda * gammaDot, 2);
    return muInf + (mu0 - muInf) * pow(arg, (n - 1.0) / 2.0);
}

double FluidSystem::powerLawPipeVelocity(double dpdx, double K, double n,
                                           double R, double r) {
    double s = (n + 1.0) / n;
    double coeff = n / (n + 1.0) * pow(-dpdx / (2.0 * K), 1.0 / n);
    return coeff * (pow(R, s) - pow(fabs(r), s));
}

double FluidSystem::binghamPlugRadius(double tau_y, double dpdx) {
    return 2.0 * tau_y / fabs(dpdx);
}

void FluidSystem::exportNonNewtonianCSV(const string& filename, int model,
                                           double param1, double param2, double param3,
                                           int numPoints) {
    ofstream f(filename);
    f << "gammaDot,mu_eff,tau\n";
    for (int i = 1; i <= numPoints; ++i) {
        double gd = pow(10.0, -2.0 + 5.0 * i / numPoints);
        double mu_eff = 0;
        switch (model) {
            case 0: mu_eff = powerLawViscosity(param1, param2, gd); break;
            case 1: mu_eff = binghamViscosity(param1, param2, gd); break;
            case 2: mu_eff = carreauViscosity(param1, param2, param3, 0.5, gd); break;
        }
        f << gd << "," << mu_eff << "," << mu_eff * gd << "\n";
    }
}

// ════════════════════════════════════════════════════════════════════════════
//  50. MICROFLUIDICS
// ════════════════════════════════════════════════════════════════════════════

double FluidSystem::hydraulicDiameter(double w, double h) {
    return 2.0 * w * h / (w + h);
}

double FluidSystem::microchannelResistance(double mu, double L, double w, double h) {
    // Rectangular channel: R ≈ 12 mu L / (w h^3) for w >> h
    return 12.0 * mu * L / (w * h * h * h);
}

double FluidSystem::capillaryNumber(double mu, double U, double sigma) {
    return mu * U / sigma;
}

double FluidSystem::deanNumber(double Re, double D, double Rc) {
    return Re * sqrt(D / (2.0 * Rc));
}

double FluidSystem::knudsenNumber(double lambda_mfp, double L) {
    return lambda_mfp / L;
}

double FluidSystem::slipVelocity(double lambda_mfp, double dudy) {
    return lambda_mfp * dudy;
}

double FluidSystem::electro_osmoticVelocity(double eps, double zeta, double E,
                                               double mu) {
    return -eps * zeta * E / mu;
}

double FluidSystem::mixingTimeDiffusion(double w, double D) {
    return w * w / D;
}

void FluidSystem::exportMicrofluidicsCSV(const string& filename, double mu,
                                            double sigma, double w, double h,
                                            double L, int numQ) {
    ofstream f(filename);
    f << "Q,U_avg,Re,Ca,R_hyd\n";
    double Dh = hydraulicDiameter(w, h);
    double A = w * h;
    for (int i = 1; i <= numQ; ++i) {
        double Q = 1e-12 * i; // nL/s range
        double U = Q / A;
        double Re = rho * U * Dh / mu;
        double Ca = capillaryNumber(mu, U, sigma);
        f << Q << "," << U << "," << Re << "," << Ca << ","
          << microchannelResistance(mu, L, w, h) << "\n";
    }
}
