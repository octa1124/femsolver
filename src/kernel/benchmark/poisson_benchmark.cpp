#include "femsolver/kernel/benchmark/poisson_benchmark.hpp"

#include <algorithm>
#include <cmath>

namespace femsolver::kernel::benchmark {

PoissonBenchmarkProblem MakeManufacturedLinearPoissonBenchmark() {
  PoissonBenchmarkProblem problem{
      mesh::BuildCentroidRefinedReferenceTetraMesh(),
      quadrature::MakeCentroidTetrahedronQuadrature(),
      [](const mesh::Point3D&) { return 0.0; },
      [](const mesh::Point3D& point) { return point.x + 2.0 * point.y + 3.0 * point.z; },
      {},
  };
  problem.dirichlet_node_ids = problem.mesh.BoundaryNodeIds();
  return problem;
}

PoissonBenchmarkResult RunPoissonBenchmark(const PoissonBenchmarkProblem& problem) {
  auto assembled_system =
      assembly::AssemblePoissonSystem(problem.mesh, problem.quadrature_rule, problem.source);
  assembly::ApplyDirichletBoundary(
      problem.mesh,
      assembly::DirichletBoundaryCondition{problem.dirichlet_node_ids, problem.exact_solution},
      &assembled_system);

  const algebra::LinearSystem linear_system = assembled_system.Finalize();

  PoissonBenchmarkResult result;
  result.linear_solver = algebra::SolveConjugateGradient(
      linear_system.matrix, linear_system.rhs, algebra::ConjugateGradientOptions{64, 1.0e-14});
  result.solution = result.linear_solver.solution;
  result.exact_solution.resize(problem.mesh.nodes().size(), 0.0);

  for (std::size_t node_index = 0; node_index < problem.mesh.nodes().size(); ++node_index) {
    const double exact_value = problem.exact_solution(problem.mesh.nodes().at(node_index));
    result.exact_solution.at(node_index) = exact_value;
    result.max_abs_error =
        std::max(result.max_abs_error, std::abs(result.solution.at(node_index) - exact_value));
  }

  return result;
}

}  // namespace femsolver::kernel::benchmark
