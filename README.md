# femsolver

`femsolver` is a greenfield repository for a finite element solver and validation platform focused on 3D electrical machine magnetostatics.

The current repository state implements the `v0.1.0` engineering baseline:

- repository governance and project conventions
- a CMake/CTest bootstrap for C++ applications and libraries
- Python tooling scaffolding for release and repository management
- GitHub Actions, CodeRabbit, SonarQube Cloud, and Codecov integration files
- a documented roadmap from `v0.1.0` through `v4.0.0`

The production physics stack is planned around:

- `MFEM` as the production finite element kernel
- `FEniCSx/DOLFINx` as a validation sidecar for canonical problems
- a 3D IPM machine roadmap that begins with locked-rotor magnetostatics

## Repository Status

- Current milestone: `v0.1.0`
- Current focus: governance, build/test infrastructure, and repository automation
- Explicitly not implemented yet: MFEM coupling, Gmsh geometry generation, nonlinear `B-H`, or electrical-machine physics

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
- Physical architecture: `docs/physics/physical-architecture.md`
- GitHub administration notes: `docs/governance/github-admin.md`
- Draft release summary: `docs/releases/v0.1.0.md`

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
