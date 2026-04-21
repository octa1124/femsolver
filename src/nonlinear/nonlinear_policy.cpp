#include "femsolver/nonlinear/nonlinear_policy.hpp"

#include <sstream>

namespace femsolver::nonlinear {

NonlinearPolicy NonlinearPolicy::Bootstrap() {
  return {true, true, 2};
}

bool NonlinearPolicy::HasBootstrapPath() const {
  return enable_picard_warm_start && enable_damped_newton && picard_steps >= 1;
}

std::string NonlinearPolicy::Summary() const {
  std::ostringstream stream;
  stream << "picard_warm_start=" << (enable_picard_warm_start ? "true" : "false")
         << ", damped_newton=" << (enable_damped_newton ? "true" : "false")
         << ", picard_steps=" << picard_steps;
  return stream.str();
}

}  // namespace femsolver::nonlinear
