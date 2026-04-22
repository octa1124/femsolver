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

std::string RenderMachineSolveReport(const case_config::CaseSpec& spec,
                                     const mesh::MeshManifest& manifest,
                                     const fem::DiscretizationStrategy& strategy,
                                     const nonlinear::NonlinearPolicy& policy,
                                     const post::SolutionBundle& bundle) {
  std::ostringstream stream;
  stream << "machine solve report\n";
  stream << "case: " << spec.Summary() << '\n';
  stream << "manifest: " << manifest.Summary() << '\n';
  stream << "discretization: " << strategy.Summary() << '\n';
  stream << "nonlinear policy: " << policy.Summary() << '\n';
  stream << "solution: " << bundle.Summary();
  return stream.str();
}

std::string RenderCheckReport(const post::SolutionBundle& bundle) {
  std::ostringstream stream;
  stream << "bootstrap check report\n";
  stream << "solution: " << bundle.Summary();
  return stream.str();
}

std::string RenderKernelPoissonBenchmarkReport(
    const kernel::benchmark::PoissonBenchmarkResult& result) {
  std::ostringstream stream;
  stream << "kernel poisson benchmark\n";
  stream << "linear_solver: converged=" << (result.linear_solver.converged ? "true" : "false")
         << ", iterations=" << result.linear_solver.iterations
         << ", residual_norm=" << result.linear_solver.residual_norm << '\n';
  stream << "solution: nodes=" << result.solution.size()
         << ", max_abs_error=" << result.max_abs_error;
  return stream.str();
}

std::string RenderKernelCurlCurlBenchmarkReport(
    const kernel::benchmark::CurlCurlBenchmarkResult& result) {
  std::ostringstream stream;
  stream << "kernel curl-curl benchmark\n";
  stream << "linear_solver: converged=" << (result.linear_solver.converged ? "true" : "false")
         << ", iterations=" << result.linear_solver.iterations
         << ", residual_norm=" << result.linear_solver.residual_norm << '\n';
  stream << "solution: edge_dofs=" << result.solution_dofs.size()
         << ", max_abs_error=" << result.max_abs_error;
  return stream.str();
}

}  // namespace femsolver::io
