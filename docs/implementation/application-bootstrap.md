# Application Skeleton And Core Types

## Scope

This document explains the minimal application and library bootstrap created in `v0.1.0`.

The repository has since grown a real `v0.2.0` kernel foundation. For the current numerical path, see:

- `docs/implementation/v0.2.0-kernel-foundation.md`
- `docs/implementation/kernel-assembly-and-benchmark.md`

## Implemented Files

- `src/app/motor_pre_main.cpp`
- `src/app/motor_solve_main.cpp`
- `src/app/motor_check_main.cpp`
- `include/femsolver/...`
- `src/case/`, `src/mesh/`, `src/material/`, `src/excitation/`, `src/fem/`, `src/nonlinear/`, `src/post/`, `src/solver/`, `src/io/`

## Purpose

At `v0.1.0`, the C++ code did not solve physics yet. Its job was to create module seams that later solver work could fill in without reshaping the repository.

The baseline already encodes the planned domain vocabulary:

- `CaseSpec`
- `MeshManifest`
- `MaterialDescriptor`
- `ExcitationDef`
- `DiscretizationStrategy`
- `NonlinearPolicy`
- `SolutionBundle`

## Why These Types Exist Now

These types are placeholders, but they are not arbitrary placeholders. They are chosen to pin down the seams that later solver work will need:

- case configuration
- mesh semantics
- material modeling
- excitations
- discretization policy
- nonlinear solve policy
- post-processing outputs

By freezing these seams early, later kernel logic can be attached without first redesigning the repository layout.

## Application Entrypoints

Three executables exist today:

- `motor_pre`
- `motor_solve`
- `motor_check`

Their `v0.1.0` behavior was intentionally small:

- print usage with `--help`
- render a bootstrap report
- exercise the core library enough to make build and integration checks meaningful

This keeps CI realistic without pretending the physics layer already exists.

At `v0.2.0`:

- `motor_pre` and `motor_solve` remain conservative scaffolds
- `motor_check` now runs the canonical Poisson benchmark and reports convergence/error information

## Deferred Work

- real file parsing
- full finite-element kernel object ownership and lifetimes
- result export formats beyond placeholder text reports
