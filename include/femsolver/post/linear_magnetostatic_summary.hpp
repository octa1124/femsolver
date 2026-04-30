#pragma once

#include <vector>

#include "femsolver/kernel/mesh/tetra_mesh.hpp"
#include "femsolver/kernel/space/tetra_dof_map.hpp"

namespace femsolver::post {

struct FluxDensitySummary {
  double average_magnitude = 0.0;
  double max_magnitude = 0.0;
};

struct MaxwellTorqueSummary {
  double torque_z = 0.0;
  double surface_area = 0.0;
  int face_count = 0;
};

FluxDensitySummary SummarizeFirstOrderNedelecFluxDensity(
    const kernel::mesh::TetraMesh& tetra_mesh,
    const kernel::space::TetraDofMap& dof_map,
    const std::vector<double>& edge_solution);

MaxwellTorqueSummary EstimateMaxwellStressTorqueZ(
    const kernel::mesh::TetraMesh& tetra_mesh,
    const kernel::space::TetraDofMap& dof_map,
    const std::vector<double>& edge_solution,
    int boundary_id,
    double reluctivity);

}  // namespace femsolver::post
