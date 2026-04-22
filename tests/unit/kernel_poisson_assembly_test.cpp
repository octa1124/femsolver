#include <gtest/gtest.h>

#include "femsolver/kernel/assembly/element_geometry.hpp"
#include "femsolver/kernel/assembly/poisson_assembler.hpp"
#include "femsolver/kernel/mesh/tetra_mesh.hpp"
#include "femsolver/kernel/quadrature/tetrahedron_quadrature.hpp"

TEST(KernelPoissonAssemblyTest, LocalReferenceElementMatrixIsSymmetricWithZeroRowSum) {
  femsolver::kernel::mesh::TetraMesh mesh(
      {{0.0, 0.0, 0.0},
       {1.0, 0.0, 0.0},
       {0.0, 1.0, 0.0},
       {0.0, 0.0, 1.0}},
      {{{0, 1, 2, 3}, 1}},
      {{{0, 2, 1}, 1}, {{0, 1, 3}, 1}, {{1, 2, 3}, 1}, {{0, 3, 2}, 1}});

  const auto geometry = femsolver::kernel::assembly::BuildElementGeometry(mesh, 0);
  const auto quadrature = femsolver::kernel::quadrature::MakeCentroidTetrahedronQuadrature();
  const auto element = femsolver::kernel::assembly::AssemblePoissonElement(
      geometry, quadrature, [](const femsolver::kernel::mesh::Point3D&) { return 0.0; });

  for (std::size_t row = 0; row < element.stiffness.size(); ++row) {
    double row_sum = 0.0;
    for (std::size_t col = 0; col < element.stiffness[row].size(); ++col) {
      EXPECT_NEAR(element.stiffness[row][col], element.stiffness[col][row], 1.0e-12);
      row_sum += element.stiffness[row][col];
    }
    EXPECT_NEAR(row_sum, 0.0, 1.0e-12);
    EXPECT_NEAR(element.load[row], 0.0, 1.0e-12);
  }
}

TEST(KernelPoissonAssemblyTest, DirichletApplicationTurnsBoundaryRowsIntoIdentity) {
  const auto mesh = femsolver::kernel::mesh::BuildCentroidRefinedReferenceTetraMesh();
  const auto quadrature = femsolver::kernel::quadrature::MakeCentroidTetrahedronQuadrature();
  auto system = femsolver::kernel::assembly::AssemblePoissonSystem(
      mesh, quadrature, [](const femsolver::kernel::mesh::Point3D&) { return 0.0; });

  femsolver::kernel::assembly::ApplyDirichletBoundary(
      mesh,
      {mesh.BoundaryNodeIds(),
       [](const femsolver::kernel::mesh::Point3D& point) { return point.x + point.y + point.z; }},
      &system);

  const auto linear_system = system.Finalize();
  for (const int node_id : mesh.BoundaryNodeIds()) {
    EXPECT_NEAR(linear_system.matrix.Entry(node_id, node_id), 1.0, 1.0e-12);
    EXPECT_NEAR(linear_system.rhs.at(static_cast<std::size_t>(node_id)),
                mesh.nodes().at(static_cast<std::size_t>(node_id)).x +
                    mesh.nodes().at(static_cast<std::size_t>(node_id)).y +
                    mesh.nodes().at(static_cast<std::size_t>(node_id)).z,
                1.0e-12);
  }
}
