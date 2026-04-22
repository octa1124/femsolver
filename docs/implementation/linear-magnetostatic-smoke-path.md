# Linear Magnetostatic Smoke Path

## Purpose

This document records the first solver-side bridge between the current `v0.3.0` vector kernel and the future `v1.0.0` robot-joint motor MVP.

The purpose of this step is to stop `motor_solve` from remaining a pure placeholder while still staying honest about what is and is not implemented.

The current result is:

- native C++ loading of `case.yaml`
- native C++ loading of the generated `mesh_manifest.yaml`
- a linear magnetostatic `A`-form smoke assembly with `Nedelec`
- current-density and remanent-flux source support
- a first machine-coupled smoke solve that binds real case and manifest semantics

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
4. builds the current smoke mesh
5. assembles the linear magnetostatic system
6. applies zero edge Dirichlet boundary conditions
7. solves with the self-owned conjugate-gradient path
8. reports energy and solve status

## Important Current Limitation

The current machine smoke solve is semantically coupled to real case and manifest data, but it still uses a synthetic region-tagged tetrahedral kernel mesh instead of importing the generated `.msh` file.

That means:

- the case and manifest contracts are real
- the source binding and solve path are real
- the mesh geometry is still a smoke mesh

This is intentional and explicitly reported as a warning in the solution bundle.

## Why This Is Still Valuable

Even with the synthetic smoke mesh, this step closes several real gaps:

- `motor_solve` is no longer only a placeholder
- the repository now owns a solver-side case contract
- the repository now owns a solver-side manifest contract
- the repository now owns a linear `J + B_r` source assembly path
- the first end-to-end application command now exercises real case semantics

This is the right kind of intermediate step because it improves real behavior without pretending the full motor MVP is done.

## What Is Not Done Yet

This path does not yet include:

- `.msh` import into the self-owned mesh layer
- faithful tooth, slot, coil, or magnet geometry
- real conductor subregions from preprocessing
- machine-grade post-processing for `B`, torque, and field export
- nonlinear magnetic materials
- validation against published motor targets

## Tests

The current implementation adds:

- unit coverage for `CaseSpec::LoadFromFile`
- unit coverage for `MeshManifest::LoadFromFile`
- unit coverage for linear magnetostatic assembly
- unit coverage for the joint-motor linear smoke solver
- integration coverage for `motor_pre -> motor_solve`

## Next Steps

The shortest honest path forward is:

1. keep the current linear source assembly
2. replace the synthetic smoke mesh with imported preprocessing mesh data
3. add `B`, energy, and torque post-processing
4. promote the smoke path into the first true `v1.0.0` machine solve path
