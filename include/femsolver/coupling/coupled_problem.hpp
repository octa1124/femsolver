#pragma once

#include <string>
#include <vector>

#include "femsolver/coupling/field_state.hpp"
#include "femsolver/coupling/physics_operator.hpp"

namespace femsolver::coupling {

struct CoupledAssembly {
  kernel::algebra::SparseMatrix matrix;
  std::vector<double> residual;
  std::vector<std::string> warnings;

  [[nodiscard]] bool IsDimensionallyConsistent() const;
  [[nodiscard]] std::string Summary() const;
};

class CoupledProblem {
 public:
  explicit CoupledProblem(FieldState state);

  void AddOperator(PhysicsOperator physics_operator);

  [[nodiscard]] const FieldState& state() const;
  [[nodiscard]] const std::vector<PhysicsOperator>& operators() const;
  [[nodiscard]] CoupledAssembly Assemble() const;
  [[nodiscard]] std::string Summary() const;

 private:
  FieldState state_;
  std::vector<PhysicsOperator> operators_;
};

}  // namespace femsolver::coupling
