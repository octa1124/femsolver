#pragma once

#include <array>

namespace femsolver::kernel::reference {

class ReferenceHexahedron {
 public:
  [[nodiscard]] static std::array<std::array<double, 3>, 8> Vertices();
  [[nodiscard]] static double Volume();
  [[nodiscard]] static bool Contains(const std::array<double, 3>& reference_point,
                                     double tolerance = 1.0e-12);
};

}  // namespace femsolver::kernel::reference
