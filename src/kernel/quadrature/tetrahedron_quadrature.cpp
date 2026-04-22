#include "femsolver/kernel/quadrature/tetrahedron_quadrature.hpp"

#include <utility>

#include "femsolver/kernel/reference/reference_tetrahedron.hpp"

namespace femsolver::kernel::quadrature {

TetrahedronQuadratureRule::TetrahedronQuadratureRule(
    std::vector<TetrahedronQuadraturePoint> points,
    const int exact_degree)
    : points_(std::move(points)), exact_degree_(exact_degree) {}

const std::vector<TetrahedronQuadraturePoint>& TetrahedronQuadratureRule::points() const {
  return points_;
}

int TetrahedronQuadratureRule::exact_degree() const {
  return exact_degree_;
}

TetrahedronQuadratureRule MakeCentroidTetrahedronQuadrature() {
  return TetrahedronQuadratureRule({{{0.25, 0.25, 0.25}, reference::ReferenceTetrahedron::Volume()}},
                                   1);
}

}  // namespace femsolver::kernel::quadrature
