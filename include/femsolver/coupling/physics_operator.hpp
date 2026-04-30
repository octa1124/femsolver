#pragma once

#include <functional>
#include <string>
#include <vector>

#include "femsolver/coupling/field_state.hpp"
#include "femsolver/kernel/algebra/sparse_matrix.hpp"

namespace femsolver::coupling {

enum class PhysicsOperatorKind {
  kMagnetostatic,
  kThermal,
  kMagnetoThermalCoupling,
  kSynthetic,
};

struct OperatorAssembly {
  kernel::algebra::SparseMatrix matrix;
  std::vector<double> residual;
  std::vector<std::string> warnings;

  [[nodiscard]] bool IsDimensionallyConsistent() const;
  [[nodiscard]] std::string Summary() const;
};

using OperatorAssemblyFunction = std::function<OperatorAssembly(const FieldState&)>;

class PhysicsOperator {
 public:
  PhysicsOperator(std::string name,
                  PhysicsOperatorKind kind,
                  std::vector<std::string> required_fields,
                  OperatorAssemblyFunction assemble);

  [[nodiscard]] const std::string& name() const;
  [[nodiscard]] PhysicsOperatorKind kind() const;
  [[nodiscard]] const std::vector<std::string>& required_fields() const;
  [[nodiscard]] bool CanAssemble(const FieldState& state) const;
  [[nodiscard]] OperatorAssembly Assemble(const FieldState& state) const;
  [[nodiscard]] std::string Summary() const;

 private:
  std::string name_;
  PhysicsOperatorKind kind_ = PhysicsOperatorKind::kSynthetic;
  std::vector<std::string> required_fields_;
  OperatorAssemblyFunction assemble_;
};

std::string ToString(PhysicsOperatorKind kind);

}  // namespace femsolver::coupling
