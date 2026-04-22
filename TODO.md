# TODO

This file tracks the executable delivery checklist from the current repository state through `v1.1.0`.

Status rule:

- `[x]` completed in the repository
- `[-]` in progress or partially implemented
- `[ ]` not started yet

## Current Snapshot

- `[x]` `v0.1.0` repository governance, CI, changelog, and release baseline
- `[x]` `v0.2.0` self-owned tetrahedral scalar kernel with Poisson benchmark
- `[-]` `v0.3.0` self-owned vector-field kernel baseline and machine-case preprocessing
- `[ ]` `v1.0.0` first robot-joint motor magnetostatic MVP
- `[ ]` `v1.1.0` nonlinear material upgrade plus `RT` and first higher-order expansion

## `v0.3.0` Closeout Checklist

### Case Catalog And Preprocessing

- `[x]` Add tracked machine cases for `joint_type_i_12s10p`, `exo_outer_rotor_36s40p`, `quadruped_joint_36s32p`, and `tbm76_envelope`
- `[x]` Require source provenance, reconstruction level, and declared geometry variants
- `[x]` Support `section_2d` and `extruded_3d` across all real cases
- `[x]` Support provisional `full_3d` declaration for `exo_outer_rotor_36s40p`
- `[x]` Generate simplified concentric-envelope `.geo` files
- `[x]` Emit `mesh_manifest.yaml` and `preprocess_summary.txt`
- `[x]` Wire `motor_pre --case --variant --output-dir`
- `[x]` Add Python schema tests and `CTest` preprocessing integration tests
- `[ ]` Replace Python bridge in `motor_pre` with native C++ case loading and preprocessing orchestration
- `[ ]` Upgrade from concentric-envelope geometry to first tooth/slot/magnet-resolved reconstruction where public data is sufficient

### Kernel Contracts

- `[x]` Keep explicit discretization descriptors for cell family, space family, and polynomial order
- `[x]` Extend tetra mesh topology to extract edges and boundary edges
- `[x]` Add dedicated `kernel/space` ownership for DoF maps and orientation
- `[x]` Document `kernel/common -> mesh/reference/quadrature/basis/space/algebra -> assembly -> benchmark` layering
- `[ ]` Add a first element-agnostic assembly selector above tetra-specific implementations

### `H(curl)` Baseline

- `[x]` Implement tetrahedral first-order `Nedelec`
- `[x]` Implement edge-based DoF numbering and sign handling
- `[x]` Implement `H(curl)` geometric mapping in `ElementGeometry`
- `[x]` Implement tetrahedral curl-curl local/global assembly
- `[x]` Add canonical curl-curl benchmark and expose it through `motor_check`
- `[ ]` Add current-source assembly tied to machine regions
- `[ ]` Add simple permanent-magnet source assembly tied to machine regions
- `[ ]` Add a first machine-coupled electromagnetics smoke path on `joint_type_i_12s10p`

### Validation And Docs

- `[x]` Add implementation docs for case catalog and preprocessing
- `[x]` Add implementation docs for the `v0.3.0` vector-kernel baseline
- `[x]` Update architecture and physics docs to reflect current repository reality
- `[ ]` Add canonical vector validation cases beyond the current manufactured benchmark

## `v1.0.0` Robot-Joint Motor MVP Checklist

### Input, Case, And Region Semantics

- `[ ]` Load case metadata without placeholder bootstrap shortcuts
- `[ ]` Load mesh manifest and validate region/boundary names against solver expectations
- `[ ]` Bind named regions to material descriptors
- `[ ]` Bind named regions to excitation descriptors
- `[ ]` Fail clearly on unsupported requests instead of silently degrading

### Electromagnetic Assembly

- `[ ]` Assemble `A in H(curl)` magnetostatic operator on tetrahedral `Nedelec`
- `[ ]` Add prescribed winding current-density contribution
- `[ ]` Add permanent-magnet remanence or magnetization contribution
- `[ ]` Add outer-air truncation boundary handling for the MVP
- `[ ]` Define the conservative gauge/boundary strategy used by the first machine solves

### Solve Path

- `[ ]` Build a first machine solve driver in `motor_solve`
- `[ ]` Use linear material models only in the first `v1.0.0` machine path
- `[ ]` Keep the first nonlinear solve hooks present but disabled or conservative until `v1.1.0`
- `[ ]` Record convergence summary and failure modes in text output

### Post-Processing

- `[ ]` Compute `B = curl(A)` on machine results
- `[ ]` Compute magnetic energy
- `[ ]` Compute Maxwell-stress torque on a declared air-gap surface
- `[ ]` Emit solver summary plus stable scalar outputs for regression

### Case Bring-Up

- `[ ]` Use `joint_type_i_12s10p extruded_3d` as the first machine electromagnetics smoke case
- `[ ]` Promote `joint_type_i_12s10p` to regression coverage
- `[ ]` Bring `exo_outer_rotor_36s40p extruded_3d` into validation once the `v1.0.0` path is stable
- `[ ]` Keep `tbm76_envelope` as preprocessing and packaging regression only unless fidelity improves

### Tests

- `[ ]` Add unit tests for source-term assembly
- `[ ]` Add integration tests for machine-case solve orchestration
- `[ ]` Add regression checks for selected scalar machine outputs
- `[ ]` Add validation hooks for `exo_outer_rotor_36s40p`

## `v1.1.0` Nonlinear, `RT`, And Higher-Order First Slice

### Nonlinear Materials

- `[ ]` Add nonlinear isotropic `B-H` curve representation
- `[ ]` Add monotone interpolation and energy-consistent evaluation
- `[ ]` Add local-frame anisotropic or orthotropic material support
- `[ ]` Derive residual and tangent from one consistent constitutive model
- `[ ]` Add tangent-vs-finite-difference validation

### Nonlinear Solve Strategy

- `[ ]` Add Picard warm start for nonlinear magnetic problems
- `[ ]` Add damped Newton update path
- `[ ]` Add line-search or backtracking fallback
- `[ ]` Add clear failure reporting for non-convergence and constitutive breakdown

### `H(div)` And Flux-Conforming Post-Processing

- `[ ]` Add lowest-order tetrahedral `Raviart-Thomas`
- `[ ]` Add `H(div)` canonical benchmark
- `[ ]` Use `RT` or compatible flux post-processing where it improves output trustworthiness

### Higher-Order First Batch

- `[ ]` Add tetrahedral `H1 P2`
- `[ ]` Add hexahedral reference element and quadrature contracts
- `[ ]` Add hexahedral `H1 Q1/Q2`
- `[ ]` Keep higher-order `ND/RT` explicitly out of the hard `v1.1.0` gate unless lower-order lines are stable

### Validation Cases

- `[ ]` Use `quadruped_joint_36s32p extruded_3d` for nonlinear and overload-oriented validation
- `[ ]` Extend `exo_outer_rotor_36s40p` into nightly validation coverage
- `[ ]` Add stable validation thresholds for saturation-sensitive outputs

## Module-Oriented Work Queue

### `src/app`

- `[-]` `motor_pre`: usable for case-driven preprocessing, still Python-bridged
- `[ ]` `motor_solve`: promote from placeholder to first real machine solve path
- `[-]` `motor_check`: scalar and vector benchmark runner present, needs machine regression orchestration

### `src/case`, `src/mesh`, `src/io`

- `[-]` `CaseSpec`: richer schema contract exists, but real file parsing is not native yet
- `[ ]` `MeshManifest`: bind generated manifest semantics into the solver path
- `[ ]` Reporting: add machine solve, post-processing, and regression summaries

### `src/kernel`

- `[x]` scalar tetrahedral path
- `[-]` vector tetrahedral path
- `[ ]` `RT`
- `[ ]` hexahedral path
- `[ ]` higher-order path

### `src/material`, `src/nonlinear`, `src/post`

- `[ ]` material-law objects beyond placeholders
- `[ ]` real nonlinear update policies
- `[ ]` real field and torque post-processing

## Recommended Next Sequence

1. Finish `v0.3.0` source-term assembly for current density and permanent magnets.
2. Convert `joint_type_i_12s10p extruded_3d` into the first real motor smoke case.
3. Promote `motor_solve` from placeholder to the first linear magnetostatic machine driver.
4. Add `B`, energy, and torque post-processing for the `v1.0.0` gate.
5. Only then open the `v1.1.0` nonlinear material and `RT` workstream.
