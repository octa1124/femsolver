# Kernel Module Boundaries

## Purpose

This note fixes the first concrete internal boundaries of the self-owned FEM kernel delivered in `v0.2.0`.

The main goal is to stop the project from drifting back into a loose collection of placeholders. Every kernel module now has an explicit ownership boundary, public interface surface, and allowed dependency direction.

## Boundary Summary

```text
kernel
├─ mesh
├─ reference
├─ quadrature
├─ basis
├─ algebra
├─ assembly
└─ benchmark
```

Allowed dependency direction:

```text
mesh        ─┐
reference   ─┼─> assembly ─> benchmark
quadrature  ─┤
basis       ─┤
algebra     ─┘
```

Additional rule:

- `io` and `src/app` may consume benchmark or solver results.
- No kernel module may depend on application code.
- Future physics modules depend on the kernel; the kernel does not depend on physics modules.

## Module Contracts

### `kernel/mesh`

Public interface:

- `Point3D`
- `TetraCell`
- `BoundaryFace`
- `TetraMesh`
- `BuildCentroidRefinedReferenceTetraMesh()`

Owns:

- nodal coordinates
- tetrahedral connectivity
- boundary-face connectivity
- positive-orientation normalization
- mesh validation
- boundary-node discovery

Does not own:

- basis functions
- quadrature
- physical coefficients
- element matrices
- solver iteration policy

### `kernel/reference`

Public interface:

- `ReferenceTetrahedron`

Owns:

- reference-cell coordinates
- reference-cell containment
- reference-cell volume

Does not own:

- physical meshes
- basis values
- quadrature rules
- mappings to global coordinates

### `kernel/quadrature`

Public interface:

- `TetrahedronQuadraturePoint`
- `TetrahedronQuadratureRule`
- `MakeCentroidTetrahedronQuadrature()`

Owns:

- quadrature points on the reference tetrahedron
- quadrature weights
- declared exactness degree

Does not own:

- basis functions
- element Jacobians
- problem-specific source terms

### `kernel/basis`

Public interface:

- `LagrangeP1Tetrahedron`

Owns:

- `H1` linear nodal basis values on the reference tetrahedron
- reference gradients

Does not own:

- DoF numbering beyond the local element
- physical gradients
- global assembly

### `kernel/algebra`

Public interface:

- `SparseMatrix`
- `SparseMatrixBuilder`
- `LinearSystem`
- `ConjugateGradientOptions`
- `ConjugateGradientResult`
- `SolveConjugateGradient()`

Owns:

- mutable sparse accumulation before finalization
- CSR-like finalized sparse storage
- linear-system storage
- minimal self-owned Krylov solve for the first benchmark

Does not own:

- mesh topology
- finite-element geometry
- PDE semantics

### `kernel/assembly`

Public interface:

- `ElementGeometry`
- `BuildElementGeometry()`
- `LocalPoissonElement`
- `DirichletBoundaryCondition`
- `GlobalLinearSystem`
- `AssemblePoissonElement()`
- `AssemblePoissonSystem()`
- `ApplyDirichletBoundary()`

Owns:

- geometric mapping from reference tetrahedron to physical cells
- local stiffness and load construction for the scalar Poisson problem
- scatter-add into a global sparse builder
- Dirichlet elimination on the mutable global system

Does not own:

- benchmark case generation
- CLI reporting
- motor-specific formulations

### `kernel/benchmark`

Public interface:

- `PoissonBenchmarkProblem`
- `PoissonBenchmarkResult`
- `MakeManufacturedLinearPoissonBenchmark()`
- `RunPoissonBenchmark()`

Owns:

- benchmark mesh selection
- manufactured exact solution
- end-to-end validation orchestration
- error summary for the first kernel checkpoint

Does not own:

- generalized application workflow
- solver-kernel building blocks already owned by lower layers

## Why The Boundaries Matter

These boundaries intentionally prepare the next steps:

- `v0.3.0` can add `H(curl)` basis and assembly without polluting mesh or algebra modules.
- `v1.x` motor physics can live above assembly contracts instead of reshaping the kernel around one application.
- later multiphysics and physics-AI work can consume stable data structures and benchmark hooks rather than reverse-engineering ad hoc internal state.
