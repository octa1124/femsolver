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

Completed deliverables:

- self-owned mesh and topology model for simple tetrahedral meshes
- reference tetrahedron, quadrature rules, geometric mapping, and Jacobians
- self-owned `H1` linear basis support
- local stiffness/load assembly and global sparse assembly
- one scalar manufactured Poisson benchmark wired into `motor_check`
- explicit kernel module boundaries for `mesh`, `reference`, `quadrature`, `basis`, `assembly`, `algebra`, and `benchmark`

Explicitly deferred:

- `H(curl)` vector finite elements
- motor-specific physics
- nonlinear materials
- multiphysics coupling

## Cross-Version Case Strategy

The machine-case strategy is fixed early so the kernel can grow against real inputs instead of generic placeholders.

- `joint_type_i_12s10p`: first simple real robot-joint motor case, intended to become the first electromagnetics smoke and regression case
- `exo_outer_rotor_36s40p`: first validation-oriented outer-rotor robot-joint case, intended to become the first main validation case
- `quadruped_joint_36s32p`: staged nonlinear and overload-oriented case for `v1.1.0`
- `tbm76_envelope`: industrial envelope-only case used for preprocessing, packaging, and naming regression rather than early electromagnetics claims

Variant policy:

- every real machine case must maintain `section_2d` and `extruded_3d`
- `full_3d` is optional and only enabled where public geometry information is sufficient
- early preprocessing may use simplified concentric envelopes, but those simplifications must be declared explicitly

## Cross-Version Discretization Objective

The long-term discretization target is broader than the first delivered kernel slice.

- Cell families: both tetrahedral and hexahedral elements must be supported.
- Approximation order: both low-order and higher-order elements must be supported.
- Function spaces: ordinary nodal `H1`, `Nedelec` for `H(curl)`, and `Raviart-Thomas` for `H(div)` must all be part of the product roadmap.
- Delivery is staged: the early motor MVP may use a validated subset, but kernel interfaces must not lock the project into tetrahedral first-order ND-only assumptions.

## `v0.3.0` Vector-Field FEM Foundation

Implemented so far:

- machine-case catalog under `cases/machines/` with source traceability, reconstruction levels, and declared geometry variants
- simplified preprocessing generator and `motor_pre --case --variant --output-dir`
- native C++ loading for `case.yaml` and the generated `mesh_manifest.yaml`
- tetrahedral edge extraction and edge-boundary discovery in the self-owned mesh layer
- tetrahedral scalar and edge DoF maps under a dedicated `kernel/space` module
- first-order tetrahedral `Nedelec` basis functions
- tetrahedral curl-curl local/global assembly
- linear magnetostatic source support for current density and remanent flux density
- canonical `H(curl)` benchmark wired into `motor_check`
- a first machine-coupled linear magnetostatic smoke path in `motor_solve`

Remaining before the milestone is truly complete:

- canonical vector-field validation cases
- documented interface planning for hexahedral and higher-order extensions without rewriting upper layers
- replace the synthetic smoke mesh in `motor_solve` with imported preprocessing mesh data

Explicitly deferred:

- faithful tooth/slot/magnet geometry reconstruction
- nonlinear `B-H`
- coupled thermal or mechanical physics

## `v1.0.0` Robot-Joint Motor MVP

Planned deliverables:

- self-owned electromagnetics solver path for a robot-joint permanent-magnet motor
- 3D locked-rotor magnetostatic `A in H(curl)` formulation
- permanent-magnet and current-density excitations
- field output, magnetic energy, and torque post-processing
- `joint_type_i_12s10p` as the first simple real electromagnetics regression case
- `exo_outer_rotor_36s40p` as the first main validation case
- `tbm76_envelope` retained as preprocessing and packaging regression only unless higher-fidelity public data becomes available

Already started toward this milestone:

- solver-side case and manifest loading
- solver-side linear source assembly for `J` and `B_r`
- first machine-coupled smoke execution in `motor_solve`

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
- `quadruped_joint_36s32p` as the staged nonlinear and overload-oriented validation case
- staged expansion toward hexahedral and higher-order `H1`/`Nedelec`/`Raviart-Thomas` support on the same kernel contracts

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
