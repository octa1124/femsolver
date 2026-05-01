#include "femsolver/kernel/basis/raviart_thomas_p0_tetrahedron.hpp"

#include <cmath>
#include <stdexcept>
#include <string>

namespace femsolver::kernel::basis {

namespace {

constexpr double kReferenceVolume = 1.0 / 6.0;
constexpr double kScale = 1.0 / (3.0 * kReferenceVolume);

struct FaceMeasure {
  Vector3 centroid;
  Vector3 outward_unit_normal;
  double area = 0.0;
};

const std::array<FaceMeasure, 4>& FaceMeasures() {
  static const double kSqrt3 = std::sqrt(3.0);
  static const std::array<FaceMeasure, 4> kMeasures = {{
      {{1.0 / 3.0, 1.0 / 3.0, 1.0 / 3.0}, {1.0 / kSqrt3, 1.0 / kSqrt3, 1.0 / kSqrt3},
       kSqrt3 / 2.0},
      {{0.0, 1.0 / 3.0, 1.0 / 3.0}, {-1.0, 0.0, 0.0}, 0.5},
      {{1.0 / 3.0, 0.0, 1.0 / 3.0}, {0.0, -1.0, 0.0}, 0.5},
      {{1.0 / 3.0, 1.0 / 3.0, 0.0}, {0.0, 0.0, -1.0}, 0.5},
  }};
  return kMeasures;
}

double Dot(const Vector3& a, const Vector3& b) {
  return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

void ValidateIndex(const int index, const char* label) {
  if (index < 0 || index >= 4) {
    throw std::out_of_range(std::string(label) + " index out of range");
  }
}

}  // namespace

const std::array<Vector3, 4>& RaviartThomasP0Tetrahedron::Vertices() {
  static const std::array<Vector3, 4> kVertices = {{
      {0.0, 0.0, 0.0},
      {1.0, 0.0, 0.0},
      {0.0, 1.0, 0.0},
      {0.0, 0.0, 1.0},
  }};
  return kVertices;
}

const std::array<std::array<int, 3>, 4>& RaviartThomasP0Tetrahedron::LocalFaces() {
  static const std::array<std::array<int, 3>, 4> kFaces = {{
      {{1, 2, 3}},
      {{0, 3, 2}},
      {{0, 1, 3}},
      {{0, 2, 1}},
  }};
  return kFaces;
}

std::array<Vector3, 4> RaviartThomasP0Tetrahedron::Evaluate(const Vector3& reference_point) {
  const auto& vertices = Vertices();
  std::array<Vector3, 4> values{};
  for (std::size_t basis_index = 0; basis_index < values.size(); ++basis_index) {
    values[basis_index] = {
        kScale * (reference_point[0] - vertices[basis_index][0]),
        kScale * (reference_point[1] - vertices[basis_index][1]),
        kScale * (reference_point[2] - vertices[basis_index][2]),
    };
  }
  return values;
}

std::array<double, 4> RaviartThomasP0Tetrahedron::ReferenceDivergences() {
  return {3.0 * kScale, 3.0 * kScale, 3.0 * kScale, 3.0 * kScale};
}

double RaviartThomasP0Tetrahedron::ReferenceFaceFlux(const int basis_index,
                                                     const int face_index) {
  ValidateIndex(basis_index, "RT basis");
  ValidateIndex(face_index, "face");
  const auto values = Evaluate(FaceMeasures().at(static_cast<std::size_t>(face_index)).centroid);
  const auto& measure = FaceMeasures().at(static_cast<std::size_t>(face_index));
  return Dot(values.at(static_cast<std::size_t>(basis_index)), measure.outward_unit_normal) *
         measure.area;
}

std::array<std::array<double, 4>, 4> RaviartThomasP0Tetrahedron::ReferenceFaceFluxMatrix() {
  std::array<std::array<double, 4>, 4> matrix{};
  for (int basis_index = 0; basis_index < 4; ++basis_index) {
    for (int face_index = 0; face_index < 4; ++face_index) {
      matrix[static_cast<std::size_t>(basis_index)][static_cast<std::size_t>(face_index)] =
          ReferenceFaceFlux(basis_index, face_index);
    }
  }
  return matrix;
}

}  // namespace femsolver::kernel::basis
