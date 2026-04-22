#include "femsolver/kernel/assembly/poisson_assembler.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "femsolver/kernel/basis/lagrange_p1_tetrahedron.hpp"

namespace femsolver::kernel::assembly {

namespace {

double Dot(const Vector3& a, const Vector3& b) {
  return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

}  // namespace

GlobalLinearSystem::GlobalLinearSystem(const int size)
    : matrix_builder_(size, size), rhs_(static_cast<std::size_t>(size), 0.0) {}

int GlobalLinearSystem::size() const {
  return static_cast<int>(rhs_.size());
}

algebra::SparseMatrixBuilder& GlobalLinearSystem::matrix_builder() {
  return matrix_builder_;
}

const algebra::SparseMatrixBuilder& GlobalLinearSystem::matrix_builder() const {
  return matrix_builder_;
}

std::vector<double>& GlobalLinearSystem::rhs() {
  return rhs_;
}

const std::vector<double>& GlobalLinearSystem::rhs() const {
  return rhs_;
}

algebra::LinearSystem GlobalLinearSystem::Finalize() const {
  return {matrix_builder_.Build(), rhs_};
}

LocalPoissonElement AssemblePoissonElement(
    const ElementGeometry& geometry,
    const quadrature::TetrahedronQuadratureRule& quadrature_rule,
    const ScalarField& source) {
  LocalPoissonElement element;

  for (const auto& quadrature_point : quadrature_rule.points()) {
    const auto shape_values = basis::LagrangeP1Tetrahedron::Evaluate(quadrature_point.reference_point);
    const auto physical_point = geometry.MapToPhysical(quadrature_point.reference_point);
    const double source_value = source ? source(physical_point) : 0.0;
    const double weight = quadrature_point.weight * geometry.determinant;

    for (std::size_t i = 0; i < element.load.size(); ++i) {
      element.load[i] += source_value * shape_values[i] * weight;
      for (std::size_t j = 0; j < element.stiffness[i].size(); ++j) {
        element.stiffness[i][j] +=
            Dot(geometry.physical_gradients[i], geometry.physical_gradients[j]) * weight;
      }
    }
  }

  return element;
}

GlobalLinearSystem AssemblePoissonSystem(
    const mesh::TetraMesh& tetra_mesh,
    const quadrature::TetrahedronQuadratureRule& quadrature_rule,
    const ScalarField& source) {
  GlobalLinearSystem system(static_cast<int>(tetra_mesh.nodes().size()));

  for (int cell_index = 0; cell_index < static_cast<int>(tetra_mesh.cells().size()); ++cell_index) {
    const ElementGeometry geometry = BuildElementGeometry(tetra_mesh, cell_index);
    const LocalPoissonElement element = AssemblePoissonElement(geometry, quadrature_rule, source);
    const auto& cell = tetra_mesh.cells().at(static_cast<std::size_t>(cell_index));

    for (std::size_t local_row = 0; local_row < element.load.size(); ++local_row) {
      const int global_row = cell.node_ids[local_row];
      system.rhs().at(static_cast<std::size_t>(global_row)) += element.load[local_row];

      for (std::size_t local_col = 0; local_col < element.stiffness[local_row].size();
           ++local_col) {
        const int global_col = cell.node_ids[local_col];
        system.matrix_builder().Add(global_row, global_col, element.stiffness[local_row][local_col]);
      }
    }
  }

  return system;
}

void ApplyDirichletBoundary(const mesh::TetraMesh& tetra_mesh,
                            const DirichletBoundaryCondition& boundary_condition,
                            GlobalLinearSystem* system) {
  if (system == nullptr) {
    throw std::invalid_argument("Dirichlet application requires a valid system pointer");
  }

  std::vector<bool> is_dirichlet(static_cast<std::size_t>(system->size()), false);
  std::vector<double> boundary_values(static_cast<std::size_t>(system->size()), 0.0);

  for (const int node_id : boundary_condition.node_ids) {
    if (node_id < 0 || node_id >= system->size()) {
      throw std::out_of_range("Dirichlet node id out of range");
    }

    is_dirichlet.at(static_cast<std::size_t>(node_id)) = true;
    const auto& point = tetra_mesh.nodes().at(static_cast<std::size_t>(node_id));
    boundary_values.at(static_cast<std::size_t>(node_id)) =
        boundary_condition.value ? boundary_condition.value(point) : 0.0;
  }

  for (int column = 0; column < system->size(); ++column) {
    if (!is_dirichlet.at(static_cast<std::size_t>(column))) {
      continue;
    }

    const double prescribed_value = boundary_values.at(static_cast<std::size_t>(column));
    for (int row = 0; row < system->size(); ++row) {
      if (row == column) {
        continue;
      }

      if (system->matrix_builder().HasEntry(row, column)) {
        const double coupling = system->matrix_builder().Entry(row, column);
        system->rhs().at(static_cast<std::size_t>(row)) -= coupling * prescribed_value;
        system->matrix_builder().Erase(row, column);
      }
    }
  }

  for (int node_id = 0; node_id < system->size(); ++node_id) {
    if (!is_dirichlet.at(static_cast<std::size_t>(node_id))) {
      continue;
    }

    system->matrix_builder().ClearRow(node_id);
    system->matrix_builder().Set(node_id, node_id, 1.0);
    system->rhs().at(static_cast<std::size_t>(node_id)) =
        boundary_values.at(static_cast<std::size_t>(node_id));
  }
}

}  // namespace femsolver::kernel::assembly
