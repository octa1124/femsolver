#include "femsolver/kernel/basis/lagrange_hex_hexahedron.hpp"

#include <cstddef>

namespace femsolver::kernel::basis {

namespace {

std::array<double, 3> QuadraticValues(const double x) {
  return {0.5 * x * (x - 1.0), 1.0 - x * x, 0.5 * x * (x + 1.0)};
}

std::array<double, 3> QuadraticGradients(const double x) {
  return {x - 0.5, -2.0 * x, x + 0.5};
}

int TensorIndex(const int i, const int j, const int k) {
  return i + 3 * j + 9 * k;
}

}  // namespace

const std::array<HexVector3, 8>& LagrangeQ1Hexahedron::Nodes() {
  static const std::array<HexVector3, 8> kNodes = {{
      {-1.0, -1.0, -1.0},
      {1.0, -1.0, -1.0},
      {1.0, 1.0, -1.0},
      {-1.0, 1.0, -1.0},
      {-1.0, -1.0, 1.0},
      {1.0, -1.0, 1.0},
      {1.0, 1.0, 1.0},
      {-1.0, 1.0, 1.0},
  }};
  return kNodes;
}

std::array<double, 8> LagrangeQ1Hexahedron::Evaluate(const HexVector3& reference_point) {
  std::array<double, 8> values{};
  const auto& nodes = Nodes();
  for (std::size_t node = 0; node < nodes.size(); ++node) {
    values[node] = 0.125 * (1.0 + nodes[node][0] * reference_point[0]) *
                   (1.0 + nodes[node][1] * reference_point[1]) *
                   (1.0 + nodes[node][2] * reference_point[2]);
  }
  return values;
}

std::array<HexVector3, 8> LagrangeQ1Hexahedron::ReferenceGradients(
    const HexVector3& reference_point) {
  std::array<HexVector3, 8> gradients{};
  const auto& nodes = Nodes();
  for (std::size_t node = 0; node < nodes.size(); ++node) {
    gradients[node] = {
        0.125 * nodes[node][0] * (1.0 + nodes[node][1] * reference_point[1]) *
            (1.0 + nodes[node][2] * reference_point[2]),
        0.125 * (1.0 + nodes[node][0] * reference_point[0]) * nodes[node][1] *
            (1.0 + nodes[node][2] * reference_point[2]),
        0.125 * (1.0 + nodes[node][0] * reference_point[0]) *
            (1.0 + nodes[node][1] * reference_point[1]) * nodes[node][2],
    };
  }
  return gradients;
}

const std::array<HexVector3, 27>& LagrangeQ2Hexahedron::Nodes() {
  static const std::array<HexVector3, 27> kNodes = [] {
    std::array<HexVector3, 27> nodes{};
    const std::array<double, 3> coordinates = {-1.0, 0.0, 1.0};
    for (int k = 0; k < 3; ++k) {
      for (int j = 0; j < 3; ++j) {
        for (int i = 0; i < 3; ++i) {
          nodes[static_cast<std::size_t>(TensorIndex(i, j, k))] = {
              coordinates[static_cast<std::size_t>(i)],
              coordinates[static_cast<std::size_t>(j)],
              coordinates[static_cast<std::size_t>(k)]};
        }
      }
    }
    return nodes;
  }();
  return kNodes;
}

std::array<double, 27> LagrangeQ2Hexahedron::Evaluate(const HexVector3& reference_point) {
  const auto lx = QuadraticValues(reference_point[0]);
  const auto ly = QuadraticValues(reference_point[1]);
  const auto lz = QuadraticValues(reference_point[2]);
  std::array<double, 27> values{};
  for (int k = 0; k < 3; ++k) {
    for (int j = 0; j < 3; ++j) {
      for (int i = 0; i < 3; ++i) {
        values[static_cast<std::size_t>(TensorIndex(i, j, k))] =
            lx[static_cast<std::size_t>(i)] * ly[static_cast<std::size_t>(j)] *
            lz[static_cast<std::size_t>(k)];
      }
    }
  }
  return values;
}

std::array<HexVector3, 27> LagrangeQ2Hexahedron::ReferenceGradients(
    const HexVector3& reference_point) {
  const auto lx = QuadraticValues(reference_point[0]);
  const auto ly = QuadraticValues(reference_point[1]);
  const auto lz = QuadraticValues(reference_point[2]);
  const auto dlx = QuadraticGradients(reference_point[0]);
  const auto dly = QuadraticGradients(reference_point[1]);
  const auto dlz = QuadraticGradients(reference_point[2]);
  std::array<HexVector3, 27> gradients{};
  for (int k = 0; k < 3; ++k) {
    for (int j = 0; j < 3; ++j) {
      for (int i = 0; i < 3; ++i) {
        gradients[static_cast<std::size_t>(TensorIndex(i, j, k))] = {
            dlx[static_cast<std::size_t>(i)] * ly[static_cast<std::size_t>(j)] *
                lz[static_cast<std::size_t>(k)],
            lx[static_cast<std::size_t>(i)] * dly[static_cast<std::size_t>(j)] *
                lz[static_cast<std::size_t>(k)],
            lx[static_cast<std::size_t>(i)] * ly[static_cast<std::size_t>(j)] *
                dlz[static_cast<std::size_t>(k)],
        };
      }
    }
  }
  return gradients;
}

}  // namespace femsolver::kernel::basis
