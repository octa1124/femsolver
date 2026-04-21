#pragma once

#include <string>
#include <vector>

namespace femsolver::solver {

struct RoadmapMilestone {
  std::string version;
  std::string description;
};

std::vector<RoadmapMilestone> BootstrapRoadmap();
std::vector<std::string> ApplicationNames();
std::string RepositoryScope();

}  // namespace femsolver::solver
