#include <algorithm>
#include <cmath>

#include <gtest/gtest.h>

#include "femsolver/kernel/algebra/conjugate_gradient.hpp"
#include "femsolver/kernel/assembly/curl_curl_assembler.hpp"
#include "femsolver/kernel/assembly/linear_magnetostatic_assembler.hpp"
#include "femsolver/kernel/mesh/tetra_mesh.hpp"
#include "femsolver/kernel/quadrature/tetrahedron_quadrature.hpp"
#include "femsolver/kernel/space/tetra_dof_map.hpp"

TEST(KernelLinearMagnetostaticAssemblerTest,
     CurrentAndRemanenceProduceNonzeroBoundaryConstrainedSmokeSolution) {
  const auto mesh = femsolver::kernel::mesh::BuildCentroidRefinedReferenceTetraMesh();
  const auto dof_map = femsolver::kernel::space::BuildFirstOrderNedelecDofMap(mesh);

  auto system = femsolver::kernel::assembly::AssembleLinearMagnetostaticSystem(
      mesh, dof_map, femsolver::kernel::quadrature::MakeCentroidTetrahedronQuadrature(),
      [](const femsolver::kernel::mesh::TetraCell&, const int cell_index) {
        femsolver::kernel::assembly::LinearMagnetostaticCellData data;
        data.reluctivity = cell_index == 2 ? 2.0 : 1.0;
        if (cell_index == 0) {
          data.current_density = {0.0, 0.0, 1.0};
        }
        if (cell_index == 1) {
          data.remanent_flux_density = {0.0, 0.0, 0.5};
        }
        return data;
      });

  const std::vector<double> zero_boundary_values(dof_map.boundary_dofs().size(), 0.0);
  femsolver::kernel::assembly::ApplyEdgeDirichletBoundary(
      femsolver::kernel::assembly::EdgeDirichletBoundaryCondition{dof_map.boundary_dofs(),
                                                                  zero_boundary_values},
      &system);

  const auto linear_system = system.Finalize();
  const auto result = femsolver::kernel::algebra::SolveConjugateGradient(
      linear_system.matrix, linear_system.rhs,
      femsolver::kernel::algebra::ConjugateGradientOptions{128, 1.0e-12});

  EXPECT_TRUE(result.converged);
  EXPECT_EQ(result.solution.size(), 10U);
  const auto max_entry = std::max_element(
      result.solution.begin(), result.solution.end(),
      [](const double left, const double right) { return std::abs(left) < std::abs(right); });
  ASSERT_NE(max_entry, result.solution.end());
  EXPECT_GT(std::abs(*max_entry), 1.0e-6);
}
