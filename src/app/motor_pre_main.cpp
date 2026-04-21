#include <iostream>
#include <string_view>

#include "femsolver/case/case_spec.hpp"
#include "femsolver/io/text_report.hpp"

namespace {

constexpr std::string_view kUsage =
    "Usage: motor_pre [--help]\n"
    "Bootstrap preprocessor placeholder for the femsolver repository baseline.\n";

}  // namespace

int main(const int argc, char** argv) {
  if (argc > 1 && std::string_view(argv[1]) == "--help") {
    std::cout << kUsage;
    return 0;
  }

  const auto spec = femsolver::case_config::CaseSpec::Bootstrap();
  std::cout << femsolver::io::RenderBanner("motor_pre") << '\n';
  std::cout << femsolver::io::RenderPreReport(spec) << '\n';
  return 0;
}
