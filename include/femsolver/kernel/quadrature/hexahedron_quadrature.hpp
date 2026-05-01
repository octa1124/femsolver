#pragma once

#include <array>
#include <vector>

namespace femsolver::kernel::quadrature {

struct HexahedronQuadraturePoint {
  std::array<double, 3> reference_point{};
  double weight = 0.0;
};

class HexahedronQuadratureRule {
 public:
  explicit HexahedronQuadratureRule(std::vector<HexahedronQuadraturePoint> points);

  [[nodiscard]] const std::vector<HexahedronQuadraturePoint>& points() const;
  [[nodiscard]] double WeightSum() const;

 private:
  std::vector<HexahedronQuadraturePoint> points_;
};

HexahedronQuadratureRule MakeHexahedronGaussLegendreRule(int order);

}  // namespace femsolver::kernel::quadrature
