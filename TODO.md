# TODO

This file tracks the executable delivery checklist from the current repository state through `v2.0.0`.

Status rule:

- `[x]` completed in the repository
- `[-]` in progress or partially implemented
- `[ ]` not started yet

## Current Snapshot

- `[x]` `v0.1.0` repository governance, CI, changelog, and release baseline
- `[x]` `v0.2.0` self-owned tetrahedral scalar kernel with Poisson benchmark
- `[x]` `v0.3.0` self-owned vector-field kernel baseline and machine-case preprocessing
- `[x]` `v1.0.0` first robot-joint motor linear magnetostatic MVP gate
- `[x]` `v1.1.0` nonlinear B-H material MVP with tangent validation
- `[x]` `v1.2.0` tetra `RT0` / H(div) benchmark slice
- `[x]` `v1.3.0` hex/high-order H1 foundation slice
- `[x]` `v2.0.0` multiphysics coupling foundation slice

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
- `[-]` Replace Python bridge in `motor_pre` with native C++ case loading and preprocessing orchestration
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
- `[x]` Add current-source assembly tied to machine regions
- `[x]` Add simple permanent-magnet source assembly tied to machine regions
- `[x]` Add a first machine-coupled electromagnetics smoke path on `joint_type_i_12s10p`

### Validation And Docs

- `[x]` Add implementation docs for case catalog and preprocessing
- `[x]` Add implementation docs for the `v0.3.0` vector-kernel baseline
- `[x]` Update architecture and physics docs to reflect current repository reality
- `[x]` Add canonical vector validation cases beyond the current manufactured benchmark through the H(div) RT benchmark

## `v1.0.0` Robot-Joint Motor MVP Checklist

### Input, Case, And Region Semantics

- `[x]` Load case metadata without placeholder bootstrap shortcuts
- `[-]` Load mesh manifest and validate region/boundary names against solver expectations
- `[x]` Bind named regions to material descriptors
- `[x]` Bind named regions to excitation descriptors
- `[-]` Fail clearly on unsupported requests instead of silently degrading

### Electromagnetic Assembly

- `[x]` Assemble `A in H(curl)` magnetostatic operator on tetrahedral `Nedelec` for the linear MVP path
- `[x]` Add prescribed winding current-density contribution
- `[x]` Add permanent-magnet remanence or magnetization contribution
- `[-]` Add outer-air truncation boundary handling for the MVP
- `[x]` Define the conservative gauge/boundary strategy used by the first machine solves

### Solve Path

- `[x]` Build a first machine solve driver in `motor_solve`
- `[x]` Use linear material models only in the first `v1.0.0` machine path
- `[x]` Keep the first nonlinear solve hooks present and conservative until the material law is wired into global assembly
- `[x]` Record convergence summary and failure modes in text output

### Post-Processing

- `[x]` Compute `B = curl(A)` on machine results
- `[x]` Compute magnetic energy
- `[x]` Compute Maxwell-stress torque on a declared air-gap surface
- `[x]` Emit solver summary plus stable scalar outputs for regression

### Case Bring-Up

- `[x]` Use `joint_type_i_12s10p extruded_3d` as the first machine electromagnetics smoke case
- `[x]` Promote `joint_type_i_12s10p` to regression coverage
- `[ ]` Bring `exo_outer_rotor_36s40p extruded_3d` into validation once the `v1.0.0` path is stable
- `[ ]` Keep `tbm76_envelope` as preprocessing and packaging regression only unless fidelity improves

### Tests

- `[x]` Add unit tests for source-term assembly
- `[x]` Add integration tests for machine-case solve orchestration
- `[x]` Add regression checks for selected scalar machine outputs
- `[ ]` Add validation hooks for `exo_outer_rotor_36s40p`

## `v1.1.0` Nonlinear B-H Material Slice

### Nonlinear Materials

- `[x]` Add nonlinear isotropic `B-H` curve representation
- `[x]` Add monotone interpolation and energy-consistent evaluation
- `[x]` Add local-frame orthotropic material support
- `[x]` Derive local field response and tangent from one consistent energy model
- `[x]` Add tangent-vs-finite-difference validation

### Nonlinear Solve Strategy

- `[-]` Add Picard warm start for nonlinear magnetic problems; policy hooks exist, global nonlinear assembly remains next
- `[-]` Add damped Newton update path; policy hooks exist, machine nonlinear residual remains next
- `[ ]` Add line-search or backtracking fallback
- `[ ]` Add clear failure reporting for non-convergence and constitutive breakdown

## `v1.2.0` `H(div)` And Flux-Conforming Foundation

- `[x]` Add lowest-order tetrahedral `Raviart-Thomas`
- `[x]` Add `H(div)` canonical benchmark
- `[x]` Expose `motor_check --hdiv-benchmark`
- `[ ]` Add signed global face DoF maps for physical tetra meshes
- `[ ]` Use `RT` or compatible flux post-processing where it improves output trustworthiness

## `v1.3.0` Higher-Order And Hex Foundation

- `[ ]` Add tetrahedral `H1 P2`
- `[x]` Add hexahedral reference element and quadrature contracts
- `[x]` Add hexahedral `H1 Q1/Q2`
- `[ ]` Keep higher-order `ND/RT` explicitly out of the hard `v1.3.0` gate unless lower-order lines are stable

### Validation Cases

- `[ ]` Use `quadruped_joint_36s32p extruded_3d` for nonlinear and overload-oriented validation
- `[ ]` Extend `exo_outer_rotor_36s40p` into nightly validation coverage
- `[ ]` Add stable validation thresholds for saturation-sensitive outputs

## `v2.0.0` Coupling Foundation Checklist

### Core Contracts

- `[x]` Add `FieldState` for owned named field blocks and field metadata
- `[x]` Add callback-based `PhysicsOperator` without introducing inheritance chains
- `[x]` Add `CoupledProblem` aggregation for residual and matrix contributions
- `[x]` Lift the current magnetostatic block shape into the first `PhysicsOperator` adapter
- `[ ]` Add shared nonlinear/time policy contracts for coupled solves

### Canonical Coupling

- `[x]` Add a synthetic multi-operator unit test for coupled assembly behavior
- `[x]` Add a small magneto-thermal canonical case
- `[-]` Add conservation and energy-shape checks for the first coupled case; residual and cross-Jacobian checks exist, full conservation checks remain next

### Analysis And Quality

- `[x]` Add non-visual machine report analysis with completion scoring
- `[x]` Add inheritance-depth analysis and keep the codebase within depth `<= 3`
- `[x]` Add version-readiness analysis for the `v1.0 -> v2.0` chain
- `[ ]` Wire analysis reports into nightly artifacts once CI storage is finalized

## Module-Oriented Work Queue

### `src/app`

- `[-]` `motor_pre`: usable for case-driven preprocessing, still Python-bridged
- `[-]` `motor_solve`: promoted from placeholder to a first linear machine smoke path
- `[-]` `motor_check`: scalar and vector benchmark runner plus first machine regression orchestration present, needs validation orchestration
- `[-]` analysis tooling: text/JSON report analysis exists, needs CI artifact wiring

### `src/case`, `src/mesh`, `src/io`

- `[x]` `CaseSpec`: richer schema contract exists and now supports native file parsing for the current repository schema
- `[-]` `MeshManifest`: generated manifest semantics now load in C++, and the first Gmsh `MSH2` tetra import path exists, but broader mesh semantics and formats are still missing
- `[-]` Reporting: machine smoke summaries now include field, energy, torque-surface, and regression signals, but field export is still missing

### `src/kernel`

- `[x]` scalar tetrahedral path
- `[x]` vector tetrahedral path
- `[x]` reference `RT`
- `[x]` reference hexahedral path
- `[x]` first high-order H1 path through hex `Q2`

### `src/material`, `src/nonlinear`, `src/post`

- `[x]` material-law objects beyond placeholders
- `[ ]` real nonlinear update policies
- `[-]` real field and torque post-processing

### `src/coupling`

- `[x]` `FieldState`
- `[x]` `PhysicsOperator`
- `[x]` `CoupledProblem`
- `[x]` magnetostatic operator adapter
- `[x]` canonical magneto-thermal operator adapter

## Recommended Next Sequence

1. Wire `MonotoneBhCurve` and `OrthotropicBhMaterial` into nonlinear tetra `Nedelec` cell residuals.
2. Add signed tetra face DoF maps and H(div) Piola mapping so RT can support flux-conforming post-processing.
3. Add hexahedral mesh geometry mapping and `Q1/Q2` scalar assembly.
4. Replace the concentric-envelope preprocessing geometry with the first tooth/slot/magnet-resolved reconstruction while preserving the imported tetra path.
5. Promote `exo_outer_rotor_36s40p` and `quadruped_joint_36s32p` into validation once the nonlinear machine path is stable.
