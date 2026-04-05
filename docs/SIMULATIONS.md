# Simulations Reference

FluidDynamicsCore includes **50 fluid dynamics simulations**. Each simulation can be selected from the GUI sidebar, visualized with ImPlot, and exported to CSV.

---

## Classical Fluid Mechanics

### 1. Hydrostatics
Computes hydrostatic pressure distribution, buoyant forces, and submerged fractions. Generates depth-vs-pressure profiles.

**Key functions:** `hydrostaticPressure`, `buoyantForce`, `submergedFraction`, `pressureProfile`

### 2. Bernoulli Equation
Applies Bernoulli's principle to compute pressure and velocity along a streamline. Includes Torricelli, Venturi, and Pitot tube models.

**Key functions:** `bernoulliPressure`, `bernoulliVelocity`, `torricelliVelocity`, `venturiVelocity`, `pitotVelocity`

### 3. Euler Equations (1D)
Solves the 1D compressible Euler equations using a finite-volume method with CFL-based time stepping.

**Key functions:** `solveEuler1D`

### 4. Navier-Stokes (2D Cavity)
Solves the incompressible Navier-Stokes equations for lid-driven cavity flow on a 2D grid.

**Key functions:** `solveNavierStokes2DCavity`

### 5. Poiseuille Flow
Analytical solutions for fully-developed laminar flow in pipes and channels. Computes velocity profiles, flow rates, and wall shear.

**Key functions:** `poiseuilleVelocity`, `poiseuilleMaxVelocity`, `poiseuilleFlowRate`, `poiseuilleWallShear`

### 6. Couette Flow
Analyzes viscous flow between parallel plates with optional pressure gradient (plane Couette–Poiseuille flow).

**Key functions:** `couetteVelocity`, `couetteShearStress`, `couetteProfile`

### 7. Stokes Flow (Creeping Flow)
Low-Reynolds-number flow around a sphere: Stokes drag, drag coefficient, terminal velocity, and streamfunction.

**Key functions:** `stokesDragForce`, `stokesDragCoefficient`, `stokesTerminalVelocity`, `stokesStreamfunction`

### 8. Potential Flow
Superposition of elementary potential flow solutions (uniform, source, vortex, doublet) and flow around a cylinder.

**Key functions:** `uniformFlow`, `sourceFlow`, `vortexFlow`, `doubletFlow`, `flowAroundCylinder`

### 9. Stream Functions & Visualization
Computes and exports stream function fields for various elementary flows and the Rankine oval.

**Key functions:** `streamFunctionUniform`, `streamFunctionSource`, `streamFunctionVortex`, `streamFunctionRankineOval`

### 10. Vorticity Dynamics
Rankine and Lamb-Oseen vortex models, Kelvin's circulation theorem, and point-vortex evolution.

**Key functions:** `rankineVortexVelocity`, `lambOseenVortexVelocity`, `evolvePointVortices`

---

## Waves & Instabilities

### 11. Surface Gravity Waves
Deep-water and shallow-water wave dispersion, group velocity, wave energy, and second-order Stokes waves.

**Key functions:** `deepWaterDispersion`, `shallowWaterSpeed`, `generalDispersion`, `waveGroupVelocity`, `stokesSecondOrderEta`

### 12. Shallow Water Equations
1D shallow water solver for dam-break and Riemann-type problems.

**Key functions:** `solveShallowWater1D`

### 13. Kelvin-Helmholtz Instability
Growth rate and critical wavelength for shear-layer instability between two fluid layers.

**Key functions:** `khGrowthRate`, `khCriticalWavelength`

### 14. Rayleigh-Taylor Instability
Growth rate, critical wavelength, and Attwood number for density-stratified instability.

**Key functions:** `rtGrowthRate`, `rtCriticalWavelength`, `attwoodNumber`

### 15. Rayleigh-Bénard Convection
Critical Rayleigh number, Nusselt number, and onset wavenumber for thermal convection.

**Key functions:** `rayleighNumber`, `criticalRayleighNumber`, `nusseltNumberRB`

### 16. Taylor-Couette Flow
Velocity profiles and Taylor number for flow between coaxial rotating cylinders.

**Key functions:** `taylorCouetteVelocity`, `taylorNumber`, `criticalTaylorNumber`

### 17. Capillary Waves
Dispersion relation for capillary-gravity waves, capillary length, minimum phase speed, and Young-Laplace pressure.

**Key functions:** `capillaryWaveDispersion`, `capillaryLength`, `youngLaplaceDropletPressure`

### 18. Solitons (KdV Equation)
Korteweg-de Vries soliton solutions and time integration for soliton–soliton interaction.

**Key functions:** `kdvSoliton`, `solveKdV`

---

## Compressible Flow & Shock Waves

### 19. Shock Waves (Burgers Equation)
Inviscid and viscous Burgers equation solvers demonstrating shock formation and propagation.

**Key functions:** `solveBurgersInviscid`, `solveBurgersViscous`, `burgersExactShockSpeed`

### 20. Riemann Problem
Exact Riemann solver for the 1D Euler equations with shock, rarefaction, and contact discontinuity resolution.

**Key functions:** `solveRiemannExact`

### 21. Speed of Sound
Speed of sound in ideal gases and liquids, Mach number, and Mach angle.

**Key functions:** `speedOfSoundIdealGas`, `speedOfSoundLiquid`, `machNumber`, `machAngle`

### 22. Isentropic Flow
Isentropic pressure/temperature/density/area ratios as functions of Mach number.

**Key functions:** `isentropicPressureRatio`, `isentropicAreaRatio`, `machFromAreaRatio`

### 23. Normal Shocks
Normal shock relations: downstream Mach number, pressure/density/temperature ratios, stagnation pressure loss.

**Key functions:** `normalShock`

### 24. Oblique Shocks
Oblique shock wave angle, deflection angle, and downstream conditions.

**Key functions:** `obliqueShockBeta`, `obliqueShock`, `maxDeflectionAngle`

### 25. Prandtl-Meyer Expansion
Prandtl-Meyer expansion fan: turning angle function and Mach number from expansion.

**Key functions:** `prandtlMeyerFunction`, `machFromPrandtlMeyer`, `expansionFanM2`

### 26. Nozzle Flow (de Laval)
Converging-diverging nozzle flow solver with isentropic Mach/pressure/temperature profiles and thrust coefficient.

**Key functions:** `solveDeLavalNozzle`, `nozzleThrustCoefficient`

### 27. Fanno Flow
Adiabatic duct flow with friction: friction parameter, temperature/pressure/stagnation-pressure ratios.

**Key functions:** `fannoFLstarOverD`, `fannoTemperatureRatio`, `fannoPressureRatio`

### 28. Rayleigh Flow
Frictionless duct flow with heat addition: temperature/pressure/stagnation ratios.

**Key functions:** `rayleighTemperatureRatio`, `rayleighPressureRatio`, `rayleighStagnationTemperatureRatio`

---

## Turbulence & Boundary Layers

### 29. Reynolds Number Regimes
Reynolds number computation, flow regime classification, pipe/plate critical Re, drag and friction factor correlations.

**Key functions:** `reynoldsNum`, `flowRegime`, `dragCoefficientSphere`, `frictionFactorTurbulentColebrook`

### 30. Kolmogorov Cascade
Kolmogorov length/time/velocity scales, energy spectrum E(k), integral scale, Taylor microscale.

**Key functions:** `kolmogorovLengthScale`, `kolmogorovEnergySpectrum`, `integralLengthScale`, `taylorMicroscale`

### 31. Mixing Length Theory
Prandtl mixing length model, turbulent viscosity, log-law velocity profile, friction velocity.

**Key functions:** `mixingLength`, `turbulentViscosity`, `logLawVelocity`, `frictionVelocity`

### 32. Boundary Layers (Blasius)
Blasius flat-plate boundary layer: ODE solution, BL/displacement/momentum thickness, skin friction.

**Key functions:** `solveBlasius`, `blasiusBLThickness`, `blasiusSkinFriction`, `blasiusBLGrowth`

### 33. Drag on a Sphere
Empirical drag coefficient correlations, terminal velocity, Oseen correction.

**Key functions:** `cdSphereEmpirical`, `terminalVelocitySphere`, `oseen_dragCoefficient`

### 34. Pipe Flow (Moody Diagram)
Darcy friction factor (laminar and Colebrook), head loss, pressure drop, pipe flow rate.

**Key functions:** `darcyFrictionFactor`, `headLoss`, `pressureDrop`

---

## Heat & Mass Transfer

### 35. Heat Convection
Nusselt number correlations for flat plates, cylinders, and internal pipe flow. Convective heat transfer and thermal BL thickness.

**Key functions:** `nusseltFlatPlate`, `nusseltCylinder`, `nusseltPipeInternal`, `convectiveHeatTransfer`

### 36. Mass Diffusion (Fick's Law)
Fick's flux, 1D diffusion analytical solution, Schmidt/Sherwood numbers, mass transfer coefficient.

**Key functions:** `fickFlux`, `diffusionSolution1D`, `schmidtNumber`, `sherwoodNumber`

---

## Magnetohydrodynamics & Plasma

### 37. Magnetohydrodynamics (MHD)
Alfvén speed, magnetosonic speeds, magnetic pressure, Hartmann number, and 1D MHD solver.

**Key functions:** `alfvenSpeed`, `magnetosonicSpeedFast`, `hartmannNumber`, `solveMHD1D`

### 38. Alfvén Waves
Alfvén wave dispersion, group velocity, and magnetic field perturbation profiles.

**Key functions:** `alfvenWaveDispersion`, `alfvenWaveGroupVelocity`, `alfvenWaveMagneticField`

### 39. Plasma Beta
Plasma beta, Debye length, plasma frequency, gyroradius, and gyrofrequency.

**Key functions:** `plasmaBeta`, `debyeLength`, `plasmaFrequency`, `gyroRadius`

---

## Astrophysical Flows

### 40. Accretion Disk Model
Keplerian velocity/angular velocity, viscous accretion rate, disk temperature, Eddington luminosity.

**Key functions:** `keplerianVelocity`, `viscousAccretionRate`, `diskTemperature`, `eddingtonLuminosity`

### 41. Jeans Instability
Jeans length, Jeans mass, growth rate, and free-fall time for gravitational instability in gas clouds.

**Key functions:** `jeansLength`, `jeansMass`, `jeansGrowthRate`, `freefall_time`

### 42. Bondi Accretion
Spherical Bondi accretion: Bondi radius, accretion rate, and radial Mach profile.

**Key functions:** `bondiRadius`, `bondiAccretionRate`, `bondiMachProfile`

### 43. Viscous Disk (Shakura-Sunyaev)
Alpha-disk model: Shakura-Sunyaev viscosity, scale height, surface density evolution, viscous timescale.

**Key functions:** `shakuraSunyaevViscosity`, `diskScaleHeight`, `diskSurfaceDensityEvolution`

### 44. Relativistic Fluid Dynamics
Relativistic enthalpy, sound speed, Lorentz factor, Bernoulli invariant, and relativistic shock relations.

**Key functions:** `relativisticEnthalpy`, `lorentzFactor`, `relativisticNormalShock`

---

## Computational Methods

### 45. Lattice Boltzmann (D2Q9)
Toy LBM solver for 2D lid-driven cavity flow using the D2Q9 lattice and BGK collision operator.

**Key functions:** `solveLBM_D2Q9`

### 46. SPH (Smoothed Particle Hydrodynamics)
Dam-break simulation using SPH with a cubic spline kernel.

**Key functions:** `sphKernelCubic`, `sphKernelGradient`, `solveSPH_DamBreak`

### 47. Vortex Methods
Biot-Savart point-vortex dynamics and vortex sheet evolution with regularization.

**Key functions:** `biotSavartVelocity`, `evolveVortexSheet`

---

## Multiphase & Special Flows

### 48. Multiphase Flow
Void fraction, mixture velocity, drift-flux model, slip ratio, and two-phase friction multiplier.

**Key functions:** `voidFraction`, `mixtureVelocity`, `driftFluxVelocity`, `twoPhaseMultiplier`

### 49. Non-Newtonian Fluids
Power-law, Bingham plastic, and Carreau viscosity models. Pipe flow for power-law fluids and Bingham plug radius.

**Key functions:** `powerLawViscosity`, `binghamViscosity`, `carreauViscosity`, `powerLawPipeVelocity`

### 50. Microfluidics
Hydraulic diameter, microchannel resistance, capillary/Dean/Knudsen numbers, slip velocity, electro-osmotic flow, diffusive mixing time.

**Key functions:** `hydraulicDiameter`, `microchannelResistance`, `capillaryNumber`, `electro_osmoticVelocity`
