#include "femsolver/post/linear_magnetostatic_summary.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "femsolver/kernel/assembly/element_geometry.hpp"
#include "femsolver/kernel/basis/nedelec_p1_tetrahedron.hpp"

namespace femsolver::post {

namespace {

using Vector3 = kernel::assembly::Vector3;

double Dot(const Vector3& a, const Vector3& b) {
  return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

}  // namespace

FluxDensitySummary SummarizeFirstOrderNedelecFluxDensity(
    const kernel::mesh::TetraMesh& tetra_mesh,
    const kernel::space::TetraDofMap& dof_map,
    const std::vector<double>& edge_solution) {
  if (dof_map.entity_kind() != kernel::space::DofEntityKind::kEdge) {
    throw std::invalid_argument("Flux-density recovery requires an edge-based DoF map");
  }
  if (static_cast<int>(edge_solution.size()) != dof_map.global_dof_count()) {
    throw std::invalid_argument("Flux-density recovery requires one value per global edge DoF");
  }

  FluxDensitySummary summary;
  const auto reference_curls = kernel::basis::FirstOrderNedelecTetrahedron::ReferenceCurls();

  double total_volume = 0.0;
  double weighted_magnitude = 0.0;
  for (int cell_index = 0; cell_index < static_cast<int>(tetra_mesh.cells().size()); ++cell_index) {
    const auto geometry = kernel::assembly::BuildElementGeometry(tetra_mesh, cell_index);
    const auto& cell_dofs = dof_map.CellDofs(cell_index);

    Vector3 curl_a{0.0, 0.0, 0.0};
    for (std::size_t local_index = 0; local_index < cell_dofs.size(); ++local_index) {
      const auto& dof = cell_dofs[local_index];
      const auto physical_curl =
          geometry.MapReferenceCurlToPhysical(reference_curls[local_index]);
      const double coefficient =
          static_cast<double>(dof.orientation) * edge_solution.at(static_cast<std::size_t>(dof.index));
      curl_a[0] += coefficient * physical_curl[0];
      curl_a[1] += coefficient * physical_curl[1];
      curl_a[2] += coefficient * physical_curl[2];
    }

    const double magnitude = std::sqrt(Dot(curl_a, curl_a));
    summary.max_magnitude = std::max(summary.max_magnitude, magnitude);
    weighted_magnitude += magnitude * geometry.volume;
    total_volume += geometry.volume;
  }

  if (total_volume > 0.0) {
    summary.average_magnitude = weighted_magnitude / total_volume;
  }
  return summary;
}

}  // namespace femsolver::post
