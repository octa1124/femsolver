#pragma once

#include <string>

namespace femsolver::nonlinear {

struct NonlinearPolicy {
  bool enable_picard_warm_start;
  bool enable_damped_newton;
  int picard_steps;

  static NonlinearPolicy Bootstrap();
  [[nodiscard]] bool HasBootstrapPath() const;
  [[nodiscard]] std::string Summary() const;
};

}  // namespace femsolver::nonlinear
