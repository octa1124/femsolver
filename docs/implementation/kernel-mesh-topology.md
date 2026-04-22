# Kernel Mesh And Topology

## Scope

This note explains the `kernel/mesh` module implemented in `v0.2.0`.

## What The Module Does

The current mesh module provides:

- point storage through `Point3D`
- tetrahedral cell storage through `TetraCell`
- boundary-face storage through `BoundaryFace`
- a `TetraMesh` container with validation
- positive-orientation normalization at construction time
- discovery of boundary node ids from boundary faces
- a small built-in canonical mesh for the first benchmark

## What The Module Does Not Do Yet

- read external mesh files
- store edges or face adjacency tables
- manage periodic topology
- carry material or excitation semantics
- own DoF numbering

## Public Interface

The public interface is defined in:

- `include/femsolver/kernel/mesh/tetra_mesh.hpp`

The main contracts are:

- `TetraMesh::IsValid()`
- `TetraMesh::BoundaryNodeIds()`
- `TetraMesh::SignedCellVolume()`
- `BuildCentroidRefinedReferenceTetraMesh()`

## Design Choices

### Positive Orientation First

Element orientation is normalized when the mesh container is constructed. This is important because later vector-element work will depend on consistent orientation semantics.

### Boundary Faces Instead Of Only Boundary Nodes

The module stores boundary faces, then derives boundary nodes from them. This is a deliberate choice so later surface integrals, traction-like terms, and Maxwell-stress-style post-processing have a path that grows from an explicit surface representation rather than from a node-only shortcut.

### Canonical Benchmark Mesh Inside The Kernel

The built-in centroid-refined reference tetrahedron is not an industrial mesh strategy. It is a controlled canonical case used to verify:

- orientation normalization
- interior-node handling
- end-to-end scalar assembly

## Verification

Current tests verify:

- the canonical mesh is valid
- the refined tetrahedra have positive signed volumes
- the refined cells recover the total domain volume
- negative cell orientation is corrected by the constructor
