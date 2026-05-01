#pragma once

#include <array>
#include <string>
#include <vector>

namespace femsolver::material {

using Vector3 = std::array<double, 3>;
using Matrix3 = std::array<std::array<double, 3>, 3>;

struct BhPoint {
  double flux_density = 0.0;
  double field_intensity = 0.0;
};

struct TangentCheckResult {
  bool passed = false;
  double max_abs_error = 0.0;
  double tolerance = 0.0;
};

class MonotoneBhCurve {
 public:
  explicit MonotoneBhCurve(std::vector<BhPoint> points);

  [[nodiscard]] const std::vector<BhPoint>& points() const;
  [[nodiscard]] double FieldIntensity(double flux_density_magnitude) const;
  [[nodiscard]] double DifferentialReluctivity(double flux_density_magnitude) const;
  [[nodiscard]] double SecantReluctivity(double flux_density_magnitude) const;
  [[nodiscard]] double EnergyDensity(double flux_density_magnitude) const;
  [[nodiscard]] bool IsMonotone() const;
  [[nodiscard]] std::string Summary() const;

 private:
  [[nodiscard]] int SegmentIndex(double flux_density_magnitude) const;

  std::vector<BhPoint> points_;
};

class OrthotropicBhMaterial {
 public:
  OrthotropicBhMaterial(MonotoneBhCurve x_axis,
                        MonotoneBhCurve y_axis,
                        MonotoneBhCurve z_axis);

  [[nodiscard]] Vector3 FieldIntensity(const Vector3& flux_density) const;
  [[nodiscard]] Matrix3 ConsistentTangent(const Vector3& flux_density) const;
  [[nodiscard]] double EnergyDensity(const Vector3& flux_density) const;
  [[nodiscard]] TangentCheckResult CheckConsistentTangent(const Vector3& flux_density,
                                                         double step,
                                                         double tolerance) const;

 private:
  std::array<MonotoneBhCurve, 3> axes_;
};

}  // namespace femsolver::material
