# ADR 0003: Tetrahedral `P1` Kernel Foundation Before Vector Electromagnetics

## Status

Accepted

## Context

After `v0.1.0`, the repository had governance and placeholder code but not a real self-owned numerical path.

The next question was whether to jump directly into magnetostatic `H(curl)` work or first implement a smaller scalar kernel slice that the repository fully owns and understands.

## Decision

The project will implement `v0.2.0` as a tetrahedral `P1` scalar kernel foundation before adding vector electromagnetics.

The delivered module boundaries are:

- `mesh`
- `reference`
- `quadrature`
- `basis`
- `algebra`
- `assembly`
- `benchmark`

The first end-to-end problem is a manufactured Poisson benchmark.

## Consequences

### Positive

- the repository now owns a real local-to-global assembly path
- geometry, basis, and sparse-algebra contracts are explicit before `H(curl)` complexity arrives
- later vector and multiphysics work can extend stable layers instead of rewriting placeholders
- correctness can be checked on a controlled canonical problem

### Negative

- this milestone does not yet solve electromagnetic physics
- some placeholder application-level types remain ahead of full production use
- later work still needs edge topology, orientation handling, and vector basis support

## Follow-Up

The next numerical milestone is `v0.3.0`:

- edge-oriented tetrahedral topology details
- first-kind Nedelec basis
- curl-curl assembly
- canonical vector-field validation problems
