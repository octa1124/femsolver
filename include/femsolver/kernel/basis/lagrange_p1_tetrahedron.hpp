#pragma once

#include <array>

namespace femsolver::kernel::basis {

// Linear nodal basis on the reference tetrahedron.
class LagrangeP1Tetrahedron {
 public:
  [[nodiscard]] static std::array<double, 4> Evaluate(const std::array<double, 3>& reference_point);
  [[nodiscard]] static std::array<std::array<double, 3>, 4> ReferenceGradients();
};

}  // namespace femsolver::kernel::basis
