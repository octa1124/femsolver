#pragma once

#include <array>

namespace femsolver::kernel::reference {

// Reference tetrahedron used by the first scalar FEM kernel.
class ReferenceTetrahedron {
 public:
  [[nodiscard]] static std::array<std::array<double, 3>, 4> Vertices();
  [[nodiscard]] static double Volume();
  [[nodiscard]] static bool Contains(const std::array<double, 3>& reference_point,
                                     double tolerance = 1.0e-12);
};

}  // namespace femsolver::kernel::reference
