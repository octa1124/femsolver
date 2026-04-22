#pragma once

#include <array>

#include "femsolver/kernel/mesh/tetra_mesh.hpp"

namespace femsolver::kernel::assembly {

using Vector3 = std::array<double, 3>;

struct ElementGeometry {
  std::array<mesh::Point3D, 4> node_positions{};
  std::array<std::array<double, 3>, 3> jacobian{};
  std::array<std::array<double, 3>, 3> inverse_jacobian{};
  double determinant = 0.0;
  double volume = 0.0;
  std::array<Vector3, 4> physical_gradients{};

  [[nodiscard]] mesh::Point3D MapToPhysical(const Vector3& reference_point) const;
  [[nodiscard]] Vector3 MapReferenceHcurlVectorToPhysical(const Vector3& reference_vector) const;
  [[nodiscard]] Vector3 MapReferenceCurlToPhysical(const Vector3& reference_curl) const;
};

ElementGeometry BuildElementGeometry(const mesh::TetraMesh& tetra_mesh, int cell_index);

}  // namespace femsolver::kernel::assembly
