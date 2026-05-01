#pragma once

#include <array>

namespace femsolver::kernel::basis {

using HexVector3 = std::array<double, 3>;

class LagrangeQ1Hexahedron {
 public:
  [[nodiscard]] static const std::array<HexVector3, 8>& Nodes();
  [[nodiscard]] static std::array<double, 8> Evaluate(const HexVector3& reference_point);
  [[nodiscard]] static std::array<HexVector3, 8> ReferenceGradients(
      const HexVector3& reference_point);
};

class LagrangeQ2Hexahedron {
 public:
  [[nodiscard]] static const std::array<HexVector3, 27>& Nodes();
  [[nodiscard]] static std::array<double, 27> Evaluate(const HexVector3& reference_point);
  [[nodiscard]] static std::array<HexVector3, 27> ReferenceGradients(
      const HexVector3& reference_point);
};

}  // namespace femsolver::kernel::basis
