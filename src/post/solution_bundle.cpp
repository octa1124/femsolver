#include "femsolver/post/solution_bundle.hpp"

#include <sstream>

namespace femsolver::post {

SolutionBundle SolutionBundle::Bootstrap() {
  return {"", "placeholder", 0.0, 0.0, 0, 0, 0.0, false, {"physics-not-implemented"}};
}

bool SolutionBundle::IsHealthy() const {
  return linear_converged && warnings.empty();
}

std::string SolutionBundle::Summary() const {
  std::ostringstream stream;
  stream << "case_id=" << (case_id.empty() ? "unknown" : case_id) << ", status=" << status
         << ", magnetic_energy=" << magnetic_energy << ", torque_estimate=" << torque_estimate
         << ", primary_dofs=" << primary_dof_count
         << ", linear_iterations=" << linear_iterations
         << ", residual_norm=" << residual_norm
         << ", linear_converged=" << (linear_converged ? "true" : "false")
         << ", warnings=" << warnings.size();
  return stream.str();
}

}  // namespace femsolver::post
