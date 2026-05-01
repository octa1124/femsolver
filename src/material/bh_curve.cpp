#include "femsolver/material/bh_curve.hpp"

#include <algorithm>
#include <cmath>
#include <sstream>
#include <stdexcept>

namespace femsolver::material {

namespace {

constexpr double kNearZero = 1.0e-14;

double AbsComponent(const double value) {
  return std::abs(value);
}

double Sign(const double value) {
  return value < 0.0 ? -1.0 : 1.0;
}

void ValidatePoints(const std::vector<BhPoint>& points) {
  if (points.size() < 2) {
    throw std::invalid_argument("B-H curve requires at least two points");
  }
  if (points.front().flux_density != 0.0 || points.front().field_intensity != 0.0) {
    throw std::invalid_argument("B-H curve must start at the origin");
  }
  for (std::size_t index = 1; index < points.size(); ++index) {
    if (points[index].flux_density <= points[index - 1].flux_density) {
      throw std::invalid_argument("B-H flux-density samples must be strictly increasing");
    }
    if (points[index].field_intensity < points[index - 1].field_intensity) {
      throw std::invalid_argument("B-H field-intensity samples must be monotone increasing");
    }
  }
}

double InterpolateOnSegment(const BhPoint& left, const BhPoint& right, const double b) {
  const double t = (b - left.flux_density) / (right.flux_density - left.flux_density);
  return left.field_intensity + t * (right.field_intensity - left.field_intensity);
}

double SlopeOnSegment(const BhPoint& left, const BhPoint& right) {
  return (right.field_intensity - left.field_intensity) /
         (right.flux_density - left.flux_density);
}

}  // namespace

MonotoneBhCurve::MonotoneBhCurve(std::vector<BhPoint> points) : points_(std::move(points)) {
  ValidatePoints(points_);
}

const std::vector<BhPoint>& MonotoneBhCurve::points() const {
  return points_;
}

double MonotoneBhCurve::FieldIntensity(const double flux_density_magnitude) const {
  if (flux_density_magnitude < 0.0) {
    throw std::invalid_argument("B-H query requires a non-negative flux-density magnitude");
  }
  const int segment = SegmentIndex(flux_density_magnitude);
  return InterpolateOnSegment(points_.at(static_cast<std::size_t>(segment)),
                              points_.at(static_cast<std::size_t>(segment + 1)),
                              flux_density_magnitude);
}

double MonotoneBhCurve::DifferentialReluctivity(const double flux_density_magnitude) const {
  if (flux_density_magnitude < 0.0) {
    throw std::invalid_argument("B-H tangent query requires a non-negative flux-density magnitude");
  }
  const int segment = SegmentIndex(flux_density_magnitude);
  return SlopeOnSegment(points_.at(static_cast<std::size_t>(segment)),
                        points_.at(static_cast<std::size_t>(segment + 1)));
}

double MonotoneBhCurve::SecantReluctivity(const double flux_density_magnitude) const {
  if (flux_density_magnitude < 0.0) {
    throw std::invalid_argument("B-H secant query requires a non-negative flux-density magnitude");
  }
  if (flux_density_magnitude < kNearZero) {
    return DifferentialReluctivity(0.0);
  }
  return FieldIntensity(flux_density_magnitude) / flux_density_magnitude;
}

double MonotoneBhCurve::EnergyDensity(const double flux_density_magnitude) const {
  if (flux_density_magnitude < 0.0) {
    throw std::invalid_argument("B-H energy query requires a non-negative flux-density magnitude");
  }

  double energy = 0.0;
  for (std::size_t right_index = 1; right_index < points_.size(); ++right_index) {
    const BhPoint& left = points_[right_index - 1];
    const BhPoint& right = points_[right_index];
    const double segment_end = std::min(flux_density_magnitude, right.flux_density);
    if (segment_end <= left.flux_density) {
      break;
    }

    const double h_left = left.field_intensity;
    const double h_end = InterpolateOnSegment(left, right, segment_end);
    energy += 0.5 * (h_left + h_end) * (segment_end - left.flux_density);
    if (flux_density_magnitude <= right.flux_density) {
      return energy;
    }
  }

  const BhPoint& last = points_.back();
  const double h_end = FieldIntensity(flux_density_magnitude);
  energy += 0.5 * (last.field_intensity + h_end) *
            (flux_density_magnitude - last.flux_density);
  return energy;
}

bool MonotoneBhCurve::IsMonotone() const {
  for (std::size_t index = 1; index < points_.size(); ++index) {
    if (points_[index].flux_density <= points_[index - 1].flux_density ||
        points_[index].field_intensity < points_[index - 1].field_intensity) {
      return false;
    }
  }
  return true;
}

std::string MonotoneBhCurve::Summary() const {
  std::ostringstream stream;
  stream << "bh_points=" << points_.size() << ", b_max=" << points_.back().flux_density
         << ", h_max=" << points_.back().field_intensity;
  return stream.str();
}

int MonotoneBhCurve::SegmentIndex(const double flux_density_magnitude) const {
  if (flux_density_magnitude >= points_.back().flux_density) {
    return static_cast<int>(points_.size()) - 2;
  }
  const auto right = std::upper_bound(
      points_.begin(), points_.end(), flux_density_magnitude,
      [](const double value, const BhPoint& point) { return value < point.flux_density; });
  return std::max(0, static_cast<int>(std::distance(points_.begin(), right)) - 1);
}

OrthotropicBhMaterial::OrthotropicBhMaterial(MonotoneBhCurve x_axis,
                                             MonotoneBhCurve y_axis,
                                             MonotoneBhCurve z_axis)
    : axes_{{std::move(x_axis), std::move(y_axis), std::move(z_axis)}} {}

Vector3 OrthotropicBhMaterial::FieldIntensity(const Vector3& flux_density) const {
  Vector3 h{};
  for (std::size_t axis = 0; axis < axes_.size(); ++axis) {
    h[axis] = Sign(flux_density[axis]) *
              axes_[axis].FieldIntensity(AbsComponent(flux_density[axis]));
  }
  return h;
}

Matrix3 OrthotropicBhMaterial::ConsistentTangent(const Vector3& flux_density) const {
  Matrix3 tangent{};
  for (std::size_t axis = 0; axis < axes_.size(); ++axis) {
    tangent[axis][axis] =
        axes_[axis].DifferentialReluctivity(AbsComponent(flux_density[axis]));
  }
  return tangent;
}

double OrthotropicBhMaterial::EnergyDensity(const Vector3& flux_density) const {
  double energy = 0.0;
  for (std::size_t axis = 0; axis < axes_.size(); ++axis) {
    energy += axes_[axis].EnergyDensity(AbsComponent(flux_density[axis]));
  }
  return energy;
}

TangentCheckResult OrthotropicBhMaterial::CheckConsistentTangent(const Vector3& flux_density,
                                                                 const double step,
                                                                 const double tolerance) const {
  if (step <= 0.0) {
    throw std::invalid_argument("Consistent tangent check requires a positive step");
  }

  const Matrix3 tangent = ConsistentTangent(flux_density);
  double max_error = 0.0;
  for (std::size_t column = 0; column < axes_.size(); ++column) {
    Vector3 plus = flux_density;
    Vector3 minus = flux_density;
    plus[column] += step;
    minus[column] -= step;
    const Vector3 h_plus = FieldIntensity(plus);
    const Vector3 h_minus = FieldIntensity(minus);
    for (std::size_t row = 0; row < axes_.size(); ++row) {
      const double finite_difference = (h_plus[row] - h_minus[row]) / (2.0 * step);
      max_error = std::max(max_error, std::abs(finite_difference - tangent[row][column]));
    }
  }

  return {max_error <= tolerance, max_error, tolerance};
}

}  // namespace femsolver::material
