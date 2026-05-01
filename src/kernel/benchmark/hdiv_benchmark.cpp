#include "femsolver/kernel/benchmark/hdiv_benchmark.hpp"

#include <algorithm>
#include <cmath>
#include <sstream>

#include "femsolver/kernel/basis/raviart_thomas_p0_tetrahedron.hpp"
#include "femsolver/kernel/reference/reference_tetrahedron.hpp"

namespace femsolver::kernel::benchmark {

std::string HdivBenchmarkResult::Summary() const {
  std::ostringstream stream;
  stream << "basis_count=" << basis_count << ", max_face_flux_error=" << max_face_flux_error
         << ", max_divergence_integral_error=" << max_divergence_integral_error
         << ", passed=" << (passed ? "true" : "false");
  return stream.str();
}

HdivBenchmarkResult RunReferenceTetraRtP0Benchmark(const double tolerance) {
  const auto flux_matrix = basis::RaviartThomasP0Tetrahedron::ReferenceFaceFluxMatrix();
  const auto divergences = basis::RaviartThomasP0Tetrahedron::ReferenceDivergences();
  const double volume = reference::ReferenceTetrahedron::Volume();

  HdivBenchmarkResult result;
  result.basis_count = 4;
  for (std::size_t basis_index = 0; basis_index < flux_matrix.size(); ++basis_index) {
    for (std::size_t face_index = 0; face_index < flux_matrix[basis_index].size(); ++face_index) {
      const double expected = basis_index == face_index ? 1.0 : 0.0;
      result.max_face_flux_error =
          std::max(result.max_face_flux_error,
                   std::abs(flux_matrix[basis_index][face_index] - expected));
    }
    result.max_divergence_integral_error =
        std::max(result.max_divergence_integral_error,
                 std::abs(divergences[basis_index] * volume - 1.0));
  }
  result.passed = result.max_face_flux_error <= tolerance &&
                  result.max_divergence_integral_error <= tolerance;
  return result;
}

}  // namespace femsolver::kernel::benchmark
