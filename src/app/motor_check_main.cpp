#include <iostream>
#include <string_view>

#include "femsolver/io/text_report.hpp"
#include "femsolver/kernel/benchmark/poisson_benchmark.hpp"
#include "femsolver/post/solution_bundle.hpp"

namespace {

constexpr std::string_view kUsage =
    "Usage: motor_check [--help] [--poisson-benchmark]\n"
    "Run kernel verification checks. The default check is the v0.2.0 scalar Poisson benchmark.\n";

}  // namespace

int main(const int argc, char** argv) {
  if (argc > 1 && std::string_view(argv[1]) == "--help") {
    std::cout << kUsage;
    return 0;
  }

  if (argc > 1 && std::string_view(argv[1]) != "--poisson-benchmark") {
    std::cerr << "Unsupported option: " << argv[1] << '\n' << kUsage;
    return 1;
  }

  const auto benchmark_result = femsolver::kernel::benchmark::RunPoissonBenchmark(
      femsolver::kernel::benchmark::MakeManufacturedLinearPoissonBenchmark());
  const auto bundle = femsolver::post::SolutionBundle::Bootstrap();
  std::cout << femsolver::io::RenderBanner("motor_check") << '\n';
  std::cout << femsolver::io::RenderKernelPoissonBenchmarkReport(benchmark_result) << '\n';
  std::cout << femsolver::io::RenderCheckReport(bundle) << '\n';
  return 0;
}
