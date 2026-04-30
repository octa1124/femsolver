#include "femsolver/coupling/coupled_problem.hpp"

#include <sstream>
#include <stdexcept>

namespace femsolver::coupling {

namespace {

kernel::algebra::SparseMatrix EmptyMatrix(const int size) {
  return kernel::algebra::SparseMatrixBuilder(size, size).Build();
}

}  // namespace

bool CoupledAssembly::IsDimensionallyConsistent() const {
  return matrix.rows() == matrix.cols() && matrix.rows() == static_cast<int>(residual.size());
}

std::string CoupledAssembly::Summary() const {
  std::ostringstream stream;
  stream << "matrix=" << matrix.rows() << "x" << matrix.cols()
         << ", residual=" << residual.size() << ", warnings=" << warnings.size();
  return stream.str();
}

CoupledProblem::CoupledProblem(FieldState state) : state_(std::move(state)) {
  if (state_.TotalDofCount() <= 0) {
    throw std::invalid_argument("CoupledProblem requires at least one field DoF");
  }
}

void CoupledProblem::AddOperator(PhysicsOperator physics_operator) {
  for (const auto& existing_operator : operators_) {
    if (existing_operator.name() == physics_operator.name()) {
      throw std::invalid_argument("CoupledProblem duplicate operator: " + physics_operator.name());
    }
  }
  if (!physics_operator.CanAssemble(state_)) {
    throw std::invalid_argument("CoupledProblem operator has unmet field requirements: " +
                                physics_operator.name());
  }
  operators_.push_back(std::move(physics_operator));
}

const FieldState& CoupledProblem::state() const {
  return state_;
}

const std::vector<PhysicsOperator>& CoupledProblem::operators() const {
  return operators_;
}

CoupledAssembly CoupledProblem::Assemble() const {
  const int system_size = state_.TotalDofCount();
  kernel::algebra::SparseMatrixBuilder matrix_builder(system_size, system_size);
  std::vector<double> residual(static_cast<std::size_t>(system_size), 0.0);
  std::vector<std::string> warnings;

  for (const auto& physics_operator : operators_) {
    const auto operator_assembly = physics_operator.Assemble(state_);
    if (operator_assembly.matrix.rows() != system_size) {
      throw std::invalid_argument("CoupledProblem operator assembly size mismatch: " +
                                  physics_operator.name());
    }

    for (int row = 0; row < operator_assembly.matrix.rows(); ++row) {
      residual.at(static_cast<std::size_t>(row)) +=
          operator_assembly.residual.at(static_cast<std::size_t>(row));
      for (int entry = operator_assembly.matrix.row_offsets().at(static_cast<std::size_t>(row));
           entry < operator_assembly.matrix.row_offsets().at(static_cast<std::size_t>(row + 1));
           ++entry) {
        matrix_builder.Add(
            row, operator_assembly.matrix.col_indices().at(static_cast<std::size_t>(entry)),
            operator_assembly.matrix.values().at(static_cast<std::size_t>(entry)));
      }
    }
    warnings.insert(warnings.end(), operator_assembly.warnings.begin(),
                    operator_assembly.warnings.end());
  }

  return {operators_.empty() ? EmptyMatrix(system_size) : matrix_builder.Build(), residual, warnings};
}

std::string CoupledProblem::Summary() const {
  std::ostringstream stream;
  stream << "fields=" << state_.fields().size() << ", total_dofs=" << state_.TotalDofCount()
         << ", operators=" << operators_.size();
  return stream.str();
}

}  // namespace femsolver::coupling
