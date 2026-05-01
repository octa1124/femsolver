#include "femsolver/kernel/reference/reference_hexahedron.hpp"

namespace femsolver::kernel::reference {

std::array<std::array<double, 3>, 8> ReferenceHexahedron::Vertices() {
  return {{
      {-1.0, -1.0, -1.0},
      {1.0, -1.0, -1.0},
      {1.0, 1.0, -1.0},
      {-1.0, 1.0, -1.0},
      {-1.0, -1.0, 1.0},
      {1.0, -1.0, 1.0},
      {1.0, 1.0, 1.0},
      {-1.0, 1.0, 1.0},
  }};
}

double ReferenceHexahedron::Volume() {
  return 8.0;
}

bool ReferenceHexahedron::Contains(const std::array<double, 3>& reference_point,
                                   const double tolerance) {
  return reference_point[0] >= -1.0 - tolerance && reference_point[0] <= 1.0 + tolerance &&
         reference_point[1] >= -1.0 - tolerance && reference_point[1] <= 1.0 + tolerance &&
         reference_point[2] >= -1.0 - tolerance && reference_point[2] <= 1.0 + tolerance;
}

}  // namespace femsolver::kernel::reference
