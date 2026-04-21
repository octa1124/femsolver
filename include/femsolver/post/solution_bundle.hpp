#pragma once

#include <string>
#include <vector>

namespace femsolver::post {

struct SolutionBundle {
  std::string status;
  double magnetic_energy;
  double torque_estimate;
  std::vector<std::string> warnings;

  static SolutionBundle Bootstrap();
  [[nodiscard]] std::string Summary() const;
};

}  // namespace femsolver::post
