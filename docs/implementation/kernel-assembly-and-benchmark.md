# Kernel Assembly, Algebra, And Benchmark

## Scope

This note covers the parts of `v0.2.0` that actually solve the first canonical PDE:

- geometric mapping
- local Poisson assembly
- global sparse assembly
- Dirichlet elimination
- conjugate-gradient solve
- manufactured benchmark evaluation

## Element Geometry

`BuildElementGeometry()` constructs:

- the cell Jacobian
- the inverse Jacobian
- the determinant
- the physical cell volume
- physical gradients obtained from `J^{-T} * grad_ref(phi)`

This function is the bridge between the reference-domain modules and the physical mesh.

## Local Poisson Element

`AssemblePoissonElement()` computes:

- local stiffness matrix
- local load vector

For the current scalar problem, the element matrix is:

`K_ij = integral(grad(phi_i) · grad(phi_j) dOmega)`

The load vector is:

`f_i = integral(s * phi_i dOmega)`

where `s` is the scalar source function.

## Global Assembly

`AssemblePoissonSystem()` loops over cells and performs:

- element-geometry construction
- local matrix/vector assembly
- scatter-add into `SparseMatrixBuilder`
- accumulation into the global right-hand side

This is the first concrete local-to-global assembly contract owned by the repository.

## Dirichlet Elimination

`ApplyDirichletBoundary()` acts on the mutable global system before CSR finalization:

1. compute prescribed nodal values
2. remove boundary-column couplings from all non-boundary rows
3. shift their effect into the right-hand side
4. replace each boundary row by an identity row

This choice is intentionally simple and explicit. It is not yet optimized for large systems, but it is easy to audit and test.

## Sparse Algebra

The algebra layer currently provides:

- `SparseMatrixBuilder` for mutable row-wise accumulation
- `SparseMatrix` for finalized CSR-like storage
- `SolveConjugateGradient()` for the first SPD benchmark

What it can do now:

- accumulate sparse entries
- finalize sparse structure
- run a minimal self-owned Krylov solve on the assembled benchmark

What it cannot do yet:

- preconditioning
- block systems
- nonlinear linearization support
- distributed memory

## Poisson Benchmark

The first canonical benchmark is a manufactured harmonic solution on a centroid-refined reference tetrahedron.

Exact solution:

- `u(x, y, z) = x + 2y + 3z`

Source:

- `0`

Boundary treatment:

- Dirichlet values are applied on all outer boundary nodes

Why this benchmark is useful:

- the exact solution is linear and belongs to the `P1` finite-element space
- the benchmark exercises one interior unknown
- the result should reproduce exact nodal values to floating-point tolerance if the kernel path is correct

## Verification

Current tests verify:

- local stiffness symmetry
- local zero row sum for the Laplacian
- Dirichlet rows become identity rows
- the manufactured benchmark converges
- the interior-node value matches the exact solution
