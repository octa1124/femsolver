#include <gtest/gtest.h>

#include <stdexcept>
#include <vector>

#include "femsolver/material/bh_curve.hpp"

namespace {

femsolver::material::MonotoneBhCurve MakeCurve(const double scale) {
  return femsolver::material::MonotoneBhCurve({
      {0.0, 0.0},
      {1.0, 100.0 * scale},
      {1.5, 400.0 * scale},
      {2.0, 1200.0 * scale},
  });
}

}  // namespace

TEST(MaterialBhCurveTest, InterpolatesEnergyAndTangentOnMonotoneCurve) {
  const auto curve = MakeCurve(1.0);

  EXPECT_TRUE(curve.IsMonotone());
  EXPECT_NEAR(curve.FieldIntensity(1.25), 250.0, 1.0e-12);
  EXPECT_NEAR(curve.DifferentialReluctivity(1.25), 600.0, 1.0e-12);
  EXPECT_NEAR(curve.SecantReluctivity(1.25), 200.0, 1.0e-12);
  EXPECT_NEAR(curve.EnergyDensity(1.5), 175.0, 1.0e-12);
  EXPECT_NE(curve.Summary().find("bh_points=4"), std::string::npos);
}

TEST(MaterialBhCurveTest, RejectsNonMonotoneOrOriginlessCurves) {
  EXPECT_THROW(
      femsolver::material::MonotoneBhCurve({{0.1, 0.0}, {1.0, 10.0}}),
      std::invalid_argument);
  EXPECT_THROW(
      femsolver::material::MonotoneBhCurve({{0.0, 0.0}, {1.0, 10.0}, {0.9, 11.0}}),
      std::invalid_argument);
  EXPECT_THROW(
      femsolver::material::MonotoneBhCurve({{0.0, 0.0}, {1.0, 10.0}, {2.0, 9.0}}),
      std::invalid_argument);
}

TEST(MaterialBhCurveTest, OrthotropicMaterialMatchesFiniteDifferenceTangent) {
  const femsolver::material::OrthotropicBhMaterial material(
      MakeCurve(1.0), MakeCurve(2.0), MakeCurve(4.0));

  const femsolver::material::Vector3 b{1.25, -1.25, 1.25};
  const auto h = material.FieldIntensity(b);
  const auto tangent = material.ConsistentTangent(b);
  const auto check = material.CheckConsistentTangent(b, 1.0e-6, 1.0e-6);

  EXPECT_NEAR(h[0], 250.0, 1.0e-12);
  EXPECT_NEAR(h[1], -500.0, 1.0e-12);
  EXPECT_NEAR(h[2], 1000.0, 1.0e-12);
  EXPECT_NEAR(tangent[0][0], 600.0, 1.0e-12);
  EXPECT_NEAR(tangent[1][1], 1200.0, 1.0e-12);
  EXPECT_NEAR(tangent[2][2], 2400.0, 1.0e-12);
  EXPECT_NEAR(tangent[0][1], 0.0, 1.0e-12);
  EXPECT_TRUE(check.passed);
  EXPECT_NEAR(material.EnergyDensity(b), 93.75 * 7.0, 1.0e-12);
}
