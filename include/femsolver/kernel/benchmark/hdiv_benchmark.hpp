#pragma once

#include <string>

namespace femsolver::kernel::benchmark {

struct HdivBenchmarkResult {
  int basis_count = 0;
  double max_face_flux_error = 0.0;
  double max_divergence_integral_error = 0.0;
  bool passed = false;

  [[nodiscard]] std::string Summary() const;
};

HdivBenchmarkResult RunReferenceTetraRtP0Benchmark(double tolerance = 1.0e-12);

}  // namespace femsolver::kernel::benchmark
