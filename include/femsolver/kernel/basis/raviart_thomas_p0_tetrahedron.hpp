#pragma once

#include <array>

#include "femsolver/kernel/basis/vector_types.hpp"

namespace femsolver::kernel::basis {

class RaviartThomasP0Tetrahedron {
 public:
  [[nodiscard]] static const std::array<Vector3, 4>& Vertices();
  [[nodiscard]] static const std::array<std::array<int, 3>, 4>& LocalFaces();
  [[nodiscard]] static std::array<Vector3, 4> Evaluate(const Vector3& reference_point);
  [[nodiscard]] static std::array<double, 4> ReferenceDivergences();
  [[nodiscard]] static double ReferenceFaceFlux(int basis_index, int face_index);
  [[nodiscard]] static std::array<std::array<double, 4>, 4> ReferenceFaceFluxMatrix();
};

}  // namespace femsolver::kernel::basis
