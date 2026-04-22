#include <numeric>

#include <gtest/gtest.h>

#include "femsolver/kernel/mesh/tetra_mesh.hpp"

TEST(KernelTetraMeshTest, CentroidRefinedReferenceMeshIsValidAndPositivelyOriented) {
  const auto mesh = femsolver::kernel::mesh::BuildCentroidRefinedReferenceTetraMesh();

  EXPECT_TRUE(mesh.IsValid());
  EXPECT_EQ(mesh.nodes().size(), 5U);
  EXPECT_EQ(mesh.cells().size(), 4U);
  EXPECT_EQ(mesh.edges().size(), 10U);
  EXPECT_EQ(mesh.BoundaryNodeIds().size(), 4U);
  EXPECT_EQ(mesh.BoundaryEdgeIds().size(), 6U);

  double total_volume = 0.0;
  for (int cell_index = 0; cell_index < static_cast<int>(mesh.cells().size()); ++cell_index) {
    const double cell_volume = mesh.SignedCellVolume(cell_index);
    EXPECT_GT(cell_volume, 0.0);
    total_volume += cell_volume;
  }

  EXPECT_NEAR(total_volume, 1.0 / 6.0, 1.0e-12);
}

TEST(KernelTetraMeshTest, ConstructorCanonicalizesNegativeOrientation) {
  femsolver::kernel::mesh::TetraMesh mesh(
      {{0.0, 0.0, 0.0},
       {1.0, 0.0, 0.0},
       {0.0, 1.0, 0.0},
       {0.0, 0.0, 1.0}},
      {{{1, 0, 2, 3}, 1}},
      {{{0, 2, 1}, 1}, {{0, 1, 3}, 1}, {{1, 2, 3}, 1}, {{0, 3, 2}, 1}});

  EXPECT_TRUE(mesh.IsValid());
  EXPECT_GT(mesh.SignedCellVolume(0), 0.0);
}
