# API Reference

FluidDynamicsCore exposes its physics engine through two interfaces:

1. **`FluidSystem` C++ class** — full-featured API with 50 simulations
2. **C API** (`fluid_c_api`) — a subset of functions exported with C linkage for FFI / embedding

---

## C API (`fluid_c_api`)

The C API provides `extern "C"` functions suitable for calling from C, Python (via `ctypes`/`cffi`), C#, or any language with C FFI support. All functions are stateless and take plain numeric arguments.

### Functions

#### `fluid_hydrostatic_pressure`

```c
double fluid_hydrostatic_pressure(double rho, double g, double h, double p0);
```

Computes hydrostatic pressure at depth `h`.

| Parameter | Description |
|---|---|
| `rho` | Fluid density (kg/m³) |
| `g` | Gravitational acceleration (m/s²) |
| `h` | Depth below surface (m) |
| `p0` | Surface pressure (Pa) |
| **Returns** | Pressure at depth `h` (Pa) |

#### `fluid_bernoulli_velocity`

```c
double fluid_bernoulli_velocity(double p1, double p2, double rho,
                                double h1, double h2, double g);
```

Computes velocity from Bernoulli's equation given two pressure/height points.

| Parameter | Description |
|---|---|
| `p1`, `p2` | Pressures at points 1 and 2 (Pa) |
| `rho` | Fluid density (kg/m³) |
| `h1`, `h2` | Heights at points 1 and 2 (m) |
| `g` | Gravitational acceleration (m/s²) |
| **Returns** | Velocity at point 2 (m/s) |

#### `fluid_reynolds_number`

```c
double fluid_reynolds_number(double rho, double U, double L, double mu);
```

Computes the Reynolds number.

| Parameter | Description |
|---|---|
| `rho` | Fluid density (kg/m³) |
| `U` | Characteristic velocity (m/s) |
| `L` | Characteristic length (m) |
| `mu` | Dynamic viscosity (Pa·s) |
| **Returns** | Reynolds number (dimensionless) |

#### `fluid_speed_of_sound`

```c
double fluid_speed_of_sound(double gamma, double R_specific, double T);
```

Computes the speed of sound in an ideal gas.

| Parameter | Description |
|---|---|
| `gamma` | Ratio of specific heats |
| `R_specific` | Specific gas constant (J/(kg·K)) |
| `T` | Temperature (K) |
| **Returns** | Speed of sound (m/s) |

#### `fluid_mach_number`

```c
double fluid_mach_number(double v, double c);
```

Computes the Mach number.

| Parameter | Description |
|---|---|
| `v` | Flow velocity (m/s) |
| `c` | Speed of sound (m/s) |
| **Returns** | Mach number (dimensionless) |

### Building as a Shared Library

To export the C API as a DLL/shared library, define `FLUIDDYNAMICS_EXPORTS` when compiling:

```cmake
target_compile_definitions(FluidDynamicsCore PRIVATE FLUIDDYNAMICS_EXPORTS)
```

This activates the `FLUID_API` macro in `FluidExport.h`, decorating exported functions with `__declspec(dllexport)` on Windows.

---

## `FluidSystem` C++ Class

The `FluidSystem` class is the core physics engine. It can be used directly in C++17 code by linking against the `FluidDynamicsPhysics` static library.

### Constructor

```cpp
FluidSystem(const std::string& fluidName, double density, double viscosity,
            double characteristicLength, double characteristicVelocity);
```

| Parameter | Description |
|---|---|
| `fluidName` | Display name of the fluid |
| `density` | Fluid density ρ (kg/m³) |
| `viscosity` | Dynamic viscosity μ (Pa·s) |
| `characteristicLength` | Reference length L (m) |
| `characteristicVelocity` | Reference velocity U (m/s) |

### Member Variables

| Variable | Type | Description |
|---|---|---|
| `name` | `std::string` | Fluid name |
| `rho` | `double` | Density (kg/m³) |
| `mu` | `double` | Dynamic viscosity (Pa·s) |
| `L` | `double` | Characteristic length (m) |
| `U` | `double` | Characteristic velocity (m/s) |

### Utility Methods

```cpp
double reynoldsNumber() const;       // Re = ρUL/μ
double kinematicViscosity() const;   // ν = μ/ρ
```

### Result Structs

| Struct | Fields | Used by |
|---|---|---|
| `FlowField2D` | `nx`, `ny`, `dx`, `dy`, `u`, `v`, `p`, `rhoField` | Navier-Stokes 2D, LBM |
| `ShockResult` | `M2`, `p2_p1`, `rho2_rho1`, `T2_T1`, `p02_p01` | Normal/oblique shocks |
| `BoundaryLayerResult` | `x`, `delta`, `delta_star`, `theta`, `cf` | Blasius, mixing length |
| `VortexState` | `x`, `y`, `gamma` | Vorticity dynamics, vortex methods |
| `NozzleResult` | `x`, `A`, `M`, `p`, `T`, `rhoField` | Nozzle flow |
| `WaveResult` | `x`, `eta`, `u` | KdV solitons |
| `MHDState` | `x`, `rhoField`, `vx/vy/vz`, `Bx/By/Bz`, `p` | MHD solver |
| `EulerState1D` | `x`, `rhoField`, `u`, `p` | Euler equations |
| `ShallowWaterState` | `x`, `h`, `u` | Shallow water |
| `BurgersState` | `x`, `u` | Burgers equation |
| `RiemannSolution` | `x`, `rhoField`, `u`, `p`, `pStar`, `uStar` | Riemann problem |
| `BlasiusProfile` | `eta`, `f`, `fp`, `fpp` | Blasius BL |
| `LBMState` | `nx`, `ny`, `ux`, `uy`, `rhoField` | Lattice Boltzmann |
| `SPHParticle` | `x`, `y`, `vx`, `vy`, `rho_p`, `p_p`, `m` | SPH |
| `RelativisticShockResult` | `rho2`, `p2`, `v2`, `gamma2` | Relativistic shocks |

### CSV Export

Every simulation has a corresponding `export*CSV` method that writes results to a comma-separated file. Example:

```cpp
FluidSystem fluid("Water", 1000.0, 1e-3, 0.1, 1.0);
fluid.exportHydrostaticsCSV("hydrostatics.csv", 9.81, 101325.0, 100, 10.0);
```

---

## `NumericalSolverFD` Class

Static utility class providing PDE solvers used internally by `FluidSystem`.

### Solvers

| Method | Scheme | Description |
|---|---|---|
| `solveAdvection1D` | First-order upwind | 1D linear advection |
| `solveDiffusion1D` | FTCS (Forward-Time Central-Space) | 1D heat/diffusion equation |
| `solveAdvectionDiffusion1D` | Upwind + FTCS | Combined advection-diffusion |
| `laxWendroff1D` | Lax-Wendroff | Second-order accurate advection |
| `crankNicolsonDiffusion1D` | Crank-Nicolson | Implicit, unconditionally stable diffusion |
| `integrateRK4` | Classical RK4 | General ODE system integration |
| `thomasSolve` | Thomas algorithm | O(n) tridiagonal system solver |
| `poissonJacobi2D` | Jacobi iteration | 2D Poisson equation |
| `poissonSOR2D` | SOR | Accelerated Poisson solver |

### Initial Condition Generators

```cpp
static std::vector<double> makeGaussianPulse(int n, double xMin, double xMax,
                                              double x0, double sigma, double amp);
static std::vector<double> makeStepFunction(int n, double xMin, double xMax,
                                             double xStep, double uL, double uR);
static std::vector<double> makeSineWave(int n, double xMin, double xMax,
                                         double amplitude, double wavenumber);
```

### Export Helpers

```cpp
static void exportSolution1DCSV(const std::string& filename,
                                 const std::vector<double>& x,
                                 const std::vector<double>& u);

static void exportSolution2DCSV(const std::string& filename,
                                 int nx, int ny, double dx, double dy,
                                 const std::vector<double>& u);
```
