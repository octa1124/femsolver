# `joint_type_i_12s10p`

## Source

- URL: `https://www.mdpi.com/1996-1073/14/20/6690`
- Kind: paper
- Intended interpretation: the source discusses a robot-joint PMSM design. This repository uses the slots-then-poles naming rule, so `12s10p` means `12` slots and `10` poles.

## Reconstruction Level

- `simplified_reconstruction`
- The current geometry is a concentric envelope reconstruction intended for preprocessing and early solver integration.

## Assumptions

- The case records public topology intent and a simplified size envelope.
- Tooth shape, detailed slot wedge geometry, end turns, adhesive layers, and exact magnet segmentation are not reconstructed yet.
- The first tracked variants are `section_2d` and `extruded_3d`.

## Planned Test Layers

- `contract`
- `integration`
- `regression`
- later `validation` and electromagnetics smoke tests
