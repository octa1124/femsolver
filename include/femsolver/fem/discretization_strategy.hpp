#pragma once

#include <string>

namespace femsolver::fem {

struct DiscretizationStrategy {
  std::string primary_unknown;
  std::string primary_space;
  std::string magnetization_space;
  std::string flux_space;

  static DiscretizationStrategy Bootstrap();
  [[nodiscard]] std::string Summary() const;
};

}  // namespace femsolver::fem
