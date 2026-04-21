#include <gtest/gtest.h>

#include "femsolver/excitation/excitation_def.hpp"

TEST(ExcitationDefTest, ConfiguredExcitationRequiresRegionAndMagnitude) {
  femsolver::excitation::ExcitationDef excitation{
      femsolver::excitation::ExcitationKind::kCurrentDensity, "phase_a", 12.5, false};

  EXPECT_TRUE(excitation.IsConfigured());
  EXPECT_NE(excitation.Summary().find("phase_a"), std::string::npos);
}

TEST(ExcitationDefTest, EmptyRegionIsNotConfigured) {
  femsolver::excitation::ExcitationDef excitation{
      femsolver::excitation::ExcitationKind::kMagnetization, "", 2.0, true};

  EXPECT_FALSE(excitation.IsConfigured());
}
