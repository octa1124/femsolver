# v1 To v2 Readiness Review

## Current Running Capability

The repository now has a running self-owned path for:

- tetrahedral scalar `H1` Poisson benchmark checks
- tetrahedral first-order `Nedelec` curl-curl benchmark checks
- case-driven preprocessing for tracked robot-joint motor cases
- Gmsh `MSH2` tetra mesh import into the solver
- linear magnetostatic smoke solving with `J` and `B_r` source terms
- cell-wise `B = curl(A)` summaries, magnetic energy, and first air-gap torque-surface metadata
- `motor_check --machine-regression` as the first machine scalar regression gate

This means `v1.0.0` is no longer just an architecture target. It has an executable spine.

## Code Review Findings

- The current motor geometry is still a concentric-envelope reconstruction. It is useful for regression and interface stability, but it is not yet a faithful slot, tooth, winding, and magnet model.
- The torque calculation now exists, but its surface is only as good as the generated `airgap_torque_surface`. Until tooth/slot geometry exists, torque should be treated as a smoke and regression scalar rather than a validated machine value.
- The `motor_pre` C++ entrypoint still shells out to Python. That is acceptable before `v2.0.0`, but the boundary should be kept stable: generated manifest plus mesh should remain the solver input contract.
- `motor_check` is becoming the right place for quality gates. Before `v2.0.0`, it should grow validation orchestration rather than putting checks into ad hoc scripts.

## Optimized `v1.0.0` Exit Criteria

To close `v1.0.0`, focus on:

- one geometry-fidelity upgrade for `joint_type_i_12s10p`, with explicit slot/tooth/magnet region semantics
- stable scalar regression thresholds for energy, average `B`, max `B`, residual norm, and torque-surface metadata
- documented output compatibility for `SolutionBundle`
- clear warnings whenever a run uses fallback synthetic mesh or simplified excitation reductions

Do not expand `v1.0.0` into nonlinear material work. That belongs in `v1.1.0`.

## Optimized Pre-`v2.0.0` Design

Before multiphysics coupling, the solver should introduce three stable contracts:

- `FieldState`: owned vectors plus named field metadata for magnetic, thermal, and future mechanical unknowns
- `PhysicsOperator`: assemble residual/Jacobian contributions for one physics domain without owning the global coupled solve
- `CoupledProblem`: registers physics operators, shared parameters, coupling variables, and solve policy

The current magnetostatic path should become the first `PhysicsOperator`, not a one-off application branch.

## `v2.0.0` Boundary

`v2.0.0` should deliver the coupling foundation, not a full industrial multiphysics product.

Recommended hard scope:

- block-system assembly interfaces
- shared nonlinear/time policy objects
- first magneto-thermal data exchange on a small canonical case
- validation hooks for conservation, energy consistency, and coupling data shape

Recommended deferrals:

- moving rotor interfaces
- eddy-current transient production runs
- thermal CAD fidelity
- neural operators, RL, and sensor feedback loops

Those AI-facing workflows should remain `v3.0.0+`, after generated data has trustworthy provenance.
