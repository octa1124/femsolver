#include "femsolver/kernel/basis/nedelec_p1_tetrahedron.hpp"

#include "femsolver/kernel/basis/lagrange_p1_tetrahedron.hpp"

namespace femsolver::kernel::basis {

namespace {

Vector3 Cross(const Vector3& a, const Vector3& b) {
  return {
      a[1] * b[2] - a[2] * b[1],
      a[2] * b[0] - a[0] * b[2],
      a[0] * b[1] - a[1] * b[0],
  };
}

}  // namespace

const std::array<std::array<int, 2>, 6>& FirstOrderNedelecTetrahedron::LocalEdges() {
  static const std::array<std::array<int, 2>, 6> kLocalEdges = {{
      {{0, 1}},
      {{0, 2}},
      {{0, 3}},
      {{1, 2}},
      {{1, 3}},
      {{2, 3}},
  }};
  return kLocalEdges;
}

std::array<Vector3, 6> FirstOrderNedelecTetrahedron::Evaluate(const Vector3& reference_point) {
  const auto lambdas = LagrangeP1Tetrahedron::Evaluate(reference_point);
  const auto gradients = LagrangeP1Tetrahedron::ReferenceGradients();

  std::array<Vector3, 6> values{};
  const auto& edges = LocalEdges();
  for (std::size_t edge_index = 0; edge_index < edges.size(); ++edge_index) {
    const std::size_t i = static_cast<std::size_t>(edges[edge_index][0]);
    const std::size_t j = static_cast<std::size_t>(edges[edge_index][1]);
    values[edge_index] = {
        lambdas[i] * gradients[j][0] - lambdas[j] * gradients[i][0],
        lambdas[i] * gradients[j][1] - lambdas[j] * gradients[i][1],
        lambdas[i] * gradients[j][2] - lambdas[j] * gradients[i][2],
    };
  }
  return values;
}

std::array<Vector3, 6> FirstOrderNedelecTetrahedron::ReferenceCurls() {
  const auto gradients = LagrangeP1Tetrahedron::ReferenceGradients();

  std::array<Vector3, 6> curls{};
  const auto& edges = LocalEdges();
  for (std::size_t edge_index = 0; edge_index < edges.size(); ++edge_index) {
    const std::size_t i = static_cast<std::size_t>(edges[edge_index][0]);
    const std::size_t j = static_cast<std::size_t>(edges[edge_index][1]);
    const Vector3 cross_product = Cross(gradients[i], gradients[j]);
    curls[edge_index] = {2.0 * cross_product[0], 2.0 * cross_product[1], 2.0 * cross_product[2]};
  }
  return curls;
}

}  // namespace femsolver::kernel::basis
