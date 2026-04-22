# Kernel Reference, Quadrature, And Basis

## Scope

This note covers the reference-domain modules implemented in `v0.2.0`:

- `kernel/reference`
- `kernel/quadrature`
- `kernel/basis`

## Reference Tetrahedron

The reference tetrahedron is the standard simplex:

- vertex 0: `(0, 0, 0)`
- vertex 1: `(1, 0, 0)`
- vertex 2: `(0, 1, 0)`
- vertex 3: `(0, 0, 1)`

It currently provides:

- vertex coordinates
- cell volume `1/6`
- containment checks in barycentric form

It does not yet provide:

- higher-order interpolation points
- edge or face orientation metadata
- quadrature ownership

## Quadrature Rule

The first rule is a one-point centroid rule:

- point: `(1/4, 1/4, 1/4)`
- weight: `1/6`
- declared exactness: degree 1

This rule is sufficient for the current milestone because:

- the source term in the manufactured benchmark is zero
- local stiffness for `P1` Poisson uses constant physical gradients
- the first goal is interface completeness and assembly-path verification, not broad rule coverage

What is not implemented yet:

- higher-order tetrahedral quadrature
- surface quadrature
- rule selection by polynomial degree

## `H1` Linear Basis

The implemented basis is the nodal `P1` basis on the reference tetrahedron:

- `phi0 = 1 - xi - eta - zeta`
- `phi1 = xi`
- `phi2 = eta`
- `phi3 = zeta`

Reference gradients are constant:

- `grad(phi0) = (-1, -1, -1)`
- `grad(phi1) = (1, 0, 0)`
- `grad(phi2) = (0, 1, 0)`
- `grad(phi3) = (0, 0, 1)`

What this basis module owns:

- reference-cell values
- reference-cell gradients

What it does not own:

- physical gradients
- global DoF maps
- continuity constraints beyond one element

## Verification

Current tests verify:

- quadrature weight equals the reference volume
- the centroid lies inside the reference tetrahedron
- basis values form a partition of unity
- basis gradients match the expected `P1` reference gradients
