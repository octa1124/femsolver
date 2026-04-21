# Kernel Architecture

## Purpose

This document describes the planned internal layers of the self-owned solver kernel.

The kernel must remain controllable and explainable. External libraries may help with benchmarking or selected linear-algebra backends, but the core numerical semantics should belong to this repository.

## Design Goals

- own the finite-element abstractions
- keep the assembly path explicit and inspectable
- support future multiphysics block systems
- support later reduced-order and physics-AI workflows through stable interfaces

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
- controlled import path from external mesh generators

### Reference Elements And Quadrature

Responsibilities:

- reference tetrahedron definitions
- quadrature points and weights
- shape-function evaluation points
- geometric Jacobians and transforms

Initial scope:

- linear tetrahedron
- quadrature sufficient for scalar and later vector-field MVPs

### Finite-Element Spaces

Responsibilities:

- scalar `H1` support
- later `H(curl)` support
- DoF numbering and ownership
- constraint and boundary-condition handling

Initial scope:

- `H1` first
- `H(curl)` next because of the motor MVP

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
