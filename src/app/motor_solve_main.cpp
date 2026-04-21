#include <iostream>
#include <string_view>

#include "femsolver/case/case_spec.hpp"
#include "femsolver/fem/discretization_strategy.hpp"
#include "femsolver/io/text_report.hpp"
#include "femsolver/nonlinear/nonlinear_policy.hpp"

namespace {

constexpr std::string_view kUsage =
    "Usage: motor_solve [--help]\n"
    "Bootstrap solver placeholder for the femsolver repository baseline.\n";

}  // namespace

int main(const int argc, char** argv) {
  if (argc > 1 && std::string_view(argv[1]) == "--help") {
    std::cout << kUsage;
    return 0;
  }

  const auto spec = femsolver::case_config::CaseSpec::Bootstrap();
  const auto strategy = femsolver::fem::DiscretizationStrategy::Bootstrap();
  const auto policy = femsolver::nonlinear::NonlinearPolicy::Bootstrap();
  std::cout << femsolver::io::RenderBanner("motor_solve") << '\n';
  std::cout << femsolver::io::RenderSolveReport(spec, strategy, policy) << '\n';
  return 0;
}
