#pragma once

#include <array>
#include <vector>

namespace femsolver::kernel::quadrature {

struct TetrahedronQuadraturePoint {
  std::array<double, 3> reference_point{};
  double weight = 0.0;
};

// Quadrature rule on the reference tetrahedron.
class TetrahedronQuadratureRule {
 public:
  TetrahedronQuadratureRule(std::vector<TetrahedronQuadraturePoint> points, int exact_degree);

  [[nodiscard]] const std::vector<TetrahedronQuadraturePoint>& points() const;
  [[nodiscard]] int exact_degree() const;

 private:
  std::vector<TetrahedronQuadraturePoint> points_;
  int exact_degree_ = 0;
};

// One-point centroid rule, exact for linear functions on the reference tetrahedron.
TetrahedronQuadratureRule MakeCentroidTetrahedronQuadrature();

}  // namespace femsolver::kernel::quadrature
