# Solver Roadmap

The solver roadmap is intentionally staged:

- `v0.1.0`: repository and quality baseline
- `v0.2.0`: self-owned kernel foundation for scalar FEM
- `v0.3.0`: self-owned vector-field FEM foundation with `H(curl)`
- `v1.0.0`: robot-joint motor electromagnetics MVP
- `v1.1.0`: nonlinear and anisotropic magnetic materials
- `v2.0.0`: multiphysics coupling foundation
- `v3.0.0`: design-study and physics-AI interfaces
- `v4.0.0`: motion and low-frequency transient infrastructure

`MFEM` and `FEniCSx/DOLFINx` are reference and validation tools. The production path is a self-owned kernel.

See also:

- `docs/architecture/functional-architecture.md`
- `docs/architecture/kernel-architecture.md`
- `docs/architecture/kernel-module-boundaries.md`
- `docs/architecture/multiphysics-and-physics-ai.md`
- `docs/physics/physical-architecture.md`
- `docs/implementation/v0.2.0-kernel-foundation.md`
- `docs/implementation/v1.0.0-solver-implementation-path.md`
