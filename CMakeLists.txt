cmake_minimum_required(VERSION 3.15)
project(FluidDynamicsCore LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── FetchContent: GLFW, Dear ImGui, ImPlot ──────────────────────────────
include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.91.8
)
FetchContent_MakeAvailable(imgui)

FetchContent_Declare(
    implot
    GIT_REPOSITORY https://github.com/epezent/implot.git
    GIT_TAG        v0.16
)
FetchContent_MakeAvailable(implot)

# ── ImGui library (static) ──────────────────────────────────────────────
add_library(imgui_lib STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui_lib PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(imgui_lib PUBLIC glfw)

# ── ImPlot library (static) ─────────────────────────────────────────────
add_library(implot_lib STATIC
    ${implot_SOURCE_DIR}/implot.cpp
    ${implot_SOURCE_DIR}/implot_items.cpp
)
target_include_directories(implot_lib PUBLIC ${implot_SOURCE_DIR})
target_link_libraries(implot_lib PUBLIC imgui_lib)

# ── FluidDynamicsPhysics (static library — physics engine, no GUI) ──────
add_library(FluidDynamicsPhysics STATIC
    FluidSystem.cpp
    NumericalSolverFD.cpp
)
target_include_directories(FluidDynamicsPhysics PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Eigen
)
if(MSVC)
    target_compile_definitions(FluidDynamicsPhysics PUBLIC _USE_MATH_DEFINES NOMINMAX)
endif()

# ── FluidDynamicsCore (GUI executable) ──────────────────────────────────
add_executable(FluidDynamicsCore
    main.cpp
    GuiApp.cpp
    fluid_c_api.cpp
)
target_link_libraries(FluidDynamicsCore PRIVATE
    FluidDynamicsPhysics
    imgui_lib
    implot_lib
)
if(WIN32)
    target_link_libraries(FluidDynamicsCore PRIVATE opengl32)
elseif(APPLE)
    find_library(COCOA_LIB Cocoa)
    find_library(IOKIT_LIB IOKit)
    find_library(OPENGL_LIB OpenGL)
    target_link_libraries(FluidDynamicsCore PRIVATE ${COCOA_LIB} ${IOKIT_LIB} ${OPENGL_LIB})
else()
    target_link_libraries(FluidDynamicsCore PRIVATE GL)
endif()

if(MSVC)
    target_compile_definitions(FluidDynamicsCore PRIVATE _USE_MATH_DEFINES NOMINMAX)
    target_precompile_headers(FluidDynamicsCore PRIVATE pch.h)
endif()
