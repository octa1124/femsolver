#include <gtest/gtest.h>

#include "femsolver/nonlinear/nonlinear_policy.hpp"

TEST(NonlinearPolicyTest, BootstrapPolicySupportsPicardAndNewton) {
  const auto policy = femsolver::nonlinear::NonlinearPolicy::Bootstrap();
  EXPECT_TRUE(policy.HasBootstrapPath());
  EXPECT_NE(policy.Summary().find("damped_newton=true"), std::string::npos);
}
