# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/)
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

- A tracked machine-case catalog for `joint_type_i_12s10p`, `exo_outer_rotor_36s40p`, `quadruped_joint_36s32p`, and `tbm76_envelope`, including source provenance and simplification notes.
- A simplified Gmsh-oriented preprocessing path that generates geometry scripts, manifests, and summaries through `motor_pre`.
- The first `v0.3.0` vector-kernel slice with tetra edge topology, edge DoF maps, first-order `Nedelec` basis functions, curl-curl assembly, and a canonical `H(curl)` benchmark.
- Native C++ loading for the current `case.yaml` and generated `mesh_manifest.yaml` contracts.
- A linear magnetostatic source assembly path for current density and remanent flux density, plus the first machine-coupled smoke solve in `motor_solve`.
- A first Gmsh `MSH2` tetrahedral mesh import path plus recovered flux-density summaries for the joint-motor smoke solve.
- An explicit joint-motor linear profile-binding layer that maps current case metadata into solver-owned material and excitation coefficients.
- A first `airgap_torque_surface` preprocessing contract, Maxwell-stress-style torque estimate, and `motor_check --machine-regression` gate for `joint_type_i_12s10p`.
- First `v2.0` coupling contracts for field state, callback-based physics operators, and coupled assembly aggregation.
- Non-visual analysis tools for machine report completion scoring and C++ inheritance-depth checks.
- `v1.1` nonlinear material MVP with monotone B-H curves, orthotropic response, energy evaluation, and finite-difference tangent checks.
- `v1.2` tetrahedral `RT0` reference basis plus H(div) face-flux/divergence benchmark exposed through `motor_check --hdiv-benchmark`.
- `v1.3` hexahedral reference element, Gauss-Legendre quadrature, and H1 `Q1/Q2` basis functions.
- `v2.0` operator adapters for magnetostatic block embedding and canonical magneto-thermal coupling.
- Version-readiness analysis tooling for the `v1.0` through `v2.0` feature chain.

### Changed

- Case metadata now declares the intended production discretization family as `nedelec` for the robot-joint motor path.
- `motor_check` now accepts combinable benchmark flags and runs both scalar and vector canonical checks by default.
- `motor_check` now includes the H(div) benchmark in default checks and accepts `--hdiv-benchmark`.
- `motor_check` now accepts `--machine-regression --case <path> --manifest <path>` for the first machine scalar regression gate.
- `motor_solve` now accepts `--case` and `--manifest` and can execute the current linear joint-motor smoke path instead of acting only as a placeholder.
- The active delivery tracker now splits `v1.1`, `v1.2`, `v1.3`, and `v2.0` into explicit completed foundation slices with remaining production gaps.

### Docs

- Added detailed technical documentation for the machine-case catalog, preprocessing path, and the current `v0.3.0` vector-kernel baseline.
- Added a dedicated implementation note for the linear magnetostatic smoke path and solver-side case/manifest loading.
- Added a `v1` to `v2` readiness review that narrows the multiphysics coupling design around field-state and physics-operator contracts.
- Added implementation documentation for the first coupling foundation and text-based analysis framework.
- Added implementation documentation for nonlinear B-H materials, RT/H(div), and hex/high-order H1 foundations.

## [0.2.0] - 2026-04-22

### Added

- Self-owned kernel modules for tetra mesh, reference tetrahedron, quadrature, linear `H1` basis, sparse algebra, scalar assembly, and a Poisson benchmark.
- Technical documentation and ADR coverage for the `v0.2.0` kernel-foundation milestone.
- Unit tests and CLI integration coverage for the first end-to-end self-owned FEM path.

### Changed

- `motor_check` now runs the canonical Poisson benchmark instead of acting only as a placeholder reporter.

## [0.1.0] - Draft

### Added

- Initial repository structure.
- Governance documents, changelog workflow, and release templates.
- CMake/CTest bootstrap, minimal applications, unit tests, and Python tooling.
- GitHub Actions, CodeRabbit, SonarQube Cloud, and Codecov configuration files.
