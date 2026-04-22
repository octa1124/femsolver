#pragma once

#include <vector>

#include "femsolver/kernel/algebra/conjugate_gradient.hpp"
#include "femsolver/kernel/assembly/poisson_assembler.hpp"
#include "femsolver/kernel/mesh/tetra_mesh.hpp"
#include "femsolver/kernel/quadrature/tetrahedron_quadrature.hpp"

namespace femsolver::kernel::benchmark {

struct PoissonBenchmarkProblem {
  mesh::TetraMesh mesh;
  quadrature::TetrahedronQuadratureRule quadrature_rule;
  assembly::ScalarField source;
  assembly::ScalarField exact_solution;
  std::vector<int> dirichlet_node_ids;
};

struct PoissonBenchmarkResult {
  std::vector<double> solution;
  std::vector<double> exact_solution;
  algebra::ConjugateGradientResult linear_solver;
  double max_abs_error = 0.0;
};

PoissonBenchmarkProblem MakeManufacturedLinearPoissonBenchmark();
PoissonBenchmarkResult RunPoissonBenchmark(const PoissonBenchmarkProblem& problem);

}  // namespace femsolver::kernel::benchmark
