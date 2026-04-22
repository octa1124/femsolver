#include <gtest/gtest.h>

#include "femsolver/case/case_spec.hpp"

TEST(CaseSpecTest, BootstrapCaseHasExpectedDefaults) {
  const auto spec = femsolver::case_config::CaseSpec::Bootstrap();
  EXPECT_EQ(spec.case_id, "joint_type_i_12s10p");
  EXPECT_EQ(spec.source_kind, "paper");
  EXPECT_EQ(spec.geometry_variants.size(), 2U);
  EXPECT_TRUE(spec.SupportsVariant("section_2d"));
  EXPECT_FALSE(spec.SupportsVariant("full_3d"));
  EXPECT_NE(spec.Summary().find("materials_profile={"), std::string::npos);
}
