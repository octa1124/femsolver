#include "femsolver/kernel/assembly/element_geometry.hpp"

#include <stdexcept>

#include "femsolver/kernel/basis/lagrange_p1_tetrahedron.hpp"

namespace femsolver::kernel::assembly {

namespace {

std::array<std::array<double, 3>, 3> Invert3x3(const std::array<std::array<double, 3>, 3>& matrix,
                                               double* determinant) {
  const double a00 = matrix[0][0];
  const double a01 = matrix[0][1];
  const double a02 = matrix[0][2];
  const double a10 = matrix[1][0];
  const double a11 = matrix[1][1];
  const double a12 = matrix[1][2];
  const double a20 = matrix[2][0];
  const double a21 = matrix[2][1];
  const double a22 = matrix[2][2];

  *determinant = a00 * (a11 * a22 - a12 * a21) - a01 * (a10 * a22 - a12 * a20) +
                 a02 * (a10 * a21 - a11 * a20);

  if (*determinant == 0.0) {
    throw std::runtime_error("Singular tetrahedral Jacobian");
  }

  const double inverse_determinant = 1.0 / *determinant;
  return {{
      {{
          (a11 * a22 - a12 * a21) * inverse_determinant,
          (a02 * a21 - a01 * a22) * inverse_determinant,
          (a01 * a12 - a02 * a11) * inverse_determinant,
      }},
      {{
          (a12 * a20 - a10 * a22) * inverse_determinant,
          (a00 * a22 - a02 * a20) * inverse_determinant,
          (a02 * a10 - a00 * a12) * inverse_determinant,
      }},
      {{
          (a10 * a21 - a11 * a20) * inverse_determinant,
          (a01 * a20 - a00 * a21) * inverse_determinant,
          (a00 * a11 - a01 * a10) * inverse_determinant,
      }},
  }};
}

Vector3 MultiplyTranspose(const std::array<std::array<double, 3>, 3>& matrix,
                          const Vector3& vector) {
  return {
      matrix[0][0] * vector[0] + matrix[1][0] * vector[1] + matrix[2][0] * vector[2],
      matrix[0][1] * vector[0] + matrix[1][1] * vector[1] + matrix[2][1] * vector[2],
      matrix[0][2] * vector[0] + matrix[1][2] * vector[1] + matrix[2][2] * vector[2],
  };
}

}  // namespace

mesh::Point3D ElementGeometry::MapToPhysical(const Vector3& reference_point) const {
  const auto& origin = node_positions[0];
  return {
      origin.x + jacobian[0][0] * reference_point[0] + jacobian[0][1] * reference_point[1] +
          jacobian[0][2] * reference_point[2],
      origin.y + jacobian[1][0] * reference_point[0] + jacobian[1][1] * reference_point[1] +
          jacobian[1][2] * reference_point[2],
      origin.z + jacobian[2][0] * reference_point[0] + jacobian[2][1] * reference_point[1] +
          jacobian[2][2] * reference_point[2],
  };
}

ElementGeometry BuildElementGeometry(const mesh::TetraMesh& tetra_mesh, const int cell_index) {
  const auto& cell = tetra_mesh.cells().at(static_cast<std::size_t>(cell_index));
  ElementGeometry geometry;
  for (std::size_t local_node = 0; local_node < geometry.node_positions.size(); ++local_node) {
    geometry.node_positions[local_node] =
        tetra_mesh.nodes().at(static_cast<std::size_t>(cell.node_ids[local_node]));
  }

  const auto& x0 = geometry.node_positions[0];
  const auto& x1 = geometry.node_positions[1];
  const auto& x2 = geometry.node_positions[2];
  const auto& x3 = geometry.node_positions[3];

  geometry.jacobian = {{
      {{x1.x - x0.x, x2.x - x0.x, x3.x - x0.x}},
      {{x1.y - x0.y, x2.y - x0.y, x3.y - x0.y}},
      {{x1.z - x0.z, x2.z - x0.z, x3.z - x0.z}},
  }};

  geometry.inverse_jacobian = Invert3x3(geometry.jacobian, &geometry.determinant);
  geometry.volume = geometry.determinant / 6.0;

  const auto reference_gradients = basis::LagrangeP1Tetrahedron::ReferenceGradients();
  for (std::size_t local_node = 0; local_node < geometry.physical_gradients.size(); ++local_node) {
    geometry.physical_gradients[local_node] =
        MultiplyTranspose(geometry.inverse_jacobian, reference_gradients[local_node]);
  }

  return geometry;
}

}  // namespace femsolver::kernel::assembly
