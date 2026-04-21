# Application Skeleton And Core Types

## Scope

This document explains the minimal application and library bootstrap created in `v0.1.0`.

## Implemented Files

- `src/app/motor_pre_main.cpp`
- `src/app/motor_solve_main.cpp`
- `src/app/motor_check_main.cpp`
- `include/femsolver/...`
- `src/case/`, `src/mesh/`, `src/material/`, `src/excitation/`, `src/fem/`, `src/nonlinear/`, `src/post/`, `src/solver/`, `src/io/`

## Purpose

The current C++ code does not solve physics yet. Its job is to create the module boundaries that future solver work will fill in.

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

Their current behavior is intentionally small:

- print usage with `--help`
- render a bootstrap report
- exercise the core library enough to make build and integration checks meaningful

This keeps CI realistic without pretending the physics layer already exists.

## Deferred Work

- real file parsing
- finite-element kernel object ownership and lifetimes
- numerical assembly
- result export formats beyond placeholder text reports
