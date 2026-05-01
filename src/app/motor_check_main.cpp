#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>

#include "femsolver/case/case_spec.hpp"
#include "femsolver/io/text_report.hpp"
#include "femsolver/kernel/benchmark/curl_curl_benchmark.hpp"
#include "femsolver/kernel/benchmark/hdiv_benchmark.hpp"
#include "femsolver/kernel/benchmark/poisson_benchmark.hpp"
#include "femsolver/mesh/mesh_manifest.hpp"
#include "femsolver/post/solution_bundle.hpp"
#include "femsolver/solver/joint_motor_linear_smoke.hpp"

namespace {

constexpr std::string_view kUsage =
    "Usage: motor_check [--help] [--poisson-benchmark] [--curl-curl-benchmark] "
    "[--hdiv-benchmark] "
    "[--machine-regression --case <path> --manifest <path>]\n"
    "Run kernel verification checks. The default check runs scalar, H(curl), and H(div) canonical "
    "benchmarks.\n";

struct CliOptions {
  bool help = false;
  bool run_poisson = false;
  bool run_curl_curl = false;
  bool run_hdiv = false;
  bool run_machine_regression = false;
  std::string case_path;
  std::string manifest_path;
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
    if (argument == "--hdiv-benchmark") {
      options.run_hdiv = true;
      continue;
    }
    if (argument == "--machine-regression") {
      options.run_machine_regression = true;
      continue;
    }
    if (argument == "--case" && index + 1 < argc) {
      options.case_path = argv[++index];
      continue;
    }
    if (argument == "--manifest" && index + 1 < argc) {
      options.manifest_path = argv[++index];
      continue;
    }

    throw std::runtime_error("Unsupported option: " + std::string(argument));
  }
  return options;
}

bool PassesMachineRegression(const femsolver::post::SolutionBundle& bundle,
                             const femsolver::mesh::MeshManifest& manifest) {
  if (!bundle.linear_converged || bundle.primary_dof_count <= 0 || bundle.magnetic_energy <= 0.0 ||
      bundle.average_flux_density_magnitude <= 0.0 ||
      bundle.max_flux_density_magnitude < bundle.average_flux_density_magnitude) {
    return false;
  }

  if (manifest.mesh_generated && bundle.torque_surface_face_count <= 0) {
    return false;
  }
  return true;
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
    const bool run_hdiv = run_default || options.run_hdiv;
    if (options.run_machine_regression &&
        (options.case_path.empty() || options.manifest_path.empty())) {
      throw std::runtime_error("motor_check machine regression requires --case and --manifest");
    }

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
    if (run_hdiv) {
      const auto hdiv_result = femsolver::kernel::benchmark::RunReferenceTetraRtP0Benchmark();
      std::cout << "hdiv benchmark: " << hdiv_result.Summary() << '\n';
    }
    if (options.run_machine_regression) {
      const auto spec = femsolver::case_config::CaseSpec::LoadFromFile(options.case_path);
      const auto manifest = femsolver::mesh::MeshManifest::LoadFromFile(options.manifest_path);
      const auto machine_bundle =
          femsolver::solver::RunJointMotorLinearSmokeSolve(spec, manifest);
      std::cout << "machine regression check\n";
      std::cout << "solution: " << machine_bundle.Summary() << '\n';
      return PassesMachineRegression(machine_bundle, manifest) ? 0 : 1;
    }
    std::cout << femsolver::io::RenderCheckReport(bundle) << '\n';
    return 0;
  } catch (const std::exception& error) {
    std::cerr << error.what() << '\n' << kUsage;
    return 1;
  }
}
