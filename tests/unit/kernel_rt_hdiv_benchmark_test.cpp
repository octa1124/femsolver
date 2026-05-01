#include <gtest/gtest.h>

#include <string>

#include "femsolver/kernel/basis/raviart_thomas_p0_tetrahedron.hpp"
#include "femsolver/kernel/benchmark/hdiv_benchmark.hpp"

TEST(KernelRtHdivBenchmarkTest, ReferenceRtP0BasisInterpolatesFaceFluxes) {
  const auto flux_matrix =
      femsolver::kernel::basis::RaviartThomasP0Tetrahedron::ReferenceFaceFluxMatrix();

  for (std::size_t basis_index = 0; basis_index < flux_matrix.size(); ++basis_index) {
    for (std::size_t face_index = 0; face_index < flux_matrix[basis_index].size(); ++face_index) {
      const double expected = basis_index == face_index ? 1.0 : 0.0;
      EXPECT_NEAR(flux_matrix[basis_index][face_index], expected, 1.0e-12);
    }
  }
}

TEST(KernelRtHdivBenchmarkTest, ReferenceRtP0HasConstantDivergence) {
  const auto divergences =
      femsolver::kernel::basis::RaviartThomasP0Tetrahedron::ReferenceDivergences();

  for (const double divergence : divergences) {
    EXPECT_NEAR(divergence, 6.0, 1.0e-12);
  }
}

TEST(KernelRtHdivBenchmarkTest, CanonicalHdivBenchmarkPasses) {
  const auto result = femsolver::kernel::benchmark::RunReferenceTetraRtP0Benchmark();

  EXPECT_TRUE(result.passed);
  EXPECT_EQ(result.basis_count, 4);
  EXPECT_NE(result.Summary().find("passed=true"), std::string::npos);
}
