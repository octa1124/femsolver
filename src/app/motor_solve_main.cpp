#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>

#include "femsolver/case/case_spec.hpp"
#include "femsolver/fem/discretization_strategy.hpp"
#include "femsolver/io/text_report.hpp"
#include "femsolver/mesh/mesh_manifest.hpp"
#include "femsolver/nonlinear/nonlinear_policy.hpp"
#include "femsolver/solver/joint_motor_linear_smoke.hpp"

namespace {

constexpr std::string_view kUsage =
    "Usage: motor_solve [--help] [--case <path> --manifest <path>]\n"
    "Run the current solver path. With --case and --manifest, execute the first linear "
    "joint-motor magnetostatic smoke solve.\n";

struct CliOptions {
  bool help = false;
  std::string case_path;
  std::string manifest_path;
};

CliOptions ParseArguments(const int argc, char** argv) {
  CliOptions options;
  for (int index = 1; index < argc; ++index) {
    const std::string argument = argv[index];
    if (argument == "--help") {
      options.help = true;
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
    throw std::runtime_error("Unsupported or incomplete argument: " + argument);
  }
  return options;
}

}  // namespace

int main(const int argc, char** argv) {
  try {
    const auto options = ParseArguments(argc, argv);
    if (options.help) {
      std::cout << kUsage;
      return 0;
    }

    const auto strategy = femsolver::fem::DiscretizationStrategy::Bootstrap();
    const auto policy = femsolver::nonlinear::NonlinearPolicy::Bootstrap();
    std::cout << femsolver::io::RenderBanner("motor_solve") << '\n';

    if (options.case_path.empty() && options.manifest_path.empty()) {
      const auto spec = femsolver::case_config::CaseSpec::Bootstrap();
      std::cout << femsolver::io::RenderSolveReport(spec, strategy, policy) << '\n';
      return 0;
    }
    if (options.case_path.empty() || options.manifest_path.empty()) {
      throw std::runtime_error("motor_solve requires both --case and --manifest when one is set");
    }

    const auto spec = femsolver::case_config::CaseSpec::LoadFromFile(options.case_path);
    const auto manifest = femsolver::mesh::MeshManifest::LoadFromFile(options.manifest_path);
    const auto bundle = femsolver::solver::RunJointMotorLinearSmokeSolve(spec, manifest);
    std::cout << femsolver::io::RenderMachineSolveReport(spec, manifest, strategy, policy, bundle)
              << '\n';
    return bundle.linear_converged ? 0 : 1;
  } catch (const std::exception& error) {
    std::cerr << error.what() << '\n' << kUsage;
    return 1;
  }
}
