#pragma once

#include <array>
#include <functional>
#include <vector>

#include "femsolver/kernel/algebra/sparse_matrix.hpp"
#include "femsolver/kernel/assembly/element_geometry.hpp"
#include "femsolver/kernel/quadrature/tetrahedron_quadrature.hpp"

namespace femsolver::kernel::assembly {

using ScalarField = std::function<double(const mesh::Point3D&)>;

struct LocalPoissonElement {
  std::array<std::array<double, 4>, 4> stiffness{};
  std::array<double, 4> load{};
};

struct DirichletBoundaryCondition {
  std::vector<int> node_ids;
  ScalarField value;
};

class GlobalLinearSystem {
 public:
  explicit GlobalLinearSystem(int size);

  [[nodiscard]] int size() const;
  [[nodiscard]] algebra::SparseMatrixBuilder& matrix_builder();
  [[nodiscard]] const algebra::SparseMatrixBuilder& matrix_builder() const;
  [[nodiscard]] std::vector<double>& rhs();
  [[nodiscard]] const std::vector<double>& rhs() const;
  [[nodiscard]] algebra::LinearSystem Finalize() const;

 private:
  algebra::SparseMatrixBuilder matrix_builder_;
  std::vector<double> rhs_;
};

LocalPoissonElement AssemblePoissonElement(
    const ElementGeometry& geometry,
    const quadrature::TetrahedronQuadratureRule& quadrature_rule,
    const ScalarField& source);

GlobalLinearSystem AssemblePoissonSystem(const mesh::TetraMesh& tetra_mesh,
                                         const quadrature::TetrahedronQuadratureRule& quadrature_rule,
                                         const ScalarField& source);

void ApplyDirichletBoundary(const mesh::TetraMesh& tetra_mesh,
                            const DirichletBoundaryCondition& boundary_condition,
                            GlobalLinearSystem* system);

}  // namespace femsolver::kernel::assembly
