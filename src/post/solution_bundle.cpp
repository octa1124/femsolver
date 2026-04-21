#include "femsolver/post/solution_bundle.hpp"

#include <sstream>

namespace femsolver::post {

SolutionBundle SolutionBundle::Bootstrap() {
  return {"placeholder", 0.0, 0.0, {"physics-not-implemented"}};
}

std::string SolutionBundle::Summary() const {
  std::ostringstream stream;
  stream << "status=" << status << ", magnetic_energy=" << magnetic_energy
         << ", torque_estimate=" << torque_estimate << ", warnings=" << warnings.size();
  return stream.str();
}

}  // namespace femsolver::post
