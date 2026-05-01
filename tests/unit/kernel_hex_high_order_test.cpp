#include <gtest/gtest.h>

#include <numeric>

#include "femsolver/kernel/basis/lagrange_hex_hexahedron.hpp"
#include "femsolver/kernel/quadrature/hexahedron_quadrature.hpp"
#include "femsolver/kernel/reference/reference_hexahedron.hpp"

TEST(KernelHexHighOrderTest, ReferenceHexahedronUsesTensorProductDomain) {
  const auto vertices = femsolver::kernel::reference::ReferenceHexahedron::Vertices();

  EXPECT_EQ(vertices.size(), 8U);
  EXPECT_DOUBLE_EQ(femsolver::kernel::reference::ReferenceHexahedron::Volume(), 8.0);
  EXPECT_TRUE(femsolver::kernel::reference::ReferenceHexahedron::Contains({0.0, 0.0, 0.0}));
  EXPECT_FALSE(femsolver::kernel::reference::ReferenceHexahedron::Contains({1.2, 0.0, 0.0}));
}

TEST(KernelHexHighOrderTest, Q1BasisPartitionsUnityAndInterpolatesVertices) {
  const auto values = femsolver::kernel::basis::LagrangeQ1Hexahedron::Evaluate({0.25, -0.5, 0.75});
  const double sum = std::accumulate(values.begin(), values.end(), 0.0);

  EXPECT_NEAR(sum, 1.0, 1.0e-12);

  const auto vertex_values =
      femsolver::kernel::basis::LagrangeQ1Hexahedron::Evaluate({-1.0, -1.0, -1.0});
  EXPECT_NEAR(vertex_values[0], 1.0, 1.0e-12);
  for (std::size_t index = 1; index < vertex_values.size(); ++index) {
    EXPECT_NEAR(vertex_values[index], 0.0, 1.0e-12);
  }
}

TEST(KernelHexHighOrderTest, Q2BasisPartitionsUnityAndInterpolatesTensorNodes) {
  const auto values = femsolver::kernel::basis::LagrangeQ2Hexahedron::Evaluate({0.2, -0.4, 0.6});
  const double sum = std::accumulate(values.begin(), values.end(), 0.0);
  const auto center_values =
      femsolver::kernel::basis::LagrangeQ2Hexahedron::Evaluate({0.0, 0.0, 0.0});
  const auto gradients =
      femsolver::kernel::basis::LagrangeQ2Hexahedron::ReferenceGradients({0.2, -0.4, 0.6});

  EXPECT_NEAR(sum, 1.0, 1.0e-12);
  EXPECT_NEAR(center_values[13], 1.0, 1.0e-12);
  for (std::size_t index = 0; index < center_values.size(); ++index) {
    if (index == 13U) {
      continue;
    }
    EXPECT_NEAR(center_values[index], 0.0, 1.0e-12);
  }

  femsolver::kernel::basis::HexVector3 gradient_sum{0.0, 0.0, 0.0};
  for (const auto& gradient : gradients) {
    gradient_sum[0] += gradient[0];
    gradient_sum[1] += gradient[1];
    gradient_sum[2] += gradient[2];
  }
  EXPECT_NEAR(gradient_sum[0], 0.0, 1.0e-12);
  EXPECT_NEAR(gradient_sum[1], 0.0, 1.0e-12);
  EXPECT_NEAR(gradient_sum[2], 0.0, 1.0e-12);
}

TEST(KernelHexHighOrderTest, HexahedronQuadratureRulesIntegrateConstants) {
  const auto q2 = femsolver::kernel::quadrature::MakeHexahedronGaussLegendreRule(2);
  const auto q3 = femsolver::kernel::quadrature::MakeHexahedronGaussLegendreRule(3);

  EXPECT_EQ(q2.points().size(), 8U);
  EXPECT_EQ(q3.points().size(), 27U);
  EXPECT_NEAR(q2.WeightSum(), 8.0, 1.0e-12);
  EXPECT_NEAR(q3.WeightSum(), 8.0, 1.0e-12);
}
