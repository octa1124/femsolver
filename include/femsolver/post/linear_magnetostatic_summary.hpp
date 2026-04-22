#pragma once

#include <vector>

#include "femsolver/kernel/mesh/tetra_mesh.hpp"
#include "femsolver/kernel/space/tetra_dof_map.hpp"

namespace femsolver::post {

struct FluxDensitySummary {
  double average_magnitude = 0.0;
  double max_magnitude = 0.0;
};

FluxDensitySummary SummarizeFirstOrderNedelecFluxDensity(
    const kernel::mesh::TetraMesh& tetra_mesh,
    const kernel::space::TetraDofMap& dof_map,
    const std::vector<double>& edge_solution);

}  // namespace femsolver::post
