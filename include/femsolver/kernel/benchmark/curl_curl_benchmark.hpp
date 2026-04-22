#pragma once

#include <vector>

#include "femsolver/kernel/algebra/conjugate_gradient.hpp"
#include "femsolver/kernel/assembly/curl_curl_assembler.hpp"
#include "femsolver/kernel/mesh/tetra_mesh.hpp"
#include "femsolver/kernel/quadrature/tetrahedron_quadrature.hpp"

namespace femsolver::kernel::benchmark {

struct CurlCurlBenchmarkProblem {
  mesh::TetraMesh mesh;
  quadrature::TetrahedronQuadratureRule quadrature_rule;
  assembly::VectorField source;
  assembly::VectorField exact_potential;
};

struct CurlCurlBenchmarkResult {
  std::vector<double> solution_dofs;
  std::vector<double> exact_dofs;
  algebra::ConjugateGradientResult linear_solver;
  double max_abs_error = 0.0;
};

CurlCurlBenchmarkProblem MakeManufacturedCurlCurlBenchmark();
CurlCurlBenchmarkResult RunCurlCurlBenchmark(const CurlCurlBenchmarkProblem& problem);

}  // namespace femsolver::kernel::benchmark
