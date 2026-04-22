# `exo_outer_rotor_36s40p`

## Source

- URL: `https://www.mdpi.com/2079-9292/12/2/397`
- Kind: paper
- Intended role: first main validation-oriented robot joint motor case.

## Reconstruction Level

- `simplified_reconstruction`
- The case currently uses a concentric outer-rotor envelope suitable for preprocessing and staged solver bring-up.

## Assumptions

- The source motivates a realistic outer-rotor joint motor topology.
- The current repository version does not yet reconstruct detailed slot profile, discrete magnet blocks, vent holes, or assembly-specific features.
- `full_3d` is declared for this case because it is the first target for future detailed OCC geometry, but today it still maps to the concentric extrusion baseline.

## Planned Test Layers

- `contract`
- `integration`
- `regression`
- future `validation`
