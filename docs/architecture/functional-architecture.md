# Functional Architecture

## Purpose

This document defines what `femsolver` is expected to do by version, what it explicitly does not do yet, and how product-facing functions are grouped.

It is intentionally written from the viewpoint of user-visible capability rather than low-level implementation.

## Product Positioning

`femsolver` is a self-owned finite element solver project that targets controllable and explainable multiphysics simulation.

Its scope is intentionally staged:

- own the solver kernel and explain every major numerical layer
- use `MFEM` and `FEniCSx/DOLFINx` only as reference and validation tools
- use a robot-joint permanent-magnet motor as the first applied MVP
- expand from electromagnetics toward a coupled multiphysics solver and later physics-AI interfaces

## Functional Tree

```text
femsolver
├─ Kernel
│  ├─ mesh and topology
│  ├─ reference elements and quadrature
│  ├─ basis functions and DoF maps
│  ├─ local and global assembly
│  └─ linear and nonlinear algebra interfaces
├─ Physics Modules
│  ├─ electromagnetics
│  ├─ thermal
│  ├─ mechanical
│  ├─ coupled operators
│  └─ time integration
├─ Case Definition
│  ├─ case configuration
│  ├─ mesh and region manifest
│  ├─ material definitions
│  ├─ excitation definitions
│  └─ output requests
├─ Preprocessing
│  ├─ geometry generation
│  ├─ mesh preparation
│  ├─ region and boundary tagging
│  └─ periodic-topology preparation
├─ Solve
│  ├─ finite-element space setup
│  ├─ residual and tangent assembly
│  ├─ linear and nonlinear solve control
│  └─ convergence diagnostics
├─ Post-processing
│  ├─ magnetic flux density
│  ├─ magnetic energy
│  ├─ torque
│  ├─ probes and slices
│  └─ regression summaries
└─ Validation And Governance
   ├─ benchmark comparison
   ├─ MFEM/FEniCS canonical checks
   ├─ test automation
   ├─ release notes and changelog
   └─ GitHub review and quality gates
```

## Versioned Capability Map

### `v0.1.0`

Can do:

- define repository and engineering rules
- configure, build, and test a bootstrap codebase
- run placeholder applications and repository-management scripts
- execute CI, release-note rendering, and changelog validation

Cannot do yet:

- read real machine cases
- generate machine geometry
- solve magnetic fields
- parse physical material data
- export real field results

### `v0.2.0`

Delivered capabilities:

- self-owned tetrahedral mesh and topology support
- reference elements, quadrature, and mapping
- scalar `H1` assembly
- sparse matrix assembly on a simple scalar PDE benchmark

Explicitly out of scope:

- vector finite elements
- motor-specific physics
- multiphysics coupling

### `v0.3.0`

Implemented so far:

- tracked machine-case catalog with paper and industrial provenance
- simplified preprocessing for `section_2d`, `extruded_3d`, and provisional `full_3d`
- edge-based `H(curl)` infrastructure
- curl-curl assembly
- canonical electromagnetic vector-field benchmarks

Still required before the milestone is complete:

- machine-coupled source assembly
- actual motor electromagnetics smoke cases
- more complete vector-field validation coverage

Explicitly out of scope for this stage:

- full motor MVP
- nonlinear anisotropic materials
- coupled thermal or mechanical fields

### `v1.0.0`

Target capabilities:

- solve a robot-joint permanent-magnet motor magnetostatic MVP
- support permanent magnets and prescribed current-density windings
- compute `B = curl(A)`, magnetic energy, and torque
- run benchmark and regression validation on the first machine demonstrator

Explicitly out of scope:

- periodic sectors
- angle sweep automation
- sliding motion
- time stepping
- eddy current
- thermal, circuit, or NVH coupling
- advanced nonlinear anisotropic material support

### `v1.1.0`

Target capabilities:

- nonlinear isotropic and anisotropic magnetic materials
- local-frame constitutive handling
- stronger nonlinear verification

Still out of scope:

- full multiphysics coupling
- transient and moving-interface behavior

### `v2.0.0`

Target capabilities:

- multiphysics coupling foundation
- block operators for coupled fields
- first electro-thermal or magneto-thermal workflows
- shared nonlinear and time-stepping infrastructure

Still out of scope:

- advanced surrogate and AI workflows
- full transient motion

### `v3.0.0`

Target capabilities:

- design-study automation
- interfaces for neural operator, reduced-model, sensor, and RL workflows
- long-run data generation and regression tracking

Still not promised:

- a complete industrial digital twin or end-to-end physical AI stack

## Current Capability Boundary

As of the current repository state, the codebase is no longer only a framework baseline.

It is now:

- a real self-owned scalar benchmark solver
- a real first-slice vector `H(curl)` benchmark solver
- a real machine-case catalog and preprocessing workflow

It is not yet:

- a full robot-joint motor electromagnetics solver
- a nonlinear magnetic solver
- a multiphysics solver

This distinction matters because the repository can already claim concrete discretization and preprocessing capability, but it still must not claim machine-level electromagnetics or multiphysics readiness before those layers exist.

## Main User Workflows

### Repository Baseline Workflow

- configure with a CMake preset
- build the bootstrap applications
- run `CTest`
- run Python tooling checks
- review changelog fragments and release-note drafts

### Current Preprocessing Workflow

- select a tracked case under `cases/machines/`
- run `motor_pre --case --variant --output-dir`
- inspect `mesh_manifest.yaml` and `preprocess_summary.txt`
- use the generated contract as the input boundary for later solver coupling

### Future Solver Workflow

The intended production workflow is:

- define a machine case and mesh manifest
- preprocess geometry and mesh metadata
- run the self-owned solve path
- post-process fields and torque
- validate against benchmark or canonical comparison, including MFEM/FEniCS references where useful

That workflow is planned but not fully implemented yet.
