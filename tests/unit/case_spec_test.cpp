#include <gtest/gtest.h>

#include "femsolver/case/case_spec.hpp"

TEST(CaseSpecTest, BootstrapCaseHasExpectedDefaults) {
  const auto spec = femsolver::case_config::CaseSpec::Bootstrap();
  EXPECT_EQ(spec.machine_name, "bootstrap-ipm-placeholder");
  EXPECT_FALSE(spec.periodic_sector);
  EXPECT_EQ(spec.requested_outputs.size(), 3U);
  EXPECT_NE(spec.Summary().find("outputs=["), std::string::npos);
}
