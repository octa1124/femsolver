#include "femsolver/kernel/assembly/curl_curl_assembler.hpp"

#include <stdexcept>

#include "femsolver/kernel/basis/nedelec_p1_tetrahedron.hpp"

namespace femsolver::kernel::assembly {

namespace {

double Dot(const Vector3& a, const Vector3& b) {
  return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

}  // namespace

LocalCurlCurlElement AssembleCurlCurlElement(
    const ElementGeometry& geometry,
    const quadrature::TetrahedronQuadratureRule& quadrature_rule,
    const VectorField& source) {
  LocalCurlCurlElement element;
  const auto reference_curls = basis::FirstOrderNedelecTetrahedron::ReferenceCurls();

  for (const auto& quadrature_point : quadrature_rule.points()) {
    const auto reference_values =
        basis::FirstOrderNedelecTetrahedron::Evaluate(quadrature_point.reference_point);
    const auto physical_point = geometry.MapToPhysical(quadrature_point.reference_point);
    const Vector3 source_value = source ? source(physical_point) : Vector3{0.0, 0.0, 0.0};
    const double weight = quadrature_point.weight * geometry.determinant;

    std::array<Vector3, 6> physical_values{};
    std::array<Vector3, 6> physical_curls{};
    for (std::size_t basis_index = 0; basis_index < physical_values.size(); ++basis_index) {
      physical_values[basis_index] =
          geometry.MapReferenceHcurlVectorToPhysical(reference_values[basis_index]);
      physical_curls[basis_index] =
          geometry.MapReferenceCurlToPhysical(reference_curls[basis_index]);
    }

    for (std::size_t row = 0; row < element.load.size(); ++row) {
      element.load[row] += Dot(source_value, physical_values[row]) * weight;
      for (std::size_t col = 0; col < element.stiffness[row].size(); ++col) {
        element.stiffness[row][col] += Dot(physical_curls[row], physical_curls[col]) * weight;
      }
    }
  }

  return element;
}

GlobalLinearSystem AssembleCurlCurlSystem(const mesh::TetraMesh& tetra_mesh,
                                          const space::TetraDofMap& dof_map,
                                          const quadrature::TetrahedronQuadratureRule& quadrature_rule,
                                          const VectorField& source) {
  if (dof_map.entity_kind() != space::DofEntityKind::kEdge) {
    throw std::invalid_argument("Curl-curl assembly requires an edge-based DoF map");
  }

  GlobalLinearSystem system(dof_map.global_dof_count());
  for (int cell_index = 0; cell_index < static_cast<int>(tetra_mesh.cells().size()); ++cell_index) {
    const ElementGeometry geometry = BuildElementGeometry(tetra_mesh, cell_index);
    const LocalCurlCurlElement element = AssembleCurlCurlElement(geometry, quadrature_rule, source);
    const auto& cell_dofs = dof_map.CellDofs(cell_index);

    for (std::size_t local_row = 0; local_row < cell_dofs.size(); ++local_row) {
      const auto& row_dof = cell_dofs[local_row];
      system.rhs().at(static_cast<std::size_t>(row_dof.index)) +=
          static_cast<double>(row_dof.orientation) * element.load[local_row];

      for (std::size_t local_col = 0; local_col < cell_dofs.size(); ++local_col) {
        const auto& col_dof = cell_dofs[local_col];
        system.matrix_builder().Add(
            row_dof.index, col_dof.index,
            static_cast<double>(row_dof.orientation * col_dof.orientation) *
                element.stiffness[local_row][local_col]);
      }
    }
  }

  return system;
}

void ApplyEdgeDirichletBoundary(const EdgeDirichletBoundaryCondition& boundary_condition,
                                GlobalLinearSystem* system) {
  if (system == nullptr) {
    throw std::invalid_argument("Edge Dirichlet application requires a valid system pointer");
  }
  if (boundary_condition.dof_ids.size() != boundary_condition.values.size()) {
    throw std::invalid_argument("Edge Dirichlet dof/value arrays must have the same size");
  }

  std::vector<bool> is_dirichlet(static_cast<std::size_t>(system->size()), false);
  std::vector<double> prescribed_values(static_cast<std::size_t>(system->size()), 0.0);
  for (std::size_t index = 0; index < boundary_condition.dof_ids.size(); ++index) {
    const int dof_id = boundary_condition.dof_ids[index];
    if (dof_id < 0 || dof_id >= system->size()) {
      throw std::out_of_range("Edge Dirichlet dof id out of range");
    }
    is_dirichlet[static_cast<std::size_t>(dof_id)] = true;
    prescribed_values[static_cast<std::size_t>(dof_id)] = boundary_condition.values[index];
  }

  for (int column = 0; column < system->size(); ++column) {
    if (!is_dirichlet[static_cast<std::size_t>(column)]) {
      continue;
    }
    const double prescribed_value = prescribed_values[static_cast<std::size_t>(column)];
    for (int row = 0; row < system->size(); ++row) {
      if (row == column) {
        continue;
      }
      if (system->matrix_builder().HasEntry(row, column)) {
        const double coupling = system->matrix_builder().Entry(row, column);
        system->rhs()[static_cast<std::size_t>(row)] -= coupling * prescribed_value;
        system->matrix_builder().Erase(row, column);
      }
    }
  }

  for (int dof_id = 0; dof_id < system->size(); ++dof_id) {
    if (!is_dirichlet[static_cast<std::size_t>(dof_id)]) {
      continue;
    }
    system->matrix_builder().ClearRow(dof_id);
    system->matrix_builder().Set(dof_id, dof_id, 1.0);
    system->rhs()[static_cast<std::size_t>(dof_id)] =
        prescribed_values[static_cast<std::size_t>(dof_id)];
  }
}

}  // namespace femsolver::kernel::assembly
