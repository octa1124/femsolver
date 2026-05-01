#include <gtest/gtest.h>

#include "femsolver/solver/project_status.hpp"

TEST(ProjectStatusTest, RoadmapContainsExpectedMilestones) {
  const auto milestones = femsolver::solver::BootstrapRoadmap();
  const auto apps = femsolver::solver::ApplicationNames();

  EXPECT_EQ(milestones.size(), 10U);
  EXPECT_EQ(milestones.front().version, "v0.1.0");
  EXPECT_EQ(apps.size(), 3U);
  EXPECT_NE(femsolver::solver::RepositoryScope().find("Self-owned"), std::string::npos);
}
