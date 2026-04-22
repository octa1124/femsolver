# Agent.md

This file defines the repository engineering rules for `femsolver`. It is the highest-priority project convention file inside the repository.

## Mission

`femsolver` is being built as a self-owned, controllable, and explainable finite element solver that is intended to expand into a multiphysics coupled solver.

The current implementation target is `v0.3.0`, which extends the delivered `v0.2.0` tetrahedral scalar kernel toward a generic, self-owned discretization architecture that can host tetrahedral and hexahedral elements, multiple polynomial orders, and multiple finite-element families.

## Version Scope

- `v0.1.0`: governance, tooling, build/test automation, repository management
- `v0.2.0`: self-owned kernel foundation with tetra mesh, quadrature, `H1`, and scalar assembly
- `v0.3.0`: generic element/space/order contracts plus vector-field FEM foundation with `H(curl)` support
- `v1.0.0`: robot-joint motor electromagnetics MVP
- `v1.1.0`: nonlinear and anisotropic magnetic materials plus broader element-family expansion
- `v2.0.0`: multiphysics coupling foundation
- `v3.0.0`: design-study and physics-AI interfaces
- `v4.0.0`: motion and low-frequency transients

## Repository Structure

- `src/` and `include/`: production C++ code
- `tests/`: all automated tests
- `tools/`: repository tooling, Gmsh helpers, FEniCS helpers, release tooling
- `cases/`: benchmark, machine, and regression cases
- `docs/`: ADRs, architecture notes, physics notes, and release summaries
- `docker/`: reproducible development and CI images
- `.github/`: workflows, templates, and repository bootstrap settings

## Kernel Module Boundaries

The `v0.2.0` kernel foundation is organized so later `H(curl)`, multiphysics, and AI-facing work can grow on top of stable internal contracts.

- `kernel/common` owns shared discretization descriptors such as cell type, space family, and polynomial order.
- `kernel/mesh` owns only mesh geometry, topology, orientation normalization, and boundary-node discovery.
- `kernel/reference` owns only reference-cell definitions.
- `kernel/quadrature` owns only quadrature rules on reference cells.
- `kernel/basis` owns only basis-function values and gradients on reference cells.
- `kernel/algebra` owns only sparse-matrix and linear-solver primitives.
- `kernel/assembly` may depend on `mesh`, `reference`, `quadrature`, `basis`, and `algebra`, and owns local/global assembly plus boundary-condition application.
- `kernel/benchmark` may depend on all kernel modules and owns canonical verification problems, but it must not contain application CLI logic.
- Application entrypoints under `src/app` and reporting under `io` may consume benchmark results, but they must not back-feed policies into kernel internals.

Hard dependency rules:

- `kernel/common` may be consumed by all kernel modules, but it must remain lightweight and algorithm-free.
- `kernel/mesh` must not depend on `basis`, `assembly`, `benchmark`, or application modules.
- `kernel/reference`, `kernel/quadrature`, and `kernel/basis` must remain reference-domain abstractions and must not depend on physical meshes.
- `kernel/algebra` must not depend on mesh, basis, material, or application code.
- Physics modules must depend on kernel layers; kernel layers must not depend on any physics module.

## Coding Standards

### C++

- Use `C++17`.
- Prefer clear domain names over abbreviated numerics-only names.
- Keep public headers under `include/femsolver/...`.
- Keep implementation details in `src/...`.
- Avoid implicit ownership; prefer value types or `std::unique_ptr`.
- New solver-facing APIs must be documented in headers.
- Do not introduce third-party dependencies casually; justify new dependencies in an ADR when they affect build or runtime behavior.

### Python

- Target Python `3.11+`.
- Keep scripts small, testable, and usable in CI.
- Prefer standard library first; document every extra dependency in `pyproject.toml`.
- All scripts used by CI must support `--help`.

### CMake

- Use target-based CMake.
- Do not use directory-wide global compile flags when a target-level rule is sufficient.
- Presets are the canonical interface for local and CI builds.

### YAML And Config

- Prefer stable, explicit keys.
- Case schema changes must be backwards-aware and documented.

## Numerical And Physics Constraints

- Do not hard-code mesh attribute integers in solver code.
- Do not place Gmsh geometry generation logic inside the solver core.
- The kernel path must remain self-owned and explainable: mesh semantics, basis definitions, quadrature, DoF maps, and assembly contracts may not be outsourced to MFEM or FEniCS.
- Cross-module discretization interfaces must not hard-code "tetrahedron only", "first order only", or "single FE family only" assumptions above the concrete implementation layer.
- The long-term discretization target includes tetrahedral and hexahedral cells, low- and high-order approximation, and at least `H1` nodal, `Nedelec`, and `Raviart-Thomas` families.
- `MFEM` and `FEniCSx/DOLFINx` may be used for study, benchmark comparison, and cross-check tooling, but they must not become the production kernel without an explicit ADR.
- Module-boundary changes inside `kernel/` must update the architecture and implementation notes that describe those boundaries.
- Any change to `material`, `fem`, `nonlinear`, or `post` must ship with tests.
- Any change to `B-H` constitutive behavior must ship with a consistent-tangent validation.
- Important numerical-strategy changes require an ADR under `docs/adr/`.

## Testing Obligations

- New modules require unit tests.
- Parser and schema changes require contract tests.
- Application-level behavior changes require at least one integration or regression test.
- Python tooling changes require `pytest` coverage.
- CI must stay green on all required checks before merge.

## Technical Documentation Obligations

- Every meaningful implementation step must have technical documentation at an appropriate level of detail.
- For repository and infrastructure work, document the implemented subsystem under `docs/implementation/`.
- For solver, material, nonlinear, mesh, or post-processing work, document the design, inputs, outputs, data flow, edge cases, and verification strategy.
- If a change is too small for a new standalone document, update the nearest existing implementation document explicitly.
- Public workflow changes must update `README.md` or a governance document in addition to code comments.

## Pull Request Obligations

Every PR must:

- explain the problem and solution
- include test evidence
- include a changelog fragment under `changes/unreleased/`
- update docs if schema, workflow, or public behavior changes
- update or add implementation technical documentation for the affected workstream

Do not merge PRs that:

- break required checks
- change core physics behavior without tests
- modify outputs or schemas without changelog and documentation updates
- add non-trivial implementation work without corresponding technical documentation

## Changelog Rule

- `CHANGELOG.md` follows Keep a Changelog + SemVer.
- Each PR must add one fragment file under `changes/unreleased/`.
- Release notes are generated from fragments and curated into `docs/releases/vX.Y.Z.md`.

## Release Summary Template

Every release summary must include:

- Completed
- Not Completed
- Known Risks
- Compatibility Changes
- Next Version Handoff

## Version Completion And Git History

- Every completed version milestone must end with a git commit that captures the finished repository state.
- Release completion should use a version-aligned commit message and, when appropriate, a tag created from that committed state.
- Do not treat a version as complete if the code, docs, changelog, and release summary are not committed together.

## Code Review Bot Rules

`CodeRabbit` is the primary AI review bot for this repository.

Review guidance for bots and humans:

- enforce `Agent.md`
- prioritize correctness, test coverage, maintainability, and repository conventions
- avoid requesting changes that contradict the roadmap or version scope
- ignore generated artifacts, mesh outputs, result files, logs, and coverage artifacts

## Artifact Hygiene

- Generated meshes, result files, logs, coverage outputs, and temporary build products must not be committed into source directories.
- Use `build/`, `.coverage/`, `.pytest_cache/`, `results/`, or dedicated output directories outside tracked source paths.
