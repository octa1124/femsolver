#include "femsolver/kernel/algebra/conjugate_gradient.hpp"

#include <cmath>
#include <stdexcept>

namespace femsolver::kernel::algebra {

namespace {

double Dot(const std::vector<double>& a, const std::vector<double>& b) {
  if (a.size() != b.size()) {
    throw std::invalid_argument("Dot product dimension mismatch");
  }

  double result = 0.0;
  for (std::size_t index = 0; index < a.size(); ++index) {
    result += a.at(index) * b.at(index);
  }
  return result;
}

std::vector<double> Subtract(const std::vector<double>& a, const std::vector<double>& b) {
  if (a.size() != b.size()) {
    throw std::invalid_argument("Vector subtraction dimension mismatch");
  }

  std::vector<double> result(a.size(), 0.0);
  for (std::size_t index = 0; index < a.size(); ++index) {
    result.at(index) = a.at(index) - b.at(index);
  }
  return result;
}

double Norm(const std::vector<double>& values) {
  return std::sqrt(Dot(values, values));
}

}  // namespace

ConjugateGradientResult SolveConjugateGradient(const SparseMatrix& matrix,
                                               const std::vector<double>& rhs,
                                               const ConjugateGradientOptions& options) {
  if (matrix.rows() != matrix.cols() || matrix.rows() != static_cast<int>(rhs.size())) {
    throw std::invalid_argument("Conjugate gradient system dimension mismatch");
  }

  ConjugateGradientResult result;
  result.solution.assign(rhs.size(), 0.0);

  std::vector<double> residual = rhs;
  result.residual_norm = Norm(residual);
  if (result.residual_norm <= options.absolute_tolerance) {
    result.converged = true;
    return result;
  }

  std::vector<double> direction = residual;
  double residual_dot = Dot(residual, residual);

  for (int iteration = 0; iteration < options.max_iterations; ++iteration) {
    const std::vector<double> matrix_direction = matrix.Multiply(direction);
    const double denominator = Dot(direction, matrix_direction);
    if (std::abs(denominator) <= 1.0e-30) {
      break;
    }

    const double alpha = residual_dot / denominator;
    for (std::size_t index = 0; index < result.solution.size(); ++index) {
      result.solution.at(index) += alpha * direction.at(index);
      residual.at(index) -= alpha * matrix_direction.at(index);
    }

    result.iterations = iteration + 1;
    result.residual_norm = Norm(residual);
    if (result.residual_norm <= options.absolute_tolerance) {
      result.converged = true;
      return result;
    }

    const double new_residual_dot = Dot(residual, residual);
    const double beta = new_residual_dot / residual_dot;
    for (std::size_t index = 0; index < direction.size(); ++index) {
      direction.at(index) = residual.at(index) + beta * direction.at(index);
    }
    residual_dot = new_residual_dot;
  }

  result.residual_norm = Norm(Subtract(matrix.Multiply(result.solution), rhs));
  return result;
}

}  // namespace femsolver::kernel::algebra
