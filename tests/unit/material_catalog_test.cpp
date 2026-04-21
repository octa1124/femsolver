#include <gtest/gtest.h>

#include "femsolver/material/material_catalog.hpp"

TEST(MaterialCatalogTest, BootstrapCatalogIncludesNonlinearOrthotropicSteel) {
  const auto materials = femsolver::material::BootstrapMaterials();

  ASSERT_EQ(materials.size(), 6U);
  EXPECT_EQ(materials.back().name, "orthotropic-nonlinear-steel");
  EXPECT_TRUE(materials.back().uses_bh_curve);
  EXPECT_TRUE(materials.back().requires_consistent_tangent);
  EXPECT_TRUE(
      femsolver::material::UsesBhCurve(femsolver::material::MaterialKind::kOrthotropicNonlinearSteel));
}
