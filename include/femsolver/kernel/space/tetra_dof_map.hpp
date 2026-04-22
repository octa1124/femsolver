#pragma once

#include <vector>

#include "femsolver/kernel/mesh/tetra_mesh.hpp"

namespace femsolver::kernel::space {

enum class DofEntityKind {
  kNode,
  kEdge,
};

struct SignedGlobalDof {
  int index = -1;
  int orientation = 1;
};

class TetraDofMap {
 public:
  TetraDofMap(DofEntityKind entity_kind,
              int global_dof_count,
              std::vector<std::vector<SignedGlobalDof>> cell_dofs,
              std::vector<int> boundary_dofs);

  [[nodiscard]] DofEntityKind entity_kind() const;
  [[nodiscard]] int global_dof_count() const;
  [[nodiscard]] const std::vector<SignedGlobalDof>& CellDofs(int cell_index) const;
  [[nodiscard]] const std::vector<int>& boundary_dofs() const;

 private:
  DofEntityKind entity_kind_ = DofEntityKind::kNode;
  int global_dof_count_ = 0;
  std::vector<std::vector<SignedGlobalDof>> cell_dofs_;
  std::vector<int> boundary_dofs_;
};

TetraDofMap BuildLagrangeP1DofMap(const mesh::TetraMesh& tetra_mesh);
TetraDofMap BuildFirstOrderNedelecDofMap(const mesh::TetraMesh& tetra_mesh);

}  // namespace femsolver::kernel::space
