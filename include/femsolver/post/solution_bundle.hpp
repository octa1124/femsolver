#pragma once

#include <string>
#include <vector>

namespace femsolver::post {

struct SolutionBundle {
  std::string case_id;
  std::string status;
  double magnetic_energy;
  double torque_estimate;
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
