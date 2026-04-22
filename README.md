# femsolver

`femsolver` is a greenfield repository for a self-owned, controllable, and explainable finite element solver that is intended to grow into a multiphysics coupled solver.

The current repository state implements the `v0.2.0` kernel-foundation baseline:

- repository governance and project conventions from `v0.1.0`
- a self-owned tetrahedral kernel slice for mesh, reference elements, quadrature, `H1` basis, scalar assembly, and a Poisson benchmark
- a CMake/CTest bootstrap for C++ applications and libraries
- Python tooling scaffolding for release and repository management
- GitHub Actions, CodeRabbit, SonarQube Cloud, and Codecov integration files
- a documented roadmap from `v0.1.0` through `v4.0.0`

The project direction is:

- own the kernel architecture, basis functions, assembly path, and solver interfaces
- use `MFEM` and `FEniCSx/DOLFINx` only as reference, verification, and benchmarking tools
- grow the kernel toward both tetrahedral and hexahedral cells, low- and high-order discretizations, and `H1`/`Nedelec`/`Raviart-Thomas` element families
- use a robot-joint permanent-magnet motor as the first application MVP
- expand from electromagnetics toward a multiphysics coupled solver and, later, physics-AI workflows

## Repository Status

- Current delivered milestone: `v0.2.0`
- Current focus: self-owned kernel foundations that lead toward `H(curl)` electromagnetics and later multiphysics
- Implemented now: tetra mesh container, reference tetrahedron, centroid quadrature, linear `H1` basis, scalar Poisson local/global assembly, Dirichlet elimination, and a canonical benchmark wired into `motor_check`
- Explicitly not implemented yet: `H(curl)` basis, electromagnetic matrix assembly, nonlinear magnetic materials, or coupled multiphysics behavior

## Quick Start

Configure, build, and test with CMake presets:

```bash
cmake --preset gcc-debug
cmake --build --preset gcc-debug
ctest --preset gcc-debug
```

The `clang-asan` and `gcc-coverage` presets are available for CI-style checks.

## Project Guides

- Engineering rules: `Agent.md`
- Product and release plan: `ROADMAP.md`
- Change history: `CHANGELOG.md`
- Implementation docs index: `docs/implementation/README.md`
- Functional architecture: `docs/architecture/functional-architecture.md`
- Kernel architecture: `docs/architecture/kernel-architecture.md`
- Kernel module boundaries: `docs/architecture/kernel-module-boundaries.md`
- Element-family and order strategy: `docs/architecture/element-family-and-order-strategy.md`
- Multiphysics and physics-AI expansion: `docs/architecture/multiphysics-and-physics-ai.md`
- Physical architecture: `docs/physics/physical-architecture.md`
- GitHub administration notes: `docs/governance/github-admin.md`
- Draft release summaries: `docs/releases/v0.1.0.md`, `docs/releases/v0.2.0.md`

## Repository Layout

Key directories:

- `src/` and `include/`: C++ sources and public headers
- `tests/`: unit, contract, integration, regression, and validation tests
- `tools/`: Python tooling for release notes, GitHub setup, Gmsh, and FEniCS helpers
- `cases/`: benchmark, machine, and regression inputs
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
