# Kernel Architecture

## Purpose

This document describes the planned internal layers of the self-owned solver kernel and the currently implemented path through those layers.

The kernel must remain controllable and explainable. External libraries may help with benchmarking or selected linear-algebra backends, but the core numerical semantics should belong to this repository.

## Design Goals

- own the finite-element abstractions
- keep the assembly path explicit and inspectable
- support multiple cell families, space families, and polynomial orders under repository-owned interfaces
- support future multiphysics block systems
- support later reduced-order and physics-AI workflows through stable interfaces

## Outside-The-Kernel Boundary

The kernel is not the whole application stack.

Case metadata, simplified motor geometry generation, and optional Gmsh invocation currently live above the kernel in:

- `cases/machines/`
- `tools/gmsh/`
- `src/app/motor_pre_main.cpp`

This boundary is deliberate. The kernel should own discretization and operator semantics, while case-catalog and preprocessing code own source provenance, reconstruction assumptions, and geometry-script generation.

## Kernel Layers

### Mesh And Topology

Responsibilities:

- vertices, edges, faces, cells
- region and boundary tags
- topology queries
- orientation data
- local geometry and mappings

Initial scope:

- tetrahedral meshes first
- hexahedral support as a permanent target, not an afterthought
- controlled import path from external mesh generators

### Reference Elements And Quadrature

Responsibilities:

- reference tetrahedron definitions
- reference hexahedron definitions
- quadrature points and weights
- shape-function evaluation points
- geometric Jacobians and transforms

Initial scope:

- linear tetrahedron
- hexahedron and order-aware rule interfaces next
- quadrature sufficient for scalar and later vector-field MVPs

### Finite-Element Spaces

Responsibilities:

- scalar `H1` support
- `H(curl)` support
- `H(div)` support
- DoF numbering and ownership
- constraint and boundary-condition handling
- polynomial-order awareness

Initial scope:

- `H1` delivered in `v0.2.0`
- first tetrahedral `H(curl)` delivered as the early `v0.3.0` slice
- `H(div)` must be part of the kernel plan because flux-conforming post-processing and multiphysics coupling will need it

### Assembly Pipeline

Responsibilities:

- cell integrals
- boundary integrals
- source-term assembly
- scatter from local element contributions to global sparse structures

Design rule:

- all important local contributions must remain inspectable and testable

### Algebra And Solvers

Responsibilities:

- sparse matrix storage or wrappers
- linear solve interfaces
- nonlinear residual and Jacobian interfaces
- later block-system composition and time stepping

Design rule:

- if external algebra backends are introduced, they should sit behind repository-owned interfaces

### Physics Modules

Responsibilities:

- electromagnetics
- thermal
- mechanical
- coupled operators

Design rule:

- physics modules depend on the kernel
- the kernel does not depend on any one physics module

## Reference Tool Role

`MFEM` and `FEniCSx/DOLFINx` may help with:

- confirming weak forms
- validating canonical cases
- checking convergence behavior

They must not define the production kernel abstractions.

## AI And Reduced-Model Role

Future AI-facing workflows should consume outputs from the kernel through stable interfaces such as:

- case definitions
- solution snapshots
- operator-evaluation hooks
- reduced-order export data

The kernel remains the trusted physical reference even when learned components are introduced later.

## Current Concrete Module Map

The currently implemented path spans the completed `v0.2.0` scalar baseline and the first `v0.3.0` vector extension.

See:

- `docs/architecture/kernel-module-boundaries.md`
- `docs/architecture/element-family-and-order-strategy.md`
- `docs/implementation/v0.2.0-kernel-foundation.md`
- `docs/implementation/v0.3.0-vector-kernel-baseline.md`

Current concrete ownership now covers:

- `common`
- `mesh`
- `reference`
- `quadrature`
- `basis`
- `space`
- `algebra`
- `assembly`
- `benchmark`

Concretely, this means the repository already owns:

- tetrahedral scalar `H1` benchmark assembly
- tetrahedral edge topology and edge-based DoF numbering
- tetrahedral first-order `Nedelec` basis evaluation and curl handling
- scalar and vector canonical benchmark orchestration
