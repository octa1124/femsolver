#include <gtest/gtest.h>

#include "femsolver/kernel/assembly/element_geometry.hpp"
#include "femsolver/kernel/mesh/tetra_mesh.hpp"

TEST(KernelElementGeometryTest, IdentityReferenceTetraMapsCorrectly) {
  femsolver::kernel::mesh::TetraMesh mesh(
      {{0.0, 0.0, 0.0},
       {1.0, 0.0, 0.0},
       {0.0, 1.0, 0.0},
       {0.0, 0.0, 1.0}},
      {{{0, 1, 2, 3}, 1}},
      {{{0, 2, 1}, 1}, {{0, 1, 3}, 1}, {{1, 2, 3}, 1}, {{0, 3, 2}, 1}});

  const auto geometry = femsolver::kernel::assembly::BuildElementGeometry(mesh, 0);
  const auto mapped = geometry.MapToPhysical({0.25, 0.25, 0.25});

  EXPECT_NEAR(geometry.determinant, 1.0, 1.0e-12);
  EXPECT_NEAR(geometry.volume, 1.0 / 6.0, 1.0e-12);
  EXPECT_NEAR(mapped.x, 0.25, 1.0e-12);
  EXPECT_NEAR(mapped.y, 0.25, 1.0e-12);
  EXPECT_NEAR(mapped.z, 0.25, 1.0e-12);
  EXPECT_NEAR(geometry.physical_gradients[0][0], -1.0, 1.0e-12);
  EXPECT_NEAR(geometry.physical_gradients[1][0], 1.0, 1.0e-12);
}
