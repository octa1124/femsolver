#include "femsolver/coupling/physics_operator.hpp"

#include <sstream>
#include <stdexcept>

namespace femsolver::coupling {

bool OperatorAssembly::IsDimensionallyConsistent() const {
  return matrix.rows() == matrix.cols() && matrix.rows() == static_cast<int>(residual.size());
}

std::string OperatorAssembly::Summary() const {
  std::ostringstream stream;
  stream << "matrix=" << matrix.rows() << "x" << matrix.cols()
         << ", residual=" << residual.size() << ", warnings=" << warnings.size();
  return stream.str();
}

PhysicsOperator::PhysicsOperator(std::string name,
                                 const PhysicsOperatorKind kind,
                                 std::vector<std::string> required_fields,
                                 OperatorAssemblyFunction assemble)
    : name_(std::move(name)),
      kind_(kind),
      required_fields_(std::move(required_fields)),
      assemble_(std::move(assemble)) {
  if (name_.empty()) {
    throw std::invalid_argument("PhysicsOperator requires a non-empty name");
  }
  if (!assemble_) {
    throw std::invalid_argument("PhysicsOperator requires an assembly callback");
  }
}

const std::string& PhysicsOperator::name() const {
  return name_;
}

PhysicsOperatorKind PhysicsOperator::kind() const {
  return kind_;
}

const std::vector<std::string>& PhysicsOperator::required_fields() const {
  return required_fields_;
}

bool PhysicsOperator::CanAssemble(const FieldState& state) const {
  for (const auto& field_name : required_fields_) {
    if (!state.HasField(field_name)) {
      return false;
    }
  }
  return true;
}

OperatorAssembly PhysicsOperator::Assemble(const FieldState& state) const {
  if (!CanAssemble(state)) {
    throw std::invalid_argument("PhysicsOperator missing required field: " + name_);
  }
  auto assembly = assemble_(state);
  if (!assembly.IsDimensionallyConsistent()) {
    throw std::invalid_argument("PhysicsOperator returned inconsistent assembly: " + name_);
  }
  return assembly;
}

std::string PhysicsOperator::Summary() const {
  std::ostringstream stream;
  stream << "name=" << name_ << ", kind=" << ToString(kind_)
         << ", required_fields=" << required_fields_.size();
  return stream.str();
}

std::string ToString(const PhysicsOperatorKind kind) {
  switch (kind) {
    case PhysicsOperatorKind::kMagnetostatic:
      return "magnetostatic";
    case PhysicsOperatorKind::kThermal:
      return "thermal";
    case PhysicsOperatorKind::kMagnetoThermalCoupling:
      return "magneto-thermal-coupling";
    case PhysicsOperatorKind::kSynthetic:
      return "synthetic";
  }
  return "unknown";
}

}  // namespace femsolver::coupling
