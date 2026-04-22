#include <gtest/gtest.h>

#include "femsolver/kernel/mesh/tetra_mesh.hpp"
#include "femsolver/kernel/space/tetra_dof_map.hpp"

TEST(KernelSpaceDofMapTest, LagrangeP1DofMapUsesNodesAsGlobalDofs) {
  const auto mesh = femsolver::kernel::mesh::BuildCentroidRefinedReferenceTetraMesh();
  const auto dof_map = femsolver::kernel::space::BuildLagrangeP1DofMap(mesh);

  EXPECT_EQ(dof_map.entity_kind(), femsolver::kernel::space::DofEntityKind::kNode);
  EXPECT_EQ(dof_map.global_dof_count(), 5);
  EXPECT_EQ(dof_map.boundary_dofs().size(), 4U);
  EXPECT_EQ(dof_map.CellDofs(0).size(), 4U);
}

TEST(KernelSpaceDofMapTest, FirstOrderNedelecDofMapUsesEdgesAndBoundaryEdges) {
  const auto mesh = femsolver::kernel::mesh::BuildCentroidRefinedReferenceTetraMesh();
  const auto dof_map = femsolver::kernel::space::BuildFirstOrderNedelecDofMap(mesh);

  EXPECT_EQ(dof_map.entity_kind(), femsolver::kernel::space::DofEntityKind::kEdge);
  EXPECT_EQ(dof_map.global_dof_count(), 10);
  EXPECT_EQ(dof_map.boundary_dofs().size(), 6U);
  ASSERT_EQ(dof_map.CellDofs(0).size(), 6U);
  for (const auto& dof : dof_map.CellDofs(0)) {
    EXPECT_GE(dof.index, 0);
    EXPECT_TRUE(dof.orientation == 1 || dof.orientation == -1);
  }
}
