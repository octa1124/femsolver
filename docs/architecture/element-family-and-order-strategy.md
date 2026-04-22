# Element Family And Order Strategy

## Purpose

This note records a top-level discretization decision for `femsolver`:

- the project will not stop at tetrahedral first-order elements
- the kernel must eventually support both tetrahedral and hexahedral cells
- the kernel must eventually support both low-order and higher-order approximation
- the kernel must eventually support ordinary nodal `H1`, `Nedelec`, and `Raviart-Thomas` families

## Why This Matters

The first delivered kernel slice in `v0.2.0` is intentionally small and auditable: tetrahedral `P1 H1` with a scalar Poisson benchmark.

That is a good bootstrap step, but it is not the whole numerical strategy. For electrical-machine and later multiphysics work:

- tetrahedra are flexible for complex geometry and fast iteration
- hexahedra can be substantially more efficient when geometry and meshing quality allow them
- higher-order elements can reduce mesh counts and improve accuracy-per-unknown when the field is smooth enough
- `Nedelec` and `Raviart-Thomas` spaces are not optional extras; they are part of a serious electromagnetics and multiphysics kernel

## Product-Level Target

The intended discretization envelope is:

- cells: tetrahedron and hexahedron
- orders: low order first, then systematic higher-order extension
- spaces:
  - `H1` nodal
  - `Nedelec` for `H(curl)`
  - `Raviart-Thomas` for `H(div)`

This does not mean every family must be implemented in the same milestone. It means the architecture cannot be allowed to exclude them.

## Interface Rule

Cross-module interfaces should describe discretization through explicit metadata:

- cell type
- finite-element family
- polynomial order

That is why the kernel now uses a shared descriptor vocabulary under:

- `include/femsolver/kernel/common/discretization_descriptor.hpp`

The practical rule is simple:

- concrete implementations may be tetrahedral and low-order at first
- upper-layer interfaces must not pretend the final product is tetrahedral and low-order only

## Staged Rollout

### Delivered In `v0.2.0`

- tetrahedral scalar `P1 H1` slice
- explicit kernel boundaries
- first end-to-end benchmark

### Next In `v0.3.0`

- generic element descriptor vocabulary
- vector-field infrastructure
- first tetrahedral `Nedelec` implementation
- interfaces that are ready for hexahedral and higher-order growth

The current repository state has already started this stage by adding:

- tetrahedral edge topology
- a dedicated `kernel/space` module
- first-order tetrahedral `Nedelec`
- a canonical curl-curl benchmark

### After `v0.3.0`

The roadmap should continue in a controlled order:

1. broaden cell support from tetrahedral-only to tetrahedral plus hexahedral
2. broaden function-space support from current subsets toward full `H1`/`Nedelec`/`Raviart-Thomas`
3. add higher-order basis and quadrature without changing upper-layer contracts
4. let application solvers choose validated subsets instead of hard-coding the global product limit

## Consequence For The Motor MVP

The first robot-joint motor MVP does not need to ship every element family on day one.

But the MVP implementation must be built on a kernel that can later absorb:

- hexahedral efficiency-oriented workflows
- higher-order discretizations to reduce mesh count
- `RT`-based post-processing and coupled-field work

This is the difference between building a one-off application and building a solver platform.
