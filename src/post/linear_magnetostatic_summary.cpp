#include "femsolver/post/linear_magnetostatic_summary.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <map>
#include <stdexcept>

#include "femsolver/kernel/assembly/element_geometry.hpp"
#include "femsolver/kernel/basis/nedelec_p1_tetrahedron.hpp"

namespace femsolver::post {

namespace {

using Vector3 = kernel::assembly::Vector3;

double Dot(const Vector3& a, const Vector3& b) {
  return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

Vector3 Cross(const Vector3& a, const Vector3& b) {
  return {
      a[1] * b[2] - a[2] * b[1],
      a[2] * b[0] - a[0] * b[2],
      a[0] * b[1] - a[1] * b[0],
  };
}

Vector3 Subtract(const kernel::mesh::Point3D& a, const kernel::mesh::Point3D& b) {
  return {a.x - b.x, a.y - b.y, a.z - b.z};
}

Vector3 Scale(const Vector3& vector, const double scale) {
  return {scale * vector[0], scale * vector[1], scale * vector[2]};
}

Vector3 Add(const Vector3& a, const Vector3& b) {
  return {a[0] + b[0], a[1] + b[1], a[2] + b[2]};
}

std::array<int, 3> SortedFaceNodes(std::array<int, 3> nodes) {
  std::sort(nodes.begin(), nodes.end());
  return nodes;
}

std::map<std::array<int, 3>, int> BuildFaceToCellMap(
    const kernel::mesh::TetraMesh& tetra_mesh) {
  std::map<std::array<int, 3>, int> face_to_cell;
  for (int cell_index = 0; cell_index < static_cast<int>(tetra_mesh.cells().size()); ++cell_index) {
    const auto& cell = tetra_mesh.cells().at(static_cast<std::size_t>(cell_index));
    for (const auto face : {std::array<int, 3>{cell.node_ids[0], cell.node_ids[1], cell.node_ids[2]},
                            std::array<int, 3>{cell.node_ids[0], cell.node_ids[1], cell.node_ids[3]},
                            std::array<int, 3>{cell.node_ids[0], cell.node_ids[2], cell.node_ids[3]},
                            std::array<int, 3>{cell.node_ids[1], cell.node_ids[2], cell.node_ids[3]}}) {
      face_to_cell.emplace(SortedFaceNodes(face), cell_index);
    }
  }
  return face_to_cell;
}

Vector3 RecoverCellCurlA(const kernel::mesh::TetraMesh& tetra_mesh,
                         const kernel::space::TetraDofMap& dof_map,
                         const std::vector<double>& edge_solution,
                         const int cell_index) {
  const auto geometry = kernel::assembly::BuildElementGeometry(tetra_mesh, cell_index);
  const auto reference_curls = kernel::basis::FirstOrderNedelecTetrahedron::ReferenceCurls();
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
  return curl_a;
}

void ValidateEdgeSolution(const kernel::space::TetraDofMap& dof_map,
                          const std::vector<double>& edge_solution,
                          const std::string& context) {
  if (dof_map.entity_kind() != kernel::space::DofEntityKind::kEdge) {
    throw std::invalid_argument(context + " requires an edge-based DoF map");
  }
  if (static_cast<int>(edge_solution.size()) != dof_map.global_dof_count()) {
    throw std::invalid_argument(context + " requires one value per global edge DoF");
  }
}

}  // namespace

FluxDensitySummary SummarizeFirstOrderNedelecFluxDensity(
    const kernel::mesh::TetraMesh& tetra_mesh,
    const kernel::space::TetraDofMap& dof_map,
    const std::vector<double>& edge_solution) {
  ValidateEdgeSolution(dof_map, edge_solution, "Flux-density recovery");

  FluxDensitySummary summary;

  double total_volume = 0.0;
  double weighted_magnitude = 0.0;
  for (int cell_index = 0; cell_index < static_cast<int>(tetra_mesh.cells().size()); ++cell_index) {
    const auto geometry = kernel::assembly::BuildElementGeometry(tetra_mesh, cell_index);
    const Vector3 curl_a = RecoverCellCurlA(tetra_mesh, dof_map, edge_solution, cell_index);

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

MaxwellTorqueSummary EstimateMaxwellStressTorqueZ(
    const kernel::mesh::TetraMesh& tetra_mesh,
    const kernel::space::TetraDofMap& dof_map,
    const std::vector<double>& edge_solution,
    const int boundary_id,
    const double reluctivity) {
  ValidateEdgeSolution(dof_map, edge_solution, "Maxwell-stress torque estimation");
  if (boundary_id <= 0) {
    return {};
  }

  MaxwellTorqueSummary summary;
  const auto face_to_cell = BuildFaceToCellMap(tetra_mesh);
  for (const auto& face : tetra_mesh.boundary_faces()) {
    if (face.boundary_id != boundary_id) {
      continue;
    }

    const auto face_cell_iterator = face_to_cell.find(SortedFaceNodes(face.node_ids));
    if (face_cell_iterator == face_to_cell.end()) {
      continue;
    }

    const auto& a = tetra_mesh.nodes().at(static_cast<std::size_t>(face.node_ids[0]));
    const auto& b = tetra_mesh.nodes().at(static_cast<std::size_t>(face.node_ids[1]));
    const auto& c = tetra_mesh.nodes().at(static_cast<std::size_t>(face.node_ids[2]));
    const Vector3 area_vector = Scale(Cross(Subtract(b, a), Subtract(c, a)), 0.5);
    const double area = std::sqrt(Dot(area_vector, area_vector));
    if (area <= 0.0) {
      continue;
    }

    const Vector3 magnetic_flux_density =
        RecoverCellCurlA(tetra_mesh, dof_map, edge_solution, face_cell_iterator->second);
    const double b_dot_area = Dot(magnetic_flux_density, area_vector);
    const double b_squared = Dot(magnetic_flux_density, magnetic_flux_density);
    const Vector3 traction_integral =
        Scale(Add(Scale(magnetic_flux_density, b_dot_area),
                  Scale(area_vector, -0.5 * b_squared)),
              reluctivity);

    const kernel::mesh::Point3D center{
        (a.x + b.x + c.x) / 3.0,
        (a.y + b.y + c.y) / 3.0,
        (a.z + b.z + c.z) / 3.0,
    };
    summary.torque_z += center.x * traction_integral[1] - center.y * traction_integral[0];
    summary.surface_area += area;
    ++summary.face_count;
  }
  return summary;
}

}  // namespace femsolver::post
