#include <iostream>
#include <string_view>

#include "femsolver/io/text_report.hpp"
#include "femsolver/post/solution_bundle.hpp"

namespace {

constexpr std::string_view kUsage =
    "Usage: motor_check [--help]\n"
    "Bootstrap checker placeholder for the femsolver repository baseline.\n";

}  // namespace

int main(const int argc, char** argv) {
  if (argc > 1 && std::string_view(argv[1]) == "--help") {
    std::cout << kUsage;
    return 0;
  }

  const auto bundle = femsolver::post::SolutionBundle::Bootstrap();
  std::cout << femsolver::io::RenderBanner("motor_check") << '\n';
  std::cout << femsolver::io::RenderCheckReport(bundle) << '\n';
  return 0;
}
