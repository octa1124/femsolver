# Machine Cases

This directory stores the tracked real-world machine-case catalog used to grow the solver against auditable examples.

## Current Catalog

- `joint_type_i_12s10p`: first simple robot-joint motor case reconstructed from a public paper source
- `exo_outer_rotor_36s40p`: first outer-rotor validation-oriented robot-joint case
- `quadruped_joint_36s32p`: staged nonlinear and overload-oriented robot-joint case
- `tbm76_envelope`: industrial envelope-only case used for preprocessing and packaging regression

## Case Policy

- every machine case must declare a source URL and a reconstruction level
- every real machine case must provide at least `section_2d` and `extruded_3d`
- `full_3d` is optional and only enabled where public geometry detail is sufficient
- simplified reconstructions must be labeled honestly in both `case.yaml` and the case `README.md`

## Current Fidelity

The current repository stores simplified parameterized reconstructions, not faithful manufacturing geometry.

This means the cases are already useful for:

- case-schema validation
- preprocessing integration tests
- naming and region-contract regression
- staged solver wiring

It does not yet mean they are ready for final electromagnetics validation against published torque or flux values.
