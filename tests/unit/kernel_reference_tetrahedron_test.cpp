#include <gtest/gtest.h>

#include "femsolver/kernel/reference/reference_tetrahedron.hpp"

TEST(KernelReferenceTetrahedronTest, ProvidesReferenceVerticesAndContainment) {
  const auto vertices = femsolver::kernel::reference::ReferenceTetrahedron::Vertices();

  EXPECT_EQ(vertices.size(), 4U);
  EXPECT_DOUBLE_EQ(vertices[1][0], 1.0);
  EXPECT_NEAR(femsolver::kernel::reference::ReferenceTetrahedron::Volume(), 1.0 / 6.0, 1.0e-12);
  EXPECT_TRUE(
      femsolver::kernel::reference::ReferenceTetrahedron::Contains({0.25, 0.25, 0.25}));
  EXPECT_FALSE(
      femsolver::kernel::reference::ReferenceTetrahedron::Contains({0.7, 0.3, 0.3}));
}
