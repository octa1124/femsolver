# Physical Architecture

## Purpose

This document defines the physical problem family that `femsolver` is designed to solve, the mathematical formulation path, and the explicit capability limits for each stage of the roadmap.

## Target Problem Family

The project targets a staged multiphysics trajectory that begins with 3D electrical-machine electromagnetics and then expands toward coupled thermal and mechanical behavior.

The first applied MVP is a robot-joint permanent-magnet motor under locked-rotor or fixed-angle magnetostatic conditions.

The initial production target is not “all multiphysics.” It is the narrower and more defensible problem:

- 3D magnetostatics
- fixed rotor angle
- prescribed excitations
- field, energy, and torque post-processing

## Physical Layers

### Electromagnetic Core

Primary field unknown:

- magnetic vector potential `A`

Primary formulation:

- `A in H(curl)`
- magnetic flux density `B = curl(A)`

### Excitations

Supported or planned excitation types:

- prescribed stranded-equivalent current density `J_free`
- permanent-magnet remanent flux density or magnetization

### Material Models

Staged material model set:

- air
- copper current-density region
- permanent magnet
- linear isotropic steel
- linear laminated steel
- orthotropic nonlinear steel based on directional `B-H`

### Coupled Physics Layers

Planned later physical layers:

- heat conduction
- thermo-magnetic coupling
- structural or reduced mechanical response
- coupled transient evolution

### Post-Processing

Planned physical outputs:

- `B = curl(A)`
- magnetic energy
- Maxwell-stress torque on a dedicated air-gap integration surface
- probe values and slice data

## Current Physics Status

### Can Do Today

At `v0.1.0`, the repository can describe the future physical architecture and test repository tooling, but it does not solve any electromagnetic problem yet.

### Cannot Do Today

- assemble finite-element operators
- read or interpret `B-H` curves
- solve a nonlinear magnetic system
- compute flux density, energy, or torque from a machine model

## `v1.0.0` Physical Contract

### Will Do

- solve 3D robot-joint permanent-magnet motor magnetostatics
- handle prescribed current-density windings
- handle permanent magnets through remanent-flux or magnetization data
- compute `B`, magnetic energy, and torque

### Will Not Do Yet

- periodic sectors
- rotor motion
- transient induction effects
- eddy current
- hysteresis
- thermal coupling
- external circuit coupling
- NVH coupling

## `v1.1.0` Material Upgrade Contract

### Will Add

- nonlinear isotropic and anisotropic `B-H`
- local material frames
- consistent nonlinear tangents

### Will Still Defer

- coupled thermal solve
- coupled structural solve
- moving-interface transients

## Governing Equations

The planned magnetic core is:

- `B = curl(A)`
- `curl(H(B)) = J_free + curl(M)`

In a linear region:

- `H = nu * B`

In anisotropic linear regions:

- `H = nu_tensor * B`

In orthotropic nonlinear regions:

- `H` is derived from an energy density in the local material frame

## Nonlinear Orthotropic `B-H` Model

The baseline nonlinear direction, once enabled, is:

- define a local orthotropic material frame
- express `B_local`
- use a separable or near-separable energy model
- derive both residual and tangent consistently from the same energy law

Planned default concept:

- in-plane directional law for laminated tangential directions
- through-thickness directional law for the lamination-normal direction

This is deliberate because the project wants nonlinear and anisotropic material support to remain interpretable and mathematically consistent rather than rely on opaque black-box material handling.

## Boundary Conditions And Domain Truncation

For the first physical version:

- use a finite outer air domain
- apply a simple truncation boundary on the outer boundary
- keep boundary handling conservative and auditable

More advanced open-boundary treatment is deferred until the basic solver path is validated.

## Validation Strategy

The physical validation ladder is:

- canonical benchmark problems
- robot-joint motor MVP full model
- later, full model versus reduced or periodic model comparison
- MFEM versus FEniCS canonical sidecar checks

The project should not claim solver readiness until this ladder exists for the implemented feature set.

## Main Deferred Physics

The following are intentionally deferred because they change the problem class substantially:

- periodic sectors
- angle stepping
- sliding interfaces
- low-frequency transient solves
- eddy current
- thermal, circuit, and structural coupling

## Physics-AI Horizon

Physics-AI is a future extension, not an immediate solver responsibility. The solver should eventually expose clean interfaces for:

- surrogate-model training data generation
- neural operator experiments
- sensor-conditioned state estimation
- RL environment coupling

Those workflows should consume a controllable solver kernel rather than replace it.

## Design Constraint

The code structure must keep physical semantics explicit:

- no hidden mesh attribute assumptions
- no geometry-generation logic inside the solver core
- no constitutive update without test and tangent validation

This preserves traceability when the solver becomes materially nonlinear.
