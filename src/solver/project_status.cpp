#include "femsolver/solver/project_status.hpp"

namespace femsolver::solver {

std::vector<RoadmapMilestone> BootstrapRoadmap() {
  return {
      {"v0.1.0", "repository governance and engineering baseline"},
      {"v0.2.0", "self-owned kernel foundation with tetra mesh, quadrature, H1, and scalar assembly"},
      {"v0.3.0",
       "case catalog and preprocessing baseline plus generic element/space/order contracts and "
       "self-owned vector-field FEM foundation with H(curl) support"},
      {"v1.0.0", "robot-joint permanent-magnet motor electromagnetics MVP"},
      {"v1.1.0", "nonlinear and anisotropic magnetic materials plus broader element-family expansion"},
      {"v2.0.0", "multiphysics coupling foundation"},
      {"v3.0.0", "design-study and physics-AI interfaces"},
      {"v4.0.0", "motion and low-frequency transients"}};
}

std::vector<std::string> ApplicationNames() {
  return {"motor_pre", "motor_solve", "motor_check"};
}

std::string RepositoryScope() {
  return "Self-owned finite element and multiphysics solver with a robot-joint motor as the first application MVP";
}

}  // namespace femsolver::solver
