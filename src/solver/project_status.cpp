#include "femsolver/solver/project_status.hpp"

namespace femsolver::solver {

std::vector<RoadmapMilestone> BootstrapRoadmap() {
  return {
      {"v0.1.0", "repository governance and engineering baseline"},
      {"v1.0.0", "3D locked-rotor IPM full model with orthotropic nonlinear B-H"},
      {"v2.0.0", "periodic sectors and validation expansion"},
      {"v3.0.0", "multi-operating-point design platform"},
      {"v4.0.0", "motion and low-frequency transients"}};
}

std::vector<std::string> ApplicationNames() {
  return {"motor_pre", "motor_solve", "motor_check"};
}

std::string RepositoryScope() {
  return "MFEM-first finite element solver and validation platform for 3D electrical machines";
}

}  // namespace femsolver::solver
