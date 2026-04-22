#include <gtest/gtest.h>

#include "femsolver/kernel/benchmark/poisson_benchmark.hpp"

TEST(KernelPoissonBenchmarkTest, ManufacturedLinearCaseConvergesToExactNodalValues) {
  const auto result = femsolver::kernel::benchmark::RunPoissonBenchmark(
      femsolver::kernel::benchmark::MakeManufacturedLinearPoissonBenchmark());

  EXPECT_TRUE(result.linear_solver.converged);
  EXPECT_LE(result.linear_solver.iterations, 16);
  EXPECT_NEAR(result.max_abs_error, 0.0, 1.0e-12);
  ASSERT_EQ(result.solution.size(), 5U);
  EXPECT_NEAR(result.solution[4], 1.5, 1.0e-12);
}
