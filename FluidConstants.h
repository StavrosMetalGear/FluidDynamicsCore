#ifndef FLUID_CONSTANTS_H
#define FLUID_CONSTANTS_H

namespace FluidConstants {

    // ── Gravitational acceleration ──────────────────────────────────────
    constexpr double g = 9.80665;                // m/s^2

    // ── Water properties (at 20 °C, 1 atm) ─────────────────────────────
    constexpr double rho_water    = 998.2;       // kg/m^3
    constexpr double mu_water     = 1.002e-3;    // Pa·s  (dynamic viscosity)
    constexpr double nu_water     = 1.004e-6;    // m^2/s (kinematic viscosity)
    constexpr double sigma_water  = 0.0728;      // N/m   (surface tension)
    constexpr double beta_water   = 2.07e-4;     // 1/K   (thermal expansion coeff)
    constexpr double cp_water     = 4182.0;      // J/(kg·K) specific heat
    constexpr double k_water      = 0.598;       // W/(m·K) thermal conductivity
    constexpr double Pr_water     = 7.01;        // Prandtl number
    constexpr double Kbulk_water  = 2.15e9;      // Pa (bulk modulus)

    // ── Air properties (at 20 °C, 1 atm) ───────────────────────────────
    constexpr double rho_air      = 1.204;       // kg/m^3
    constexpr double mu_air       = 1.825e-5;    // Pa·s
    constexpr double nu_air       = 1.516e-5;    // m^2/s
    constexpr double cp_air       = 1005.0;      // J/(kg·K)
    constexpr double k_air        = 0.0257;      // W/(m·K)
    constexpr double Pr_air       = 0.713;       // Prandtl number
    constexpr double gamma_air    = 1.4;         // ratio of specific heats

    // ── Ideal gas ───────────────────────────────────────────────────────
    constexpr double R_universal  = 8.314462618; // J/(mol·K)
    constexpr double M_air        = 0.0289647;   // kg/mol  (molar mass of air)
    constexpr double R_air        = R_universal / M_air; // ~287.05 J/(kg·K)

    // ── Thermodynamic / atmospheric ─────────────────────────────────────
    constexpr double P_atm        = 101325.0;    // Pa
    constexpr double T_std        = 293.15;      // K  (20 °C)
    constexpr double T_stp        = 273.15;      // K  (0 °C)

    // ── Fundamental constants (SI) ──────────────────────────────────────
    constexpr double k_B          = 1.380649e-23;    // J/K  Boltzmann
    constexpr double N_A          = 6.02214076e23;   // 1/mol Avogadro
    constexpr double sigma_SB     = 5.670374419e-8;  // W/(m^2·K^4) Stefan-Boltzmann
    constexpr double c_light      = 2.99792458e8;    // m/s

    // ── MHD / plasma ────────────────────────────────────────────────────
    constexpr double mu_0         = 1.25663706212e-6; // H/m  (vacuum permeability)
    constexpr double epsilon_0    = 8.854187817e-12;  // F/m
    constexpr double e_charge     = 1.602176634e-19;  // C
    constexpr double m_proton     = 1.67262192e-27;   // kg
    constexpr double m_electron   = 9.1093837015e-31; // kg

    // ── Mathematical constants ──────────────────────────────────────────
    constexpr double PI           = 3.14159265358979323846;
    constexpr double TWO_PI       = 2.0 * PI;

    // ── Common non-dimensional numbers (reference) ──────────────────────
    // These are computed, not constant; declared for documentation:
    // Re = rho * U * L / mu          Reynolds number
    // Ma = U / c_s                   Mach number
    // Fr = U / sqrt(g * L)           Froude number
    // We = rho * U^2 * L / sigma     Weber number
    // Ra = g * beta * dT * L^3 / (nu * alpha)  Rayleigh number
    // Pr = nu / alpha                Prandtl number
    // Sc = nu / D                    Schmidt number
    // Pe = U * L / alpha             Péclet number

} // namespace FluidConstants

#endif // FLUID_CONSTANTS_H
