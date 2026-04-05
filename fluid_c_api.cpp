// ════════════════════════════════════════════════════════════════════════════
//  fluid_c_api.cpp — C API wrapper for FluidDynamicsPhysics library
// ════════════════════════════════════════════════════════════════════════════

#include "FluidSystem.h"
#include "FluidExport.h"

extern "C" {

FLUID_API double fluid_hydrostatic_pressure(double rho, double g, double h, double p0) {
    return FluidSystem::hydrostaticPressure(rho, g, h, p0);
}

FLUID_API double fluid_bernoulli_velocity(double p1, double p2, double rho,
                                           double h1, double h2, double g) {
    return FluidSystem::bernoulliVelocity(p1, p2, rho, h1, h2, g);
}

FLUID_API double fluid_reynolds_number(double rho, double U, double L, double mu) {
    return FluidSystem::reynoldsNum(rho, U, L, mu);
}

FLUID_API double fluid_speed_of_sound(double gamma, double R_specific, double T) {
    return FluidSystem::speedOfSoundIdealGas(gamma, R_specific, T);
}

FLUID_API double fluid_mach_number(double v, double c) {
    return FluidSystem::machNumber(v, c);
}

} // extern "C"
