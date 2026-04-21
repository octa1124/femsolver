# Functional Architecture

## Purpose

This document defines what `femsolver` is expected to do by version, what it explicitly does not do yet, and how product-facing functions are grouped.

It is intentionally written from the viewpoint of user-visible capability rather than low-level implementation.

## Product Positioning

`femsolver` is a finite element solver and validation platform for 3D electrical-machine analysis.

Its scope is intentionally staged:

- use `MFEM` as the production solver path
- use `FEniCSx/DOLFINx` only for canonical validation problems
- begin with locked-rotor 3D magnetostatics
- expand toward periodic sectors, multi-angle design studies, and then motion/transient capability

## Functional Tree

```text
femsolver
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
   ├─ MFEM vs FEniCS canonical checks
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

### `v1.0.0`

Target capabilities:

- solve 3D locked-rotor IPM full-model magnetostatics
- support permanent magnets and prescribed current-density windings
- support linear isotropic steel, linear laminated steel, and orthotropic nonlinear `B-H`
- compute `B = curl(A)`, magnetic energy, and Maxwell-stress torque
- run minimal benchmark and regression validation

Explicitly out of scope:

- periodic sectors
- angle sweep automation
- sliding motion
- time stepping
- eddy current
- thermal, circuit, or NVH coupling

### `v2.0.0`

Target capabilities:

- periodic topology meshes and periodic IPM sectors
- equivalence checks between full model and sector model
- stronger canonical validation
- batch case running

Still out of scope:

- low-frequency transient behavior
- rotor motion interface
- coupled thermal and circuit simulation

### `v3.0.0`

Target capabilities:

- multi-angle magnetostatic sweeps
- design-study style batch execution
- torque ripple and air-gap field outputs
- richer anisotropic `B-H` configuration

Still out of scope:

- full transient eddy current
- inverter and controller co-simulation

### `v4.0.0`

Target capabilities:

- motion-related infrastructure
- low-frequency transient and eddy-current path
- interfaces for later thermal, circuit, and NVH expansion

Still not promised:

- a complete industrial multi-physics digital twin

## Current Capability Boundary

As of the repository state created in `v0.1.0`, the codebase is a framework and governance baseline, not yet a physics solver.

This means:

- the repository structure and contracts are real
- the application binaries are real
- the tests and automation are real
- the physics behavior is still a planned next stage

This distinction matters because future commits should not claim solver capability before numerical assembly, constitutive laws, and validation exist.

## Main User Workflows

### Repository Baseline Workflow

- configure with a CMake preset
- build the bootstrap applications
- run `CTest`
- run Python tooling checks
- review changelog fragments and release-note drafts

### Future Solver Workflow

The intended production workflow is:

- define a machine case and mesh manifest
- preprocess geometry and mesh metadata
- run the MFEM solve path
- post-process fields and torque
- validate against benchmark or canonical comparison

That workflow is planned but not fully implemented yet.
