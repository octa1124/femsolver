# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/)
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

- Placeholder for upcoming `v0.3.0` vector-field kernel work.

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
