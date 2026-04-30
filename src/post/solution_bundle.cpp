#include "femsolver/post/solution_bundle.hpp"

#include <sstream>

namespace femsolver::post {

SolutionBundle SolutionBundle::Bootstrap() {
  SolutionBundle bundle;
  bundle.status = "placeholder";
  bundle.warnings = {"physics-not-implemented"};
  return bundle;
}

bool SolutionBundle::IsHealthy() const {
  return linear_converged && warnings.empty();
}

std::string SolutionBundle::Summary() const {
  std::ostringstream stream;
  stream << "case_id=" << (case_id.empty() ? "unknown" : case_id) << ", status=" << status
         << ", magnetic_energy=" << magnetic_energy << ", torque_estimate=" << torque_estimate
         << ", average_flux_density_magnitude=" << average_flux_density_magnitude
         << ", max_flux_density_magnitude=" << max_flux_density_magnitude
         << ", torque_surface_area=" << torque_surface_area
         << ", torque_surface_face_count=" << torque_surface_face_count
         << ", primary_dofs=" << primary_dof_count
         << ", linear_iterations=" << linear_iterations
         << ", residual_norm=" << residual_norm
         << ", linear_converged=" << (linear_converged ? "true" : "false")
         << ", warnings=" << warnings.size();
  return stream.str();
}

}  // namespace femsolver::post
