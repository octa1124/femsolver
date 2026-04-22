#include <gtest/gtest.h>

#include "femsolver/post/solution_bundle.hpp"

TEST(SolutionBundleTest, BootstrapBundleHasWarningUntilPhysicsExists) {
  const auto bundle = femsolver::post::SolutionBundle::Bootstrap();
  EXPECT_EQ(bundle.status, "placeholder");
  EXPECT_EQ(bundle.warnings.size(), 1U);
  EXPECT_FALSE(bundle.IsHealthy());
  EXPECT_NE(bundle.Summary().find("warnings=1"), std::string::npos);
}
