# Testing And Quality Gates

## Scope

This document explains the `v0.1.0` testing stack, local validation flow, and quality gates.

## Implemented Files

- `tests/CMakeLists.txt`
- `tests/unit/*.cpp`
- `tests/python/test_repository_tools.py`
- `.clang-format`
- `.clang-tidy`
- `pyproject.toml`
- `.github/workflows/ci.yml`

## Test Layers

### C++ Unit Tests

`GoogleTest` is used for baseline C++ unit coverage. The current tests verify the bootstrap types and repository roadmap wiring:

- case defaults
- mesh manifest validation
- material catalog expectations
- excitation configuration
- nonlinear-policy defaults
- release-roadmap metadata
- solution-bundle placeholders

### Integration Smoke Tests

`CTest` also runs the three application binaries with `--help` to ensure the executables are built and callable.

### Python Tooling Tests

`pytest` validates repository scripts:

- changelog-fragment validation
- release-note rendering
- GitHub bootstrap command rendering

This is important because the repository-management scripts are part of the product-development workflow, not throwaway utilities.

## Quality Gates

The baseline quality gates include:

- changelog-fragment validation
- `pytest`
- `CTest`
- `clang-format --dry-run`
- `clang-tidy`
- coverage generation for CI

## Validation Performed For This Baseline

The current baseline has been verified locally with:

- `python3 tools/scripts/check_changelog.py --directory changes/unreleased`
- `python3 -m pytest tests/python`
- `cmake --preset gcc-debug`
- `cmake --build --preset gcc-debug`
- `ctest --preset gcc-debug`

## Deferred Work

- schema contract tests
- benchmark regression tests
- MFEM versus FEniCSx canonical validation
- long-running nightly physics regression
