# Repository Layout

This repository separates product code, test code, tooling, and governance:

- `src/` and `include/`: C++ applications and libraries
- `tests/`: unit, contract, integration, regression, and validation suites
- `tools/`: Python repository tooling and future Gmsh/FEniCS helpers
- `cases/`: benchmark and machine case assets
- `docs/`: ADRs, architecture notes, implementation notes, physics notes, release notes, governance notes
- `docker/`: reproducible development and CI images
- `.github/`: workflows, templates, and repository metadata bootstrap files

The design principle is to keep future MFEM solver code isolated from geometry-generation scripts and repository automation.
