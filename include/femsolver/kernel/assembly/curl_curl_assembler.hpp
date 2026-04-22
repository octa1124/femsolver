#pragma once

#include <array>
#include <functional>
#include <vector>

#include "femsolver/kernel/assembly/element_geometry.hpp"
#include "femsolver/kernel/assembly/poisson_assembler.hpp"
#include "femsolver/kernel/quadrature/tetrahedron_quadrature.hpp"
#include "femsolver/kernel/space/tetra_dof_map.hpp"

namespace femsolver::kernel::assembly {

using VectorField = std::function<Vector3(const mesh::Point3D&)>;

struct LocalCurlCurlElement {
  std::array<std::array<double, 6>, 6> stiffness{};
  std::array<double, 6> load{};
};

struct EdgeDirichletBoundaryCondition {
  std::vector<int> dof_ids;
  std::vector<double> values;
};

LocalCurlCurlElement AssembleCurlCurlElement(
    const ElementGeometry& geometry,
    const quadrature::TetrahedronQuadratureRule& quadrature_rule,
    const VectorField& source);

GlobalLinearSystem AssembleCurlCurlSystem(const mesh::TetraMesh& tetra_mesh,
                                          const space::TetraDofMap& dof_map,
                                          const quadrature::TetrahedronQuadratureRule& quadrature_rule,
                                          const VectorField& source);

void ApplyEdgeDirichletBoundary(const EdgeDirichletBoundaryCondition& boundary_condition,
                                GlobalLinearSystem* system);

}  // namespace femsolver::kernel::assembly
