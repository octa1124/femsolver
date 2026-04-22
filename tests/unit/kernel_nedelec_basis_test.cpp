#include <array>

#include <gtest/gtest.h>

#include "femsolver/kernel/basis/nedelec_p1_tetrahedron.hpp"

namespace {

double EdgeIntegralAtMidpoint(const std::array<double, 3>& start,
                              const std::array<double, 3>& end,
                              const femsolver::kernel::basis::Vector3& basis_value) {
  const std::array<double, 3> tangent = {
      end[0] - start[0],
      end[1] - start[1],
      end[2] - start[2],
  };
  return basis_value[0] * tangent[0] + basis_value[1] * tangent[1] + basis_value[2] * tangent[2];
}

}  // namespace

TEST(KernelNedelecBasisTest, BasisHasExpectedEdgeInterpolationPropertyOnReferenceEdge01) {
  const auto values =
      femsolver::kernel::basis::FirstOrderNedelecTetrahedron::Evaluate({0.5, 0.0, 0.0});

  ASSERT_EQ(values.size(), 6U);
  EXPECT_NEAR(
      EdgeIntegralAtMidpoint({0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, values[0]), 1.0, 1.0e-12);
  EXPECT_NEAR(
      EdgeIntegralAtMidpoint({0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, values[1]), 0.0, 1.0e-12);
}

TEST(KernelNedelecBasisTest, ReferenceCurlsAreConstantAndNonZeroForCurlBasis) {
  const auto curls = femsolver::kernel::basis::FirstOrderNedelecTetrahedron::ReferenceCurls();

  ASSERT_EQ(curls.size(), 6U);
  EXPECT_NEAR(curls[0][2], 2.0, 1.0e-12);
  EXPECT_NEAR(curls[3][2], 2.0, 1.0e-12);
  EXPECT_NEAR(curls[4][1], -2.0, 1.0e-12);
}
