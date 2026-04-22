# Linear Magnetostatic Smoke Path

## Purpose

This document records the first solver-side bridge between the current `v0.3.0` vector kernel and the future `v1.0.0` robot-joint motor MVP.

The purpose of this step is to stop `motor_solve` from remaining a pure placeholder while still staying honest about what is and is not implemented.

The current result is:

- native C++ loading of `case.yaml`
- native C++ loading of the generated `mesh_manifest.yaml`
- explicit binding from case-declared material and excitation profiles into a solver-owned linear smoke model
- a linear magnetostatic `A`-form smoke assembly with `Nedelec`
- current-density and remanent-flux source support
- a first Gmsh `MSH2` tetrahedral import path for generated preprocessing meshes
- a first machine-coupled smoke solve that binds real case, manifest, and imported mesh semantics
- per-cell `B = curl(A)` recovery with average and max flux-density summaries

## What Is Implemented

### Native Case And Manifest Loading

The repository now reads the current preprocessing contracts directly in C++:

- `CaseSpec::LoadFromFile(...)`
- `MeshManifest::LoadFromFile(...)`

These loaders intentionally cover the current repository schema rather than full generic YAML.

That scope is deliberate:

- they parse the schema that this repository actually produces
- they fail early on invalid or incomplete files
- they let `motor_solve` stop depending on placeholder bootstrap values

### Linear Magnetostatic Cell Data

The new linear magnetostatic assembly path uses a per-cell data model:

- reluctivity `nu`
- current density `J`
- remanent flux density `B_r`

This is the minimum self-owned source model needed to connect the vector kernel to motor semantics.

### Explicit Case-Profile Binding

The current solver no longer hard-codes all region behavior directly inside the assembly callback.

Instead it builds a `JointMotorLinearModel` from the current case metadata:

- `materials_profile.profile_id`
- `excitation_profile.profile_id`
- `excitation_profile.current_density_model`
- `excitation_profile.magnetization_model`

That model resolves the current smoke-path region semantics explicitly:

- `outer_air` and `airgap`
- `stator_core`
- `magnet_ring`
- `rotor_core`
- `shaft`

The current implementation is still conservative and honest:

- it supports only the repository's current smoke-profile ids
- it rejects envelope-only profiles for `motor_solve`
- it documents the current placeholder reductions through solver warnings

Examples of those current reductions are:

- balanced three-phase excitation collapsed to a uniform `stator_core` current density
- radial permanent-magnet declarations collapsed to an axial remanent-flux source for the smoke path

### Weak Form Slice

The implemented linear smoke path uses the `A in H(curl)` idea in the simplest repository-owned form:

- stiffness term from `nu * curl(A) dot curl(v)`
- current source term from `J dot v`
- remanence source term from `nu * B_r dot curl(v)`

This is not yet the full production motor formulation, but it is the correct direction for a linear magnetostatic bridge.

### `motor_solve` Behavior

`motor_solve` now has two modes:

- bootstrap mode with no input files
- machine smoke mode with `--case` and `--manifest`

In machine smoke mode it:

1. loads `case.yaml`
2. loads `mesh_manifest.yaml`
3. validates required named regions
4. imports the generated Gmsh `MSH2` tetra mesh when available
5. falls back to the current synthetic smoke mesh when import is unavailable
6. assembles the linear magnetostatic system
7. applies zero edge Dirichlet boundary conditions
8. solves with the self-owned conjugate-gradient path
9. reports energy, recovered flux-density summaries, and solve status

## Why This Is Still Valuable

Even with the remaining fallback path, this step closes several real gaps:

- `motor_solve` is no longer only a placeholder
- the repository now owns a solver-side case contract
- the repository now owns a solver-side manifest contract
- the repository now owns a first solver-side material and excitation profile binding layer
- the repository now owns a linear `J + B_r` source assembly path
- the repository now owns the first real preprocessing-mesh import path
- the first end-to-end application command now exercises real case semantics and can consume generated meshes

This is the right kind of intermediate step because it improves real behavior without pretending the full motor MVP is done.

## What Is Not Done Yet

This path does not yet include:

- faithful tooth, slot, coil, or magnet geometry
- real conductor subregions from preprocessing
- machine-grade torque and field export
- nonlinear magnetic materials
- validation against published motor targets

## Tests

The current implementation adds:

- unit coverage for `CaseSpec::LoadFromFile`
- unit coverage for `MeshManifest::LoadFromFile`
- unit coverage for Gmsh `MSH2` tetra import
- unit coverage for linear magnetostatic assembly
- unit coverage for the joint-motor linear smoke solver
- integration coverage for `motor_pre -> motor_solve`

## Next Steps

The shortest honest path forward is:

1. keep the current linear source assembly and imported tetra path stable
2. replace the concentric-envelope geometry with the first tooth/slot/magnet-resolved reconstruction
3. add torque evaluation and stable regression thresholds on imported-machine outputs
4. promote the smoke path into the first true `v1.0.0` machine solve path
