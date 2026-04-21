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

- MFEM integration
- Gmsh-driven geometry generation
- physical material laws
- nonlinear solvers

## `v1.0.0` 3D Locked-Rotor IPM Full Model

Planned deliverables:

- `MFEM` production solver path
- 3D magnetostatic `A in H(curl)` formulation
- permanent magnet and current-density excitations
- linear isotropic and anisotropic materials
- orthotropic nonlinear `B-H`
- `Picard + damped Newton`
- magnetic energy, `B = curl(A)`, and Maxwell-stress torque post-processing
- simplified 360-degree IPM full model

Explicitly deferred:

- periodic sectors
- multi-angle sweeps
- motion, eddy current, thermal, and circuit coupling

## `v2.0.0` Periodic Sector And Validation Expansion

Planned deliverables:

- periodic topology meshes
- IPM sector models
- full-model versus sector regression
- FEniCSx canonical validation cases
- expanded benchmark and convergence coverage
- batch case runner

## `v3.0.0` Multi-Operating-Point Design Platform

Planned deliverables:

- multi-angle magnetostatic scans
- batch operating-point evaluation
- torque ripple and air-gap field outputs
- richer anisotropic `B-H` configuration
- nightly long-run regression and performance tracking

## `v4.0.0` Motion And Low-Frequency Transients

Planned deliverables:

- angle stepping or sliding-band infrastructure
- low-frequency transient and eddy-current capability
- output interfaces for later thermal, circuit, and NVH work

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
