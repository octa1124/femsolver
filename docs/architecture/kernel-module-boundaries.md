# Kernel Module Boundaries

## Purpose

This note fixes the concrete internal boundaries of the self-owned FEM kernel after the completed `v0.2.0` scalar baseline and the first implemented `v0.3.0` vector-field slice.

The main goal is to stop the project from drifting back into a loose collection of placeholders. Every kernel module now has an explicit ownership boundary, public interface surface, and allowed dependency direction.

## Boundary Summary

```text
kernel
├─ common
├─ mesh
├─ reference
├─ quadrature
├─ basis
├─ space
├─ algebra
├─ assembly
└─ benchmark
```

Allowed dependency direction:

```text
common      ─┬─> mesh
            ├─> reference
            ├─> quadrature
            ├─> basis
            ├─> algebra
            └─> space

mesh/reference/quadrature/basis/algebra/space ─> assembly ─> benchmark
```

Additional rule:

- `io` and `src/app` may consume benchmark or solver results.
- No kernel module may depend on application code.
- `kernel/space` may depend on mesh topology and discretization descriptors, but it must not absorb PDE-specific assembly logic.
- Future physics modules depend on the kernel; the kernel does not depend on physics modules.

## Module Contracts

### `kernel/common`

Public interface:

- `CellType`
- `SpaceFamily`
- `PolynomialOrder`
- `ElementDescriptor`

Owns:

- shared discretization vocabulary across kernel modules
- cell-family identity
- FE-family identity
- polynomial-order identity

Does not own:

- geometry
- basis evaluation
- DoF maps
- assembly algorithms
- solver algorithms

### `kernel/mesh`

Public interface:

- current concrete types:
  - `Point3D`
  - `TetraCell`
  - `BoundaryFace`
  - `TetraEdge`
  - `TetraMesh`
  - `BuildCentroidRefinedReferenceTetraMesh()`
- future contract direction:
  - mesh interfaces must stay extendable to hexahedral cells without rewriting upper-layer assembly and space APIs

Owns:

- nodal coordinates
- tetrahedral connectivity
- boundary-face connectivity
- positive-orientation normalization
- mesh validation
- boundary-node discovery
- unique tetrahedral edge extraction
- boundary-edge discovery

Does not own:

- basis functions
- quadrature
- physical coefficients
- element matrices
- solver iteration policy

### `kernel/reference`

Public interface:

- current concrete type:
  - `ReferenceTetrahedron`
- future contract direction:
  - reference-cell interfaces must grow to tetrahedron and hexahedron families and be polynomial-order aware where needed

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

- current concrete types:
  - `TetrahedronQuadraturePoint`
  - `TetrahedronQuadratureRule`
  - `MakeCentroidTetrahedronQuadrature()`
- future contract direction:
  - quadrature selection must depend on cell family and requested exactness/order

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

- current concrete type:
  - `LagrangeP1Tetrahedron`
- current vector-field type:
  - `FirstOrderNedelecTetrahedron`
- future contract direction:
  - basis interfaces must grow to cover nodal `H1`, `Nedelec`, and `Raviart-Thomas` families on tetrahedral and hexahedral cells

Owns:

- `H1` linear nodal basis values on the reference tetrahedron
- reference gradients
- tetrahedral first-order `Nedelec` basis values on the reference tetrahedron
- constant reference curls for first-order tetrahedral `Nedelec`

Does not own:

- DoF numbering beyond the local element
- physical gradients
- entity orientation
- global assembly

### `kernel/space`

Public interface:

- `DofEntityKind`
- `SignedGlobalDof`
- `TetraDofMap`
- `BuildLagrangeP1DofMap()`
- `BuildFirstOrderNedelecDofMap()`

Owns:

- local-to-global DoF numbering
- orientation-aware sign handling for edge DoFs
- boundary-DoF discovery expressed in solver-facing numbering

Does not own:

- basis-function evaluation
- reference-cell definitions
- physical coefficients
- element matrices
- benchmark exact solutions

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
- `LocalCurlCurlElement`
- `DirichletBoundaryCondition`
- `EdgeDirichletBoundaryCondition`
- `GlobalLinearSystem`
- `AssemblePoissonElement()`
- `AssemblePoissonSystem()`
- `AssembleCurlCurlElement()`
- `AssembleCurlCurlSystem()`
- `ApplyDirichletBoundary()`
- `ApplyEdgeDirichletBoundary()`

Owns:

- geometric mapping from reference tetrahedron to physical cells
- local stiffness and load construction for the scalar Poisson problem
- `H(curl)` mapping from reference vectors and reference curls into the physical element
- local stiffness and load construction for the tetrahedral curl-curl operator
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
- `CurlCurlBenchmarkProblem`
- `CurlCurlBenchmarkResult`
- `MakeManufacturedLinearPoissonBenchmark()`
- `MakeManufacturedCurlCurlBenchmark()`
- `RunPoissonBenchmark()`
- `RunCurlCurlBenchmark()`

Owns:

- benchmark mesh selection
- manufactured exact solution
- end-to-end validation orchestration for scalar and vector canonical cases
- error summary for the first scalar and vector kernel checkpoints

Does not own:

- generalized application workflow
- solver-kernel building blocks already owned by lower layers

## Why The Boundaries Matter

These boundaries intentionally prepare the next steps:

- the first `v0.3.0` vector-field slice can add `H(curl)` basis and assembly without polluting mesh or algebra modules.
- the dedicated `kernel/space` layer keeps global numbering and orientation logic out of both basis evaluation and PDE assembly.
- the same boundaries also leave room for hexahedral and higher-order elements instead of forcing a tetrahedral-only architecture.
- `v1.x` motor physics can live above assembly contracts instead of reshaping the kernel around one application.
- later multiphysics and physics-AI work can consume stable data structures and benchmark hooks rather than reverse-engineering ad hoc internal state.
