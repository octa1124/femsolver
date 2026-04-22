#include <gtest/gtest.h>

#include "femsolver/kernel/benchmark/curl_curl_benchmark.hpp"

TEST(KernelCurlCurlBenchmarkTest, ManufacturedCurlCurlCaseConvergesToExactEdgeDofs) {
  const auto result = femsolver::kernel::benchmark::RunCurlCurlBenchmark(
      femsolver::kernel::benchmark::MakeManufacturedCurlCurlBenchmark());

  EXPECT_TRUE(result.linear_solver.converged);
  EXPECT_LE(result.linear_solver.iterations, 32);
  EXPECT_NEAR(result.max_abs_error, 0.0, 1.0e-11);
  ASSERT_EQ(result.solution_dofs.size(), 10U);
}
