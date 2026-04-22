#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>

#include "femsolver/io/text_report.hpp"
#include "femsolver/kernel/benchmark/curl_curl_benchmark.hpp"
#include "femsolver/kernel/benchmark/poisson_benchmark.hpp"
#include "femsolver/post/solution_bundle.hpp"

namespace {

constexpr std::string_view kUsage =
    "Usage: motor_check [--help] [--poisson-benchmark] [--curl-curl-benchmark]\n"
    "Run kernel verification checks. The default check runs both scalar and vector canonical "
    "benchmarks.\n";

struct CliOptions {
  bool help = false;
  bool run_poisson = false;
  bool run_curl_curl = false;
};

CliOptions ParseArguments(const int argc, char** argv) {
  CliOptions options;
  for (int index = 1; index < argc; ++index) {
    const std::string_view argument = argv[index];
    if (argument == "--help") {
      options.help = true;
      continue;
    }
    if (argument == "--poisson-benchmark") {
      options.run_poisson = true;
      continue;
    }
    if (argument == "--curl-curl-benchmark") {
      options.run_curl_curl = true;
      continue;
    }

    throw std::runtime_error("Unsupported option: " + std::string(argument));
  }
  return options;
}

}  // namespace

int main(const int argc, char** argv) {
  try {
    const CliOptions options = ParseArguments(argc, argv);
    if (options.help) {
      std::cout << kUsage;
      return 0;
    }

    const bool run_default = argc == 1;
    const bool run_poisson = run_default || options.run_poisson;
    const bool run_curl_curl = run_default || options.run_curl_curl;

    const auto bundle = femsolver::post::SolutionBundle::Bootstrap();
    std::cout << femsolver::io::RenderBanner("motor_check") << '\n';
    if (run_poisson) {
      const auto poisson_result = femsolver::kernel::benchmark::RunPoissonBenchmark(
          femsolver::kernel::benchmark::MakeManufacturedLinearPoissonBenchmark());
      std::cout << femsolver::io::RenderKernelPoissonBenchmarkReport(poisson_result) << '\n';
    }
    if (run_curl_curl) {
      const auto curl_curl_result = femsolver::kernel::benchmark::RunCurlCurlBenchmark(
          femsolver::kernel::benchmark::MakeManufacturedCurlCurlBenchmark());
      std::cout << femsolver::io::RenderKernelCurlCurlBenchmarkReport(curl_curl_result) << '\n';
    }
    std::cout << femsolver::io::RenderCheckReport(bundle) << '\n';
    return 0;
  } catch (const std::exception& error) {
    std::cerr << error.what() << '\n' << kUsage;
    return 1;
  }
}
