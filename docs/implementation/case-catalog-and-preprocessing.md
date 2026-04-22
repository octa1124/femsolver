# Case Catalog And Preprocessing

## Purpose

This document explains the current machine-case catalog and preprocessing path that were introduced as the first concrete application-facing layer above the self-owned FEM kernel.

The goal of this work is not yet full-fidelity motor CAD. The goal is to replace generic placeholders with:

- auditable case provenance
- explicit reconstruction assumptions
- reproducible generated artifacts
- stable contracts that later solver work can consume

## What Is Implemented Now

The repository now contains four tracked cases under `cases/machines/`:

- `joint_type_i_12s10p`
- `exo_outer_rotor_36s40p`
- `quadruped_joint_36s32p`
- `tbm76_envelope`

Each case currently ships with:

- `case.yaml`
- `README.md`

The current `case.yaml` contract records:

- `case_id`
- `source_kind`
- `source_url`
- `reconstruction_level`
- `geometry_variants`
- `cell_type`
- `space_family`
- `polynomial_order`
- `materials_profile`
- `excitation_profile`
- `validation_targets`
- `geometry_parameters`

The current implementation also provides:

- Python-side schema validation in `tools/gmsh/case_catalog.py`
- a simplified geometry generator in `tools/gmsh/generate_joint_motor_case.py`
- `motor_pre` as a C++ application entrypoint that dispatches to that generator
- manifest and summary output for downstream workflow stabilization

## What It Can Do

Today the preprocessing path can:

- validate that every tracked case declares provenance and required geometry metadata
- enforce the policy that real cases include both `section_2d` and `extruded_3d`
- generate simplified concentric-envelope `.geo` files
- optionally invoke `gmsh` if it is installed
- emit an ASCII `MSH2` tetrahedral mesh artifact for the first native solver-side import path
- emit a stable `mesh_manifest.yaml`
- emit a human-readable `preprocess_summary.txt`

## What It Cannot Do Yet

Today the preprocessing path does not yet:

- rebuild faithful slot, tooth, wedge, and magnet details from the cited sources
- generate periodic topology
- represent end-turns, adhesive layers, vent holes, or manufacturing details
- guarantee electromagnetics-ready mesh quality for real motor solves
- provide a native C++ preprocessing orchestrator for `motor_pre`

## Why The Case Schema Looks This Way

The schema is designed to stabilize the application boundary early.

The important idea is that later solver work should not guess:

- where the case came from
- how faithful the reconstruction is
- which geometry variants exist
- what material and excitation profiles are expected
- which validation ladder the case belongs to

The current schema therefore prefers explicit fields over compact but ambiguous shorthand.

## Generator Algorithm

The current generator is intentionally simple and explicit.

For each case and variant it:

1. loads and validates `case.yaml`
2. derives a concentric parameter set from `geometry_parameters`
3. computes derived radii such as the outer air radius and magnet-inner radius
4. writes a `.geo` file with concentric circular regions
5. uses 2D surfaces for `section_2d`
6. extrudes those surfaces by `axial_length_mm` for `extruded_3d`
7. currently reuses the extrusion baseline for provisional `full_3d`
8. writes `mesh_manifest.yaml`
9. writes `preprocess_summary.txt`
10. runs `gmsh` only when available and not explicitly skipped
11. requests `MSH2` output so the current self-owned tetrahedral import path can consume the generated mesh

This path is intentionally auditable. There is no hidden CAD layer yet.

## Output Contract

The generated manifest captures:

- case provenance
- reconstruction level
- requested variant
- declared region names
- declared boundary names
- geometry parameters
- mesh-generation status
- generated file paths

This contract is important because later `motor_solve` work can consume the manifest instead of reverse-engineering generator internals.

## Current Case Roles

The four current cases have different responsibilities.

- `joint_type_i_12s10p`: first simple real robot-joint case intended for early electromagnetics smoke integration
- `exo_outer_rotor_36s40p`: first main validation-oriented outer-rotor case
- `quadruped_joint_36s32p`: future nonlinear and overload-oriented case
- `tbm76_envelope`: industrial packaging and preprocessing regression case only

## Testing And Verification

Current automated coverage includes:

- schema validation for every tracked case
- catalog enumeration tests
- generated-artifact tests for `section_2d`
- generated-artifact tests for `extruded_3d`
- `CTest` integration runs through `motor_pre`

The current tests verify:

- provenance and required fields exist
- `slot_count` and `pole_count` are preserved when declared
- region and boundary naming are stable
- manifests and summaries are generated consistently

## Design Limits And Risks

- `motor_pre` currently bridges into Python rather than parsing YAML natively in C++
- the generated geometry is a simplified concentric envelope
- `full_3d` is still provisional for `exo_outer_rotor_36s40p`
- the first imported-mesh path is intentionally narrow and currently supports only ASCII Gmsh `MSH2` tetrahedral meshes
- case metadata already points to the intended production `Nedelec` path, but preprocessing still stays geometry-first rather than full machine CAD

## Next Steps

The next meaningful extensions are:

- native C++ case parsing or a stable shared parser boundary
- more detailed tooth/slot/magnet geometry reconstruction where public data is sufficient
- coupling generated region/boundary names into real solver-side material and excitation assignment
- regression tolerances on mesh statistics once the geometry path is less provisional
