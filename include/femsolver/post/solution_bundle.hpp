#pragma once

#include <string>
#include <vector>

namespace femsolver::post {

struct SolutionBundle {
  std::string case_id;
  std::string status;
  double magnetic_energy;
  double torque_estimate;
  double average_flux_density_magnitude = 0.0;
  double max_flux_density_magnitude = 0.0;
  double torque_surface_area = 0.0;
  int torque_surface_face_count = 0;
  int primary_dof_count = 0;
  int linear_iterations = 0;
  double residual_norm = 0.0;
  bool linear_converged = false;
  std::vector<std::string> warnings;

  static SolutionBundle Bootstrap();
  [[nodiscard]] bool IsHealthy() const;
  [[nodiscard]] std::string Summary() const;
};

}  // namespace femsolver::post
