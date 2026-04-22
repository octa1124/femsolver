#include "femsolver/kernel/reference/reference_tetrahedron.hpp"

namespace femsolver::kernel::reference {

std::array<std::array<double, 3>, 4> ReferenceTetrahedron::Vertices() {
  return {{
      {{0.0, 0.0, 0.0}},
      {{1.0, 0.0, 0.0}},
      {{0.0, 1.0, 0.0}},
      {{0.0, 0.0, 1.0}},
  }};
}

double ReferenceTetrahedron::Volume() {
  return 1.0 / 6.0;
}

bool ReferenceTetrahedron::Contains(const std::array<double, 3>& reference_point,
                                    const double tolerance) {
  const double xi = reference_point[0];
  const double eta = reference_point[1];
  const double zeta = reference_point[2];
  const double lambda0 = 1.0 - xi - eta - zeta;
  return xi >= -tolerance && eta >= -tolerance && zeta >= -tolerance &&
         lambda0 >= -tolerance;
}

}  // namespace femsolver::kernel::reference
