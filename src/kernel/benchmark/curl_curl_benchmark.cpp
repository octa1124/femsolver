#include "femsolver/kernel/benchmark/curl_curl_benchmark.hpp"

#include <algorithm>
#include <cmath>

#include "femsolver/kernel/space/tetra_dof_map.hpp"

namespace femsolver::kernel::benchmark {

namespace {

double ExactEdgeDof(const mesh::Point3D& start,
                    const mesh::Point3D& end,
                    const assembly::VectorField& exact_potential) {
  const mesh::Point3D midpoint{
      0.5 * (start.x + end.x),
      0.5 * (start.y + end.y),
      0.5 * (start.z + end.z),
  };
  const auto potential = exact_potential(midpoint);
  const assembly::Vector3 tangent{
      end.x - start.x,
      end.y - start.y,
      end.z - start.z,
  };
  return potential[0] * tangent[0] + potential[1] * tangent[1] + potential[2] * tangent[2];
}

}  // namespace

CurlCurlBenchmarkProblem MakeManufacturedCurlCurlBenchmark() {
  return {
      mesh::BuildCentroidRefinedReferenceTetraMesh(),
      quadrature::MakeCentroidTetrahedronQuadrature(),
      [](const mesh::Point3D&) { return assembly::Vector3{0.0, 0.0, 0.0}; },
      [](const mesh::Point3D& point) { return assembly::Vector3{-0.5 * point.y, 0.5 * point.x, 0.0}; },
  };
}

CurlCurlBenchmarkResult RunCurlCurlBenchmark(const CurlCurlBenchmarkProblem& problem) {
  const auto dof_map = space::BuildFirstOrderNedelecDofMap(problem.mesh);
  auto system = assembly::AssembleCurlCurlSystem(problem.mesh, dof_map, problem.quadrature_rule,
                                                 problem.source);

  std::vector<double> exact_dofs(static_cast<std::size_t>(dof_map.global_dof_count()), 0.0);
  for (std::size_t edge_index = 0; edge_index < problem.mesh.edges().size(); ++edge_index) {
    const auto& edge = problem.mesh.edges()[edge_index];
    const auto& start = problem.mesh.nodes().at(static_cast<std::size_t>(edge.node_ids[0]));
    const auto& end = problem.mesh.nodes().at(static_cast<std::size_t>(edge.node_ids[1]));
    exact_dofs[edge_index] = ExactEdgeDof(start, end, problem.exact_potential);
  }

  std::vector<double> boundary_values;
  boundary_values.reserve(dof_map.boundary_dofs().size());
  for (const int boundary_dof : dof_map.boundary_dofs()) {
    boundary_values.push_back(exact_dofs[static_cast<std::size_t>(boundary_dof)]);
  }
  assembly::ApplyEdgeDirichletBoundary(
      assembly::EdgeDirichletBoundaryCondition{dof_map.boundary_dofs(), boundary_values}, &system);

  const algebra::LinearSystem linear_system = system.Finalize();

  CurlCurlBenchmarkResult result;
  result.linear_solver = algebra::SolveConjugateGradient(
      linear_system.matrix, linear_system.rhs, algebra::ConjugateGradientOptions{128, 1.0e-14});
  result.solution_dofs = result.linear_solver.solution;
  result.exact_dofs = exact_dofs;

  for (std::size_t dof_index = 0; dof_index < result.solution_dofs.size(); ++dof_index) {
    result.max_abs_error = std::max(
        result.max_abs_error,
        std::abs(result.solution_dofs[dof_index] - result.exact_dofs[dof_index]));
  }

  return result;
}

}  // namespace femsolver::kernel::benchmark
