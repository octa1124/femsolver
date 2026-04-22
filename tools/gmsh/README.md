# Gmsh Tooling

This directory owns the current preprocessing helpers for tracked machine cases.

## Current Files

- `case_catalog.py`: validates `cases/machines/*/case.yaml`
- `generate_joint_motor_case.py`: generates simplified concentric-envelope `.geo`, manifest, and summary outputs

## Current Scope

The preprocessing path is intentionally conservative in the current `v0.3.0` baseline.

It can do:

- read the tracked machine-case catalog
- validate required schema fields
- generate `section_2d`, `extruded_3d`, and provisional `full_3d` geometry scripts
- emit `mesh_manifest.yaml` and `preprocess_summary.txt`
- call `gmsh` when available on `PATH`

It cannot do yet:

- faithful tooth, slot, wedge, or discrete-magnet reconstruction
- OCC-based detailed motor CAD
- periodic topology generation
- general-purpose mesh import beyond the first `MSH2` tetrahedral bridge into the self-owned solver kernel

## Usage

Direct Python usage:

```bash
python3 tools/gmsh/generate_joint_motor_case.py \
  --case cases/machines/joint_type_i_12s10p/case.yaml \
  --variant section_2d \
  --output-dir build/generated/joint_type_i_12s10p_section_2d
```

Application entrypoint:

```bash
./build/gcc-debug/src/motor_pre \
  --case cases/machines/joint_type_i_12s10p/case.yaml \
  --variant extruded_3d \
  --output-dir build/generated/joint_type_i_12s10p_extruded_3d
```

## Output Contract

Each run produces:

- one `.geo` geometry script
- one `mesh_manifest.yaml`
- one `preprocess_summary.txt`
- one ASCII `MSH2` `.msh` file when `gmsh` is available and succeeds

The manifest always records:

- case provenance
- reconstruction level
- variant
- declared region and boundary names
- geometry parameters
- mesh-generation status

## Important Limitation

The current generated geometry is a simplified concentric envelope. It is suitable for schema tests, preprocessing integration tests, and early workflow bring-up, but it is not yet a faithful production motor geometry.
