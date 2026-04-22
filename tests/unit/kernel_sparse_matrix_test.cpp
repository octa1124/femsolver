#include <gtest/gtest.h>

#include "femsolver/kernel/algebra/sparse_matrix.hpp"

TEST(KernelSparseMatrixTest, BuilderAccumulatesAndBuildsCsrMatrix) {
  femsolver::kernel::algebra::SparseMatrixBuilder builder(3, 3);
  builder.Add(0, 0, 2.0);
  builder.Add(0, 1, -1.0);
  builder.Add(1, 0, -1.0);
  builder.Add(1, 1, 2.0);
  builder.Add(2, 2, 5.0);
  builder.Add(2, 2, 1.0);

  const auto matrix = builder.Build();
  const std::vector<double> y = matrix.Multiply({1.0, 2.0, 3.0});

  EXPECT_EQ(matrix.rows(), 3);
  EXPECT_EQ(matrix.cols(), 3);
  EXPECT_NEAR(matrix.Entry(2, 2), 6.0, 1.0e-12);
  ASSERT_EQ(y.size(), 3U);
  EXPECT_NEAR(y[0], 0.0, 1.0e-12);
  EXPECT_NEAR(y[1], 3.0, 1.0e-12);
  EXPECT_NEAR(y[2], 18.0, 1.0e-12);
}
