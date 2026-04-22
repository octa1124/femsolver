#pragma once

#include <vector>

#include "femsolver/kernel/algebra/sparse_matrix.hpp"

namespace femsolver::kernel::algebra {

struct ConjugateGradientOptions {
  int max_iterations = 500;
  double absolute_tolerance = 1.0e-12;
};

struct ConjugateGradientResult {
  std::vector<double> solution;
  int iterations = 0;
  double residual_norm = 0.0;
  bool converged = false;
};

ConjugateGradientResult SolveConjugateGradient(const SparseMatrix& matrix,
                                               const std::vector<double>& rhs,
                                               const ConjugateGradientOptions& options = {});

}  // namespace femsolver::kernel::algebra
