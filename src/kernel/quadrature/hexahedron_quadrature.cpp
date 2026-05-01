#include "femsolver/kernel/quadrature/hexahedron_quadrature.hpp"

#include <cmath>
#include <stdexcept>
#include <utility>

namespace femsolver::kernel::quadrature {

HexahedronQuadratureRule::HexahedronQuadratureRule(std::vector<HexahedronQuadraturePoint> points)
    : points_(std::move(points)) {
  if (points_.empty()) {
    throw std::invalid_argument("Hexahedron quadrature requires at least one point");
  }
}

const std::vector<HexahedronQuadraturePoint>& HexahedronQuadratureRule::points() const {
  return points_;
}

double HexahedronQuadratureRule::WeightSum() const {
  double sum = 0.0;
  for (const auto& point : points_) {
    sum += point.weight;
  }
  return sum;
}

HexahedronQuadratureRule MakeHexahedronGaussLegendreRule(const int order) {
  std::vector<std::pair<double, double>> one_dimensional_points;
  if (order == 2) {
    const double coordinate = 1.0 / std::sqrt(3.0);
    one_dimensional_points = {{-coordinate, 1.0}, {coordinate, 1.0}};
  } else if (order == 3) {
    const double coordinate = std::sqrt(3.0 / 5.0);
    one_dimensional_points = {{-coordinate, 5.0 / 9.0}, {0.0, 8.0 / 9.0}, {coordinate, 5.0 / 9.0}};
  } else {
    throw std::invalid_argument("Hexahedron Gauss-Legendre rule supports order 2 or 3");
  }

  std::vector<HexahedronQuadraturePoint> points;
  for (const auto& xi : one_dimensional_points) {
    for (const auto& eta : one_dimensional_points) {
      for (const auto& zeta : one_dimensional_points) {
        points.push_back({{xi.first, eta.first, zeta.first}, xi.second * eta.second * zeta.second});
      }
    }
  }
  return HexahedronQuadratureRule(std::move(points));
}

}  // namespace femsolver::kernel::quadrature
