#pragma once

#include <array>

#include "femsolver/kernel/basis/vector_types.hpp"

namespace femsolver::kernel::basis {

class FirstOrderNedelecTetrahedron {
 public:
  [[nodiscard]] static const std::array<std::array<int, 2>, 6>& LocalEdges();
  [[nodiscard]] static std::array<Vector3, 6> Evaluate(const Vector3& reference_point);
  [[nodiscard]] static std::array<Vector3, 6> ReferenceCurls();
};

}  // namespace femsolver::kernel::basis
