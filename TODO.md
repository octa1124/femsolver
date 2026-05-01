# TODO

This file tracks two different things:

- foundation slices that already exist in the repository
- the remaining path to an industrial-grade nonlinear multiphysics motor solver

Status rule:

- `[x]` implemented and tested in the repository
- `[-]` partially implemented or usable only as a foundation slice
- `[ ]` not implemented yet

Controlling audit:

- `docs/architecture/industrial-nonlinear-multiphysics-gap-audit.md`

## Current Truth Snapshot

- `[x]` `v0.1.0` repository governance, build/test, changelog, and release baseline
- `[x]` `v0.2.0` tetrahedral scalar H1 kernel foundation
- `[-]` `v0.3.0` vector-field and preprocessing foundation; not industrial geometry/preprocessing
- `[-]` `v1.0.0` linear robot-joint motor smoke/regression path; not validated motor design output
- `[-]` `v1.1.0` local nonlinear B-H material foundation; not global nonlinear solve
- `[-]` `v1.2.0` reference RT/H(div) foundation; not physical flux-conforming post-processing
- `[-]` `v1.3.0` reference hex/Q1/Q2 foundation; not physical hex assembly
- `[-]` `v2.0.0` coupled-operator foundation; not production multiphysics

The project is therefore not complete as an industrial nonlinear multiphysics motor solver. It has a useful self-owned foundation chain that must now be converted into full physics, validation, and industrial workflows.

## Foundation Inventory

### `v0.1.0` Governance

- `[x]` Repository layout
- `[x]` CMake/CTest bootstrap
- `[x]` Changelog fragments
- `[x]` Release summary convention
- `[x]` Engineering rules in `Agent.md`
- `[ ]` Fully enforced GitHub branch rules and release evidence automation

### `v0.2.0` Scalar Kernel

- `[x]` Tetra mesh container
- `[x]` Orientation normalization
- `[x]` Reference tetrahedron
- `[x]` Tetra quadrature
- `[x]` H1 P1 basis
- `[x]` Sparse matrix builder
- `[x]` Scalar Poisson assembly and benchmark
- `[ ]` Industrial mesh quality metrics
- `[ ]` Scalable linear solver/preconditioner layer

### `v0.3.0` Vector Kernel And Preprocessing

- `[x]` Tetra edge extraction and boundary-edge discovery
- `[x]` First-order Nedelec basis
- `[x]` Edge DoF map and orientations
- `[x]` H(curl) mapping
- `[x]` Curl-curl assembly and benchmark
- `[x]` Current-density and remanence source assembly
- `[x]` Simplified machine case catalog and preprocessing
- `[x]` Gmsh MSH2 tetra import bridge
- `[-]` `motor_pre` still bridges to Python
- `[ ]` Tooth/slot/magnet-resolved geometry
- `[ ]` Periodic-sector topology
- `[ ]` Production mesh import/validation coverage

### `v1.0.0` Linear Motor Smoke Path

- `[x]` `motor_solve --case --manifest`
- `[x]` Linear magnetostatic `A in H(curl)` smoke solve
- `[x]` Current-density and permanent-magnet source binding
- `[x]` B-field summary and magnetic energy
- `[x]` Preliminary air-gap torque-surface scalar
- `[x]` `motor_check --machine-regression`
- `[-]` Region/boundary validation is incomplete
- `[-]` Torque is a regression smoke scalar, not validated industrial torque
- `[ ]` Flux linkage output
- `[ ]` Magnet working point output
- `[ ]` Multi-angle locked-rotor sweep
- `[ ]` Validated `exo_outer_rotor_36s40p` case

### `v1.1.0` Nonlinear B-H Foundation

- `[x]` Monotone B-H curve
- `[x]` Energy density, secant reluctivity, and differential reluctivity
- `[x]` Orthotropic local material response
- `[x]` Consistent tangent finite-difference test
- `[ ]` Nonlinear Nedelec cell residual
- `[ ]` Nonlinear Nedelec cell Jacobian
- `[ ]` Local material axis rotation from case/mesh metadata
- `[ ]` Picard warm start on assembled nonlinear problem
- `[ ]` Damped Newton with line search
- `[ ]` Nonlinear machine regression and overload validation

### `v1.2.0` RT/H(div) Foundation

- `[x]` Reference tetra RT0 basis
- `[x]` Face-flux interpolation benchmark
- `[x]` Constant-divergence benchmark
- `[x]` `motor_check --hdiv-benchmark`
- `[ ]` Physical H(div) Piola mapping
- `[ ]` Signed global face DoF map
- `[ ]` RT-based flux recovery
- `[ ]` Conservation checks on motor results

### `v1.3.0` Hex And High-Order Foundation

- `[x]` Reference hexahedron
- `[x]` Hex Gauss quadrature
- `[x]` H1 Q1 basis and gradients
- `[x]` H1 Q2 basis and gradients
- `[ ]` Tetra H1 P2
- `[ ]` Physical hex mesh and geometry mapping
- `[ ]` Hex Q1/Q2 assembly
- `[ ]` High-order Nedelec
- `[ ]` High-order RT

### `v2.0.0` Coupling Foundation

- `[x]` `FieldState`
- `[x]` `PhysicsOperator`
- `[x]` `CoupledProblem`
- `[x]` Magnetostatic operator adapter
- `[x]` Canonical magneto-thermal residual/Jacobian test
- `[x]` Version-readiness analysis
- `[ ]` Production thermal conduction operator
- `[ ]` Loss-to-thermal source mapping
- `[ ]` Coupled nonlinear/time policy
- `[ ]` Route production motor solve through `CoupledProblem`
- `[ ]` Coupled conservation and energy checks

## Industrial Product Backlog

### P0: Nonlinear Electromagnetic Core

- `[ ]` Define nonlinear magnetostatic residual API for tetra Nedelec elements
- `[ ]` Implement local nonlinear cell residual from B-H energy
- `[ ]` Implement consistent local Jacobian from the same model
- `[ ]` Add finite-difference Jacobian tests on one-cell and small-mesh cases
- `[ ]` Implement Picard warm start around assembled nonlinear systems
- `[ ]` Implement damped Newton with line search and failure diagnostics
- `[ ]` Bind nonlinear material profiles from `case.yaml`
- `[ ]` Add nonlinear smoke case for `joint_type_i_12s10p`
- `[ ]` Add overload/saturation validation using `quadruped_joint_36s32p`

### P0: Geometry And Mesh Fidelity

- `[ ]` Replace concentric-envelope geometry with tooth/slot/magnet-resolved reconstruction
- `[ ]` Add winding phase/slot region generation
- `[ ]` Add magnet region orientation and segmentation metadata
- `[ ]` Validate region and boundary names against solver requirements
- `[ ]` Add mesh quality checks and hard failure thresholds
- `[ ]` Preserve simplified cases as smoke tests, not validation cases

### P1: Post-Processing And Validation

- `[ ]` Add stable field-export schema
- `[ ]` Add flux linkage and phase quantity summaries
- `[ ]` Add magnet working point summaries
- `[ ]` Add validated Maxwell stress torque integration
- `[ ]` Add torque ripple and air-gap harmonic analysis after multi-angle sweep exists
- `[ ]` Add RT-based flux recovery and conservation analysis
- `[ ]` Add non-visual comparison reports for validation cases

### P1: Thermal And Coupled Multiphysics

- `[ ]` Implement H1 thermal conduction operator
- `[ ]` Add thermal material catalog and boundary conditions
- `[ ]` Map copper/core/magnet losses into thermal sources
- `[ ]` Add magnetic-to-thermal coupled canonical benchmark
- `[ ]` Add steady-state coupled solve path
- `[ ]` Add transient/time-policy contracts only after steady state is stable

### P2: Hex, High Order, And Performance

- `[ ]` Add physical hex mesh container and mappings
- `[ ]` Add H1 tetra P2
- `[ ]` Add hex Q1/Q2 assembly
- `[ ]` Add high-order Nedelec path
- `[ ]` Add high-order RT path
- `[ ]` Add convergence/performance benchmark matrix
- `[ ]` Add preconditioner strategy and solver scaling analysis

### P2: Industrial Workflow

- `[ ]` Add nightly validation suite
- `[ ]` Add release evidence bundle generation
- `[ ]` Add run-to-run result comparison
- `[ ]` Add CI artifact publication for reports
- `[ ]` Add benchmark dashboards after text/JSON analysis is stable

## Next Implementation Cut

The next correct code cut is not another roadmap document. It is:

1. Nonlinear tetra Nedelec cell residual and Jacobian using the existing B-H material model.
2. A one-cell nonlinear benchmark with finite-difference Jacobian verification.
3. A small assembled nonlinear benchmark with Picard/Newton solve control.
4. Only then, a machine-level nonlinear smoke path for `joint_type_i_12s10p`.

That sequence is the shortest credible route from the current foundation code toward an industrial nonlinear motor solver.
