#pragma once

#include <array>
#include <functional>

#include "femsolver/kernel/assembly/curl_curl_assembler.hpp"

namespace femsolver::kernel::assembly {

struct LinearMagnetostaticCellData {
  double reluctivity = 1.0;
  Vector3 current_density{0.0, 0.0, 0.0};
  Vector3 remanent_flux_density{0.0, 0.0, 0.0};
};

using LinearMagnetostaticCellSelector =
    std::function<LinearMagnetostaticCellData(const mesh::TetraCell&, int)>;

struct LocalLinearMagnetostaticElement {
  std::array<std::array<double, 6>, 6> stiffness{};
  std::array<double, 6> load{};
};

LocalLinearMagnetostaticElement AssembleLinearMagnetostaticElement(
    const ElementGeometry& geometry,
    const quadrature::TetrahedronQuadratureRule& quadrature_rule,
    const LinearMagnetostaticCellData& cell_data);

GlobalLinearSystem AssembleLinearMagnetostaticSystem(
    const mesh::TetraMesh& tetra_mesh,
    const space::TetraDofMap& dof_map,
    const quadrature::TetrahedronQuadratureRule& quadrature_rule,
    const LinearMagnetostaticCellSelector& cell_selector);

}  // namespace femsolver::kernel::assembly
