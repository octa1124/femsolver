#include "femsolver/io/text_report.hpp"

#include <sstream>

#include "femsolver/core/version.hpp"

namespace femsolver::io {

std::string RenderBanner(const std::string& app_name) {
  std::ostringstream stream;
  stream << femsolver::core::kProjectName << " " << femsolver::core::kVersion << " :: " << app_name;
  return stream.str();
}

std::string RenderPreReport(const case_config::CaseSpec& spec) {
  std::ostringstream stream;
  stream << "bootstrap preprocessor report\n";
  stream << "case: " << spec.Summary();
  return stream.str();
}

std::string RenderSolveReport(const case_config::CaseSpec& spec,
                              const fem::DiscretizationStrategy& strategy,
                              const nonlinear::NonlinearPolicy& policy) {
  std::ostringstream stream;
  stream << "bootstrap solver report\n";
  stream << "case: " << spec.Summary() << '\n';
  stream << "discretization: " << strategy.Summary() << '\n';
  stream << "nonlinear policy: " << policy.Summary();
  return stream.str();
}

std::string RenderCheckReport(const post::SolutionBundle& bundle) {
  std::ostringstream stream;
  stream << "bootstrap check report\n";
  stream << "solution: " << bundle.Summary();
  return stream.str();
}

}  // namespace femsolver::io
