#include "femsolver/kernel/basis/lagrange_p1_tetrahedron.hpp"

namespace femsolver::kernel::basis {

std::array<double, 4> LagrangeP1Tetrahedron::Evaluate(
    const std::array<double, 3>& reference_point) {
  const double xi = reference_point[0];
  const double eta = reference_point[1];
  const double zeta = reference_point[2];
  return {1.0 - xi - eta - zeta, xi, eta, zeta};
}

std::array<std::array<double, 3>, 4> LagrangeP1Tetrahedron::ReferenceGradients() {
  return {{
      {{-1.0, -1.0, -1.0}},
      {{1.0, 0.0, 0.0}},
      {{0.0, 1.0, 0.0}},
      {{0.0, 0.0, 1.0}},
  }};
}

}  // namespace femsolver::kernel::basis
