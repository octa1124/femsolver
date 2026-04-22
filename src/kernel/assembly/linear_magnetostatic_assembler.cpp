#include "femsolver/kernel/assembly/linear_magnetostatic_assembler.hpp"

#include <stdexcept>

#include "femsolver/kernel/basis/nedelec_p1_tetrahedron.hpp"

namespace femsolver::kernel::assembly {

namespace {

double Dot(const Vector3& a, const Vector3& b) {
  return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

}  // namespace

LocalLinearMagnetostaticElement AssembleLinearMagnetostaticElement(
    const ElementGeometry& geometry,
    const quadrature::TetrahedronQuadratureRule& quadrature_rule,
    const LinearMagnetostaticCellData& cell_data) {
  LocalLinearMagnetostaticElement element;
  const auto reference_curls = basis::FirstOrderNedelecTetrahedron::ReferenceCurls();

  for (const auto& quadrature_point : quadrature_rule.points()) {
    const auto reference_values =
        basis::FirstOrderNedelecTetrahedron::Evaluate(quadrature_point.reference_point);
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
      element.load[row] +=
          (Dot(cell_data.current_density, physical_values[row]) +
           cell_data.reluctivity *
               Dot(cell_data.remanent_flux_density, physical_curls[row])) *
          weight;
      for (std::size_t col = 0; col < element.stiffness[row].size(); ++col) {
        element.stiffness[row][col] +=
            cell_data.reluctivity * Dot(physical_curls[row], physical_curls[col]) * weight;
      }
    }
  }

  return element;
}

GlobalLinearSystem AssembleLinearMagnetostaticSystem(
    const mesh::TetraMesh& tetra_mesh,
    const space::TetraDofMap& dof_map,
    const quadrature::TetrahedronQuadratureRule& quadrature_rule,
    const LinearMagnetostaticCellSelector& cell_selector) {
  if (dof_map.entity_kind() != space::DofEntityKind::kEdge) {
    throw std::invalid_argument("Linear magnetostatics requires an edge-based DoF map");
  }

  GlobalLinearSystem system(dof_map.global_dof_count());
  for (int cell_index = 0; cell_index < static_cast<int>(tetra_mesh.cells().size()); ++cell_index) {
    const auto& cell = tetra_mesh.cells().at(static_cast<std::size_t>(cell_index));
    const auto cell_data = cell_selector ? cell_selector(cell, cell_index)
                                         : LinearMagnetostaticCellData{};
    const ElementGeometry geometry = BuildElementGeometry(tetra_mesh, cell_index);
    const auto element =
        AssembleLinearMagnetostaticElement(geometry, quadrature_rule, cell_data);
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

}  // namespace femsolver::kernel::assembly
