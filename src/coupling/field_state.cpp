#include "femsolver/coupling/field_state.hpp"

#include <sstream>
#include <stdexcept>

namespace femsolver::coupling {

void FieldState::AddField(FieldBlock field) {
  if (field.name.empty()) {
    throw std::invalid_argument("FieldState requires non-empty field names");
  }
  if (field.values.empty()) {
    throw std::invalid_argument("FieldState field blocks must contain at least one value");
  }
  const std::string field_name = field.name;
  const auto [_, inserted] = fields_.emplace(field_name, std::move(field));
  if (!inserted) {
    throw std::invalid_argument("FieldState duplicate field: " + field_name);
  }
}

bool FieldState::HasField(const std::string& name) const {
  return fields_.find(name) != fields_.end();
}

const FieldBlock& FieldState::Field(const std::string& name) const {
  return fields_.at(name);
}

FieldBlock& FieldState::MutableField(const std::string& name) {
  return fields_.at(name);
}

const std::map<std::string, FieldBlock>& FieldState::fields() const {
  return fields_;
}

int FieldState::TotalDofCount() const {
  int total = 0;
  for (const auto& [_, field] : fields_) {
    total += static_cast<int>(field.values.size());
  }
  return total;
}

std::string FieldState::Summary() const {
  std::ostringstream stream;
  stream << "fields=" << fields_.size() << ", total_dofs=" << TotalDofCount();
  for (const auto& [name, field] : fields_) {
    stream << "; " << name << ":" << ToString(field.kind) << "[" << field.values.size()
           << "] unit=" << (field.unit.empty() ? "unspecified" : field.unit);
  }
  return stream.str();
}

std::string ToString(const FieldKind kind) {
  switch (kind) {
    case FieldKind::kMagneticVectorPotential:
      return "magnetic-vector-potential";
    case FieldKind::kTemperature:
      return "temperature";
    case FieldKind::kMechanicalDisplacement:
      return "mechanical-displacement";
    case FieldKind::kGenericScalar:
      return "generic-scalar";
  }
  return "unknown";
}

}  // namespace femsolver::coupling
