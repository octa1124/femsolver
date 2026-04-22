#pragma once

#include <string>

#include "femsolver/case/case_spec.hpp"
#include "femsolver/fem/discretization_strategy.hpp"
#include "femsolver/kernel/benchmark/poisson_benchmark.hpp"
#include "femsolver/nonlinear/nonlinear_policy.hpp"
#include "femsolver/post/solution_bundle.hpp"

namespace femsolver::io {

std::string RenderBanner(const std::string& app_name);
std::string RenderPreReport(const case_config::CaseSpec& spec);
std::string RenderSolveReport(const case_config::CaseSpec& spec,
                              const fem::DiscretizationStrategy& strategy,
                              const nonlinear::NonlinearPolicy& policy);
std::string RenderCheckReport(const post::SolutionBundle& bundle);
std::string RenderKernelPoissonBenchmarkReport(
    const kernel::benchmark::PoissonBenchmarkResult& result);

}  // namespace femsolver::io
