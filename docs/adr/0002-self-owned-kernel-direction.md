# ADR 0002: Use A Self-Owned Solver Kernel

## Status

Accepted

## Context

The earlier repository wording leaned toward an MFEM-first production implementation. That direction is misaligned with the actual product goal:

- own the solver internals
- keep the numerical path controllable and explainable
- support future multiphysics coupling
- leave room for later reduced models, neural operators, RL environments, and sensor-facing physics workflows

Depending on MFEM or FEniCS as the production kernel would speed up a narrow electromagnetics application but would reduce control over:

- mesh semantics
- basis and space definitions
- assembly contracts
- multiphysics block structure
- solver and time-integration interfaces

## Decision

Use a self-owned kernel as the production path.

`MFEM` and `FEniCSx/DOLFINx` are allowed only in these roles:

- study and reference
- benchmark comparison
- weak-form and discretization cross-checks
- optional external tooling that is isolated from the production kernel

The first application MVP is a robot-joint permanent-magnet motor. That MVP is not the entire product; it is the first proving ground for the kernel.

## Consequences

- roadmap staging must include kernel milestones before the first motor MVP
- the production code must implement its own mesh, basis, quadrature, DoF, and assembly path over time
- multiphysics growth becomes a first-class design constraint rather than a future refactor
- reference comparisons against MFEM and FEniCS remain valuable, but they do not define the product core
