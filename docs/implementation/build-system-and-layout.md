# Build System And Code Layout

## Scope

This document explains the `v0.1.0` build system and source-tree organization.

## Implemented Files

- `CMakeLists.txt`
- `CMakePresets.json`
- `cmake/ProjectOptions.cmake`
- `src/CMakeLists.txt`
- `docker/dev/Dockerfile`
- `docker/ci/Dockerfile`

## Build Architecture

### Top-Level CMake

The top-level `CMakeLists.txt` does four things:

- configures project metadata
- blocks in-source builds
- applies macOS SDK compatibility handling
- delegates to `src/` and `tests/`

The macOS compatibility block exists because this workstation’s Command Line Tools lacked libc++ headers while full Xcode was present. The repository therefore falls back to Xcode’s libc++ headers when necessary so local builds remain reproducible.

### Project Options Module

`cmake/ProjectOptions.cmake` centralizes:

- sanitizer toggles
- coverage toggles
- compiler warning settings
- target-default helper functions

This keeps warning and instrumentation logic target-based and reusable.

### CMake Presets

`CMakePresets.json` defines the canonical build entrypoints:

- `gcc-debug`
- `clang-asan`
- `gcc-coverage`

These presets are used locally and in CI so developer workflows match automation workflows.

## Source Layout

The tree is organized by responsibility:

- `include/femsolver/...` for public headers
- `src/...` for implementation
- `tests/...` for automated tests
- `tools/...` for scripts and future Gmsh/FEniCS helpers

This layout is intentionally stable so future MFEM integration can happen without moving repository-level tooling around.

## Deferred Work

- dependency discovery for MFEM, Hypre, Gmsh, and FEniCSx
- package-level install/export logic
- packaging or binary distribution
