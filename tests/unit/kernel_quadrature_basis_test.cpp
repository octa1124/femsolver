#include <numeric>

#include <gtest/gtest.h>

#include "femsolver/kernel/basis/lagrange_p1_tetrahedron.hpp"
#include "femsolver/kernel/quadrature/tetrahedron_quadrature.hpp"
#include "femsolver/kernel/reference/reference_tetrahedron.hpp"

TEST(KernelQuadratureBasisTest, CentroidQuadratureMatchesReferenceTetraVolume) {
  const auto quadrature = femsolver::kernel::quadrature::MakeCentroidTetrahedronQuadrature();

  ASSERT_EQ(quadrature.points().size(), 1U);
  EXPECT_EQ(quadrature.exact_degree(), 1);
  EXPECT_TRUE(femsolver::kernel::reference::ReferenceTetrahedron::Contains(
      quadrature.points().front().reference_point));
  EXPECT_NEAR(quadrature.points().front().weight, 1.0 / 6.0, 1.0e-12);
}

TEST(KernelQuadratureBasisTest, LagrangeP1BasisFormsPartitionOfUnity) {
  const auto values = femsolver::kernel::basis::LagrangeP1Tetrahedron::Evaluate({0.25, 0.25, 0.25});
  const double sum = std::accumulate(values.begin(), values.end(), 0.0);

  EXPECT_NEAR(sum, 1.0, 1.0e-12);
  for (const double value : values) {
    EXPECT_NEAR(value, 0.25, 1.0e-12);
  }

  const auto reference_gradients =
      femsolver::kernel::basis::LagrangeP1Tetrahedron::ReferenceGradients();
  EXPECT_EQ(reference_gradients.size(), 4U);
  EXPECT_DOUBLE_EQ(reference_gradients[0][0], -1.0);
}
