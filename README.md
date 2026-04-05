# FluidDynamicsCore

A C++17 fluid dynamics simulation engine with an interactive GUI, featuring **50 physics simulations** spanning classical fluid mechanics, compressible flow, turbulence, astrophysical flows, and computational methods.

![C++17](https://img.shields.io/badge/C%2B%2B-17-blue)
![CMake](https://img.shields.io/badge/CMake-%3E%3D3.15-blue)
![License](https://img.shields.io/badge/License-MIT-green)

## Features

- **50 fluid dynamics simulations** covering a wide range of topics:
  - Hydrostatics, Bernoulli equation, Euler equations
  - Navier-Stokes (2D cavity), Poiseuille flow, Couette flow
  - Potential flow, vorticity dynamics, boundary layers
  - Compressible flow: normal/oblique shocks, nozzle flow, Fanno/Rayleigh flow
  - Turbulence: Kolmogorov cascade, mixing length theory
  - Waves: surface gravity waves, shallow water, solitons (KdV), capillary waves
  - Instabilities: Kelvin-Helmholtz, Rayleigh-Taylor, Rayleigh-Bénard, Jeans
  - Astrophysics: accretion disks, Bondi accretion, viscous disks, relativistic fluids
  - MHD and plasma: Alfvén waves, plasma beta
  - Computational methods: Lattice Boltzmann, SPH, vortex methods
  - Multiphase flow, non-Newtonian fluids, microfluidics
- **Interactive GUI** built with [Dear ImGui](https://github.com/ocornut/imgui) and [ImPlot](https://github.com/epezent/implot) for real-time plotting
- **CSV export** for every simulation — import results into MATLAB, Python, Excel, etc.
- **C API** (`fluid_c_api`) for embedding the physics engine in other applications or language bindings
- **Numerical solvers** (advection, diffusion, Crank-Nicolson, RK4, Poisson/SOR, Thomas algorithm)

## Project Structure

```
FluidDynamicsCore/
├── CMakeLists.txt            # Build configuration (CMake 3.15+)
├── main.cpp                  # Application entry point (GLFW + OpenGL window)
├── GuiApp.h / GuiApp.cpp     # ImGui/ImPlot GUI with 50 simulation panels
├── FluidSystem.h / .cpp      # Physics engine — all 50 simulations
├── NumericalSolverFD.h / .cpp# PDE solvers (advection, diffusion, Poisson, RK4, …)
├── fluid_c_api.cpp           # C API wrapper for FFI / embedding
├── FluidExport.h             # DLL export macros
├── pch.h                     # Precompiled header (MSVC)
├── Eigen/                    # Eigen header-only linear algebra library
├── LICENSE.txt               # MIT License
├── README.md                 # This file
└── docs/                     # Additional documentation
    ├── BUILDING.md           # Build instructions
    ├── SIMULATIONS.md        # Detailed list of all 50 simulations
    └── API.md                # C API and FluidSystem class reference
```

## Prerequisites

| Dependency | Version | Acquired via |
|---|---|---|
| C++ compiler | C++17 support | System (MSVC, GCC, Clang) |
| CMake | ≥ 3.15 | System |
| OpenGL | 3.0+ | System / driver |
| GLFW | 3.4 | FetchContent (automatic) |
| Dear ImGui | v1.91.8 | FetchContent (automatic) |
| ImPlot | v0.16 | FetchContent (automatic) |
| Eigen | (bundled) | Included in `Eigen/` directory |

> **Note:** GLFW, Dear ImGui, and ImPlot are downloaded automatically by CMake at configure time — no manual installation is required.

## Quick Start

### Windows (Visual Studio / MSVC)

```bash
cmake -B build -G Ninja
cmake --build build --config Release
./build/FluidDynamicsCore
```

### Linux

```bash
# Install OpenGL development libraries (if not already present)
sudo apt-get install libgl-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev

cmake -B build -G Ninja
cmake --build build --config Release
./build/FluidDynamicsCore
```

### macOS

```bash
cmake -B build -G Ninja
cmake --build build --config Release
./build/FluidDynamicsCore
```

## Usage

1. Launch `FluidDynamicsCore`.
2. Select a simulation from the sidebar (1–50).
3. Adjust parameters in the GUI controls.
4. View real-time plots rendered by ImPlot.
5. Export results to CSV using the export buttons provided in each simulation panel.

## Documentation

| Document | Description |
|---|---|
| [docs/BUILDING.md](docs/BUILDING.md) | Detailed build instructions for all platforms |
| [docs/SIMULATIONS.md](docs/SIMULATIONS.md) | Full list and description of all 50 simulations |
| [docs/API.md](docs/API.md) | C API reference and `FluidSystem` class documentation |

## License

This project is licensed under the MIT License — see [LICENSE.txt](LICENSE.txt) for details.
