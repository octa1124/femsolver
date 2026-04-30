#pragma once

#include <map>
#include <string>
#include <vector>

namespace femsolver::coupling {

enum class FieldKind {
  kMagneticVectorPotential,
  kTemperature,
  kMechanicalDisplacement,
  kGenericScalar,
};

struct FieldBlock {
  std::string name;
  FieldKind kind = FieldKind::kGenericScalar;
  std::string unit;
  std::vector<double> values;
};

class FieldState {
 public:
  void AddField(FieldBlock field);

  [[nodiscard]] bool HasField(const std::string& name) const;
  [[nodiscard]] const FieldBlock& Field(const std::string& name) const;
  [[nodiscard]] FieldBlock& MutableField(const std::string& name);
  [[nodiscard]] const std::map<std::string, FieldBlock>& fields() const;
  [[nodiscard]] int TotalDofCount() const;
  [[nodiscard]] std::string Summary() const;

 private:
  std::map<std::string, FieldBlock> fields_;
};

std::string ToString(FieldKind kind);

}  // namespace femsolver::coupling
