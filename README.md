# femsolver

`femsolver` is a self-owned, controllable, and explainable finite element solver project that is intended to grow into a multiphysics coupled solver.

The repository now contains two delivered layers:

- the completed `v0.2.0` scalar-kernel baseline
- the first implemented slice of the in-progress `v0.3.0` vector-field and case-preprocessing baseline

That means the repository already includes:

- repository governance and project conventions from `v0.1.0`
- a self-owned tetrahedral scalar kernel for mesh, reference elements, quadrature, `H1` basis, scalar assembly, sparse algebra, and a Poisson benchmark
- the first self-owned tetrahedral `H(curl)` slice with edge topology, edge DoF maps, first-order `Nedelec` basis functions, curl-curl assembly, and a canonical vector benchmark
- a tracked machine-case catalog for robot-joint motors and an industrial envelope case
- a simplified Gmsh-oriented preprocessing path that emits `.geo`, manifest, and summary artifacts through `motor_pre`
- a CMake/CTest bootstrap for C++ applications and libraries
- Python tooling scaffolding for release, repository management, and preprocessing
- GitHub Actions, CodeRabbit, SonarQube Cloud, and Codecov integration files
- a documented roadmap from `v0.1.0` through `v4.0.0`

The project direction is:

- own the kernel architecture, basis functions, assembly path, and solver interfaces
- use `MFEM` and `FEniCSx/DOLFINx` only as reference, verification, and benchmarking tools
- grow the kernel toward both tetrahedral and hexahedral cells, low- and high-order discretizations, and `H1`/`Nedelec`/`Raviart-Thomas` element families
- use a robot-joint permanent-magnet motor as the first application MVP
- expand from electromagnetics toward a multiphysics coupled solver and, later, physics-AI workflows

## Repository Status

- Latest completed milestone: `v0.2.0`
- Current development baseline: `v0.3.0-dev`
- Current focus: self-owned vector-field FEM foundations and real machine-case preprocessing that lead toward a robot-joint motor electromagnetics MVP
- Implemented now:
  - tetra mesh container with cell orientation normalization, edge extraction, boundary-node discovery, and boundary-edge discovery
  - reference tetrahedron, centroid quadrature, linear `H1` basis, scalar Poisson local/global assembly, Dirichlet elimination, and a canonical scalar benchmark wired into `motor_check`
  - tetra first-order `Nedelec` basis, edge-based DoF maps, Piola-style `H(curl)` mapping support, curl-curl local/global assembly, and a canonical vector-field benchmark
  - four tracked machine cases under `cases/machines/` with source URLs, reconstruction levels, declared variants, and simplified-geometry assumptions
  - `motor_pre --case --variant --output-dir` for simplified geometry generation, optional Gmsh invocation, manifest writing, and summary reporting
  - native C++ loading of `case.yaml` and generated `mesh_manifest.yaml`
  - a first linear magnetostatic smoke solve in `motor_solve` that binds real case and manifest semantics, assembles `J` and `B_r` sources, and solves on a synthetic region-tagged kernel mesh
- Explicitly not implemented yet:
  - imported `.msh` data in the self-owned mesh layer for machine solves
  - detailed slot/tooth/magnet OCC geometry reconstruction
  - machine-grade field post-processing and torque evaluation on real machine meshes
  - nonlinear magnetic materials or coupled multiphysics behavior

## Quick Start

Configure, build, and test with CMake presets:

```bash
cmake --preset gcc-debug
cmake --build --preset gcc-debug
ctest --preset gcc-debug
```

The `clang-asan` and `gcc-coverage` presets are available for CI-style checks.

Generate a simplified machine-preprocessing artifact set:

```bash
./build/gcc-debug/src/motor_pre \
  --case cases/machines/joint_type_i_12s10p/case.yaml \
  --variant section_2d \
  --output-dir build/generated/joint_type_i_12s10p_section_2d
```

Run the canonical scalar and vector kernel checks:

```bash
./build/gcc-debug/src/motor_check
```

Run the current linear joint-motor smoke solve after preprocessing:

```bash
./build/gcc-debug/src/motor_solve \
  --case cases/machines/joint_type_i_12s10p/case.yaml \
  --manifest build/generated/joint_type_i_12s10p_extruded_3d/mesh_manifest.yaml
```

## Project Guides

- Engineering rules: `Agent.md`
- Active delivery tracker: `TODO.md`
- Product and release plan: `ROADMAP.md`
- Change history: `CHANGELOG.md`
- Implementation docs index: `docs/implementation/README.md`
- Case catalog and preprocessing implementation: `docs/implementation/case-catalog-and-preprocessing.md`
- Vector-kernel implementation baseline: `docs/implementation/v0.3.0-vector-kernel-baseline.md`
- Linear magnetostatic smoke path: `docs/implementation/linear-magnetostatic-smoke-path.md`
- Functional architecture: `docs/architecture/functional-architecture.md`
- Kernel architecture: `docs/architecture/kernel-architecture.md`
- Kernel module boundaries: `docs/architecture/kernel-module-boundaries.md`
- Element-family and order strategy: `docs/architecture/element-family-and-order-strategy.md`
- Multiphysics and physics-AI expansion: `docs/architecture/multiphysics-and-physics-ai.md`
- Physical architecture: `docs/physics/physical-architecture.md`
- GitHub administration notes: `docs/governance/github-admin.md`
- Draft release summaries: `docs/releases/v0.1.0.md`, `docs/releases/v0.2.0.md`, `docs/releases/v0.3.0.md`

## Repository Layout

Key directories:

- `src/` and `include/`: C++ sources and public headers
- `tests/`: unit, contract, integration, regression, and validation tests
- `tools/`: Python tooling for release notes, GitHub setup, Gmsh, and FEniCS helpers
- `cases/`: benchmark, machine, and regression inputs, including the tracked robot-joint motor case catalog
- `docker/`: dev and CI container images
- `.github/`: Actions workflows, issue templates, PR templates, and repository settings

## GitHub-Side Setup

Some repository management items require admin access and cannot be fully applied from local files alone:

- GitHub label and milestone creation
- branch protection and rulesets
- GitHub App installation for CodeRabbit
- repository secrets for SonarQube Cloud and Codecov when needed

This repository includes declarative config and helper scripts for those steps:

- `.github/repository-settings/labels.yaml`
- `.github/repository-settings/milestones.yaml`
- `.github/repository-settings/ruleset-main.json`
- `tools/scripts/github_bootstrap.py`

## License

This repository is licensed under the MIT License. See `LICENSE`.
