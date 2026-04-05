# Building FluidDynamicsCore

This document provides detailed build instructions for all supported platforms.

## Requirements

- **C++ compiler** with C++17 support (MSVC 2019+, GCC 8+, Clang 7+)
- **CMake** 3.15 or newer
- **OpenGL** 3.0+ capable GPU and drivers
- **Ninja** build system (recommended) or platform-default generator

All third-party libraries (GLFW, Dear ImGui, ImPlot) are fetched automatically via CMake `FetchContent` during the configure step. An internet connection is required for the first build.

## Build Steps

### 1. Clone the Repository

```bash
git clone <repository-url>
cd FluidDynamicsCore
```

### 2. Configure with CMake

```bash
cmake -B build -G Ninja
```

To use a different generator (e.g., Visual Studio):

```bash
cmake -B build -G "Visual Studio 17 2022"
```

### 3. Build

```bash
cmake --build build --config Release
```

For a Debug build:

```bash
cmake --build build --config Debug
```

### 4. Run

```bash
# Windows
.\build\FluidDynamicsCore.exe

# Linux / macOS
./build/FluidDynamicsCore
```

## Platform-Specific Notes

### Windows (MSVC)

- The project defines `_USE_MATH_DEFINES` and `NOMINMAX` automatically for MSVC.
- Precompiled headers (`pch.h`) are enabled under MSVC for faster builds.
- Links against `opengl32.lib` automatically.

### Linux

Install the required development packages before building:

```bash
# Debian / Ubuntu
sudo apt-get install build-essential libgl-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev

# Fedora
sudo dnf install mesa-libGL-devel libXrandr-devel libXinerama-devel libXcursor-devel libXi-devel

# Arch
sudo pacman -S mesa libxrandr libxinerama libxcursor libxi
```

### macOS

- Requires Xcode Command Line Tools (`xcode-select --install`).
- Uses OpenGL 3.2 Core Profile with forward compatibility.
- Links against `Cocoa`, `IOKit`, and `OpenGL` frameworks automatically.

## Build Targets

| Target | Type | Description |
|---|---|---|
| `FluidDynamicsPhysics` | Static library | Physics engine (no GUI dependencies) |
| `FluidDynamicsCore` | Executable | Full GUI application |
| `imgui_lib` | Static library | Dear ImGui + GLFW/OpenGL3 backends |
| `implot_lib` | Static library | ImPlot plotting library |

## Integrating the Physics Library

The `FluidDynamicsPhysics` static library can be used independently of the GUI. Link against it in your own CMake project:

```cmake
add_subdirectory(path/to/FluidDynamicsCore)
target_link_libraries(YourTarget PRIVATE FluidDynamicsPhysics)
```

This gives you access to `FluidSystem` and `NumericalSolverFD` without any ImGui/GLFW dependency.

## Troubleshooting

| Problem | Solution |
|---|---|
| CMake cannot find OpenGL | Install GPU drivers and OpenGL development headers for your platform. |
| FetchContent download fails | Check your internet connection. If behind a proxy, configure `CMAKE_PREFIX_PATH` or download dependencies manually. |
| GLFW fails to create a window | Ensure you have a display server running (X11/Wayland on Linux) and a compatible GPU. |
| `_USE_MATH_DEFINES` errors on MSVC | The CMakeLists.txt already defines this; make sure you are building via CMake and not manually compiling individual files. |
