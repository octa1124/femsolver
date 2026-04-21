# Roadmap

## `v0.1.0` Repository Baseline

Completed or in progress:

- repository structure and governance
- `Agent.md`, `CHANGELOG.md`, and release-note templates
- CMake/CTest bootstrap and minimal application skeleton
- GitHub Actions, CodeRabbit, SonarQube Cloud, and Codecov configuration
- issue/PR templates and repository bootstrap data for labels, milestones, and rulesets

Implementation documentation and git hygiene:

- each repository-baseline subsystem must have a technical note under `docs/implementation/`
- the completed milestone must be captured in git as a version-finish commit

Not in scope:

- kernel mesh and basis implementation
- matrix assembly
- physical material laws
- nonlinear solvers

## `v0.2.0` Kernel Foundation MVP

Planned deliverables:

- self-owned mesh and topology model for simple tetrahedral meshes
- reference tetrahedron, quadrature rules, geometric mapping, and Jacobians
- self-owned `H1` linear basis support
- local stiffness/load assembly and global sparse assembly
- one scalar benchmark, such as Poisson or heat conduction, end to end

Explicitly deferred:

- `H(curl)` vector finite elements
- motor-specific physics
- nonlinear materials
- multiphysics coupling

## `v0.3.0` Vector-Field FEM Foundation

Planned deliverables:

- edge orientation and DoF ownership for vector elements
- first-kind Nedelec `H(curl)` basis on tetrahedra
- curl-curl assembly
- current-source and simple magnetization source support
- canonical vector-field validation cases

Explicitly deferred:

- full motor geometry workflows
- nonlinear `B-H`
- coupled thermal or mechanical physics

## `v1.0.0` Robot-Joint Motor MVP

Planned deliverables:

- self-owned electromagnetics solver path for a robot-joint permanent-magnet motor
- 3D locked-rotor magnetostatic `A in H(curl)` formulation
- permanent-magnet and current-density excitations
- field output, magnetic energy, and torque post-processing
- benchmark and regression validation for the first machine demonstrator

Explicitly deferred:

- periodic sectors
- multi-angle sweeps
- motion, eddy current, thermal, and circuit coupling
- advanced nonlinear anisotropic material support

## `v1.1.0` Nonlinear And Anisotropic Magnetic Materials

Planned deliverables:

- nonlinear isotropic `B-H`
- local-frame anisotropic and orthotropic magnetic material support
- consistent tangent construction
- nonlinear solve stabilization and regression coverage

## `v2.0.0` Multiphysics Coupling Foundation

Planned deliverables:

- block-system architecture for coupled fields
- shared time-stepping and nonlinear-problem interfaces
- first electro-thermal or magneto-thermal coupling path
- extensible interfaces for later magneto-mechanical work
- stronger canonical validation and expanded convergence coverage

## `v3.0.0` Design Studies And Physics-AI Interfaces

Planned deliverables:

- multi-operating-point studies and batch execution
- reduced-order and surrogate-data export hooks
- interfaces for neural operator or surrogate-model workflows
- sensor-data and RL-environment integration interfaces
- nightly long-run regression and performance tracking

## `v4.0.0` Motion And Low-Frequency Transients

Planned deliverables:

- angle stepping or sliding-band infrastructure
- low-frequency transient and eddy-current capability
- tighter multiphysics state exchange for thermal, circuit, and NVH work

## Release Summary Rule

Every release summary must include:

- completed work
- deferred or incomplete work
- known risks
- compatibility changes
- handoff to the next version

Every completed version must also:

- update implementation docs for the work actually shipped
- land in git as a commit that represents the finished milestone state
