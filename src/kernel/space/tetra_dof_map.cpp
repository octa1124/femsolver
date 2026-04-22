#include "femsolver/kernel/space/tetra_dof_map.hpp"

#include <map>
#include <stdexcept>

#include "femsolver/kernel/basis/nedelec_p1_tetrahedron.hpp"

namespace femsolver::kernel::space {

TetraDofMap::TetraDofMap(DofEntityKind entity_kind,
                         const int global_dof_count,
                         std::vector<std::vector<SignedGlobalDof>> cell_dofs,
                         std::vector<int> boundary_dofs)
    : entity_kind_(entity_kind),
      global_dof_count_(global_dof_count),
      cell_dofs_(std::move(cell_dofs)),
      boundary_dofs_(std::move(boundary_dofs)) {}

DofEntityKind TetraDofMap::entity_kind() const {
  return entity_kind_;
}

int TetraDofMap::global_dof_count() const {
  return global_dof_count_;
}

const std::vector<SignedGlobalDof>& TetraDofMap::CellDofs(const int cell_index) const {
  return cell_dofs_.at(static_cast<std::size_t>(cell_index));
}

const std::vector<int>& TetraDofMap::boundary_dofs() const {
  return boundary_dofs_;
}

TetraDofMap BuildLagrangeP1DofMap(const mesh::TetraMesh& tetra_mesh) {
  std::vector<std::vector<SignedGlobalDof>> cell_dofs;
  for (const auto& cell : tetra_mesh.cells()) {
    cell_dofs.push_back({
        {cell.node_ids[0], 1},
        {cell.node_ids[1], 1},
        {cell.node_ids[2], 1},
        {cell.node_ids[3], 1},
    });
  }

  return TetraDofMap(DofEntityKind::kNode, static_cast<int>(tetra_mesh.nodes().size()),
                     std::move(cell_dofs), tetra_mesh.BoundaryNodeIds());
}

TetraDofMap BuildFirstOrderNedelecDofMap(const mesh::TetraMesh& tetra_mesh) {
  std::map<std::array<int, 2>, int> edge_lookup;
  for (std::size_t edge_index = 0; edge_index < tetra_mesh.edges().size(); ++edge_index) {
    edge_lookup[tetra_mesh.edges().at(edge_index).node_ids] = static_cast<int>(edge_index);
  }

  std::vector<std::vector<SignedGlobalDof>> cell_dofs;
  const auto& local_edges = basis::FirstOrderNedelecTetrahedron::LocalEdges();
  for (const auto& cell : tetra_mesh.cells()) {
    std::vector<SignedGlobalDof> local_dofs;
    local_dofs.reserve(local_edges.size());
    for (const auto& local_edge : local_edges) {
      const int start_node = cell.node_ids[static_cast<std::size_t>(local_edge[0])];
      const int end_node = cell.node_ids[static_cast<std::size_t>(local_edge[1])];
      const std::array<int, 2> canonical_edge =
          start_node < end_node ? std::array<int, 2>{start_node, end_node}
                                : std::array<int, 2>{end_node, start_node};
      const int orientation = start_node < end_node ? 1 : -1;
      local_dofs.push_back({edge_lookup.at(canonical_edge), orientation});
    }
    cell_dofs.push_back(std::move(local_dofs));
  }

  return TetraDofMap(DofEntityKind::kEdge, static_cast<int>(tetra_mesh.edges().size()),
                     std::move(cell_dofs), tetra_mesh.BoundaryEdgeIds());
}

}  // namespace femsolver::kernel::space
