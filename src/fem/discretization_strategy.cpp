#include "femsolver/fem/discretization_strategy.hpp"

#include <sstream>

namespace femsolver::fem {

DiscretizationStrategy DiscretizationStrategy::Bootstrap() {
  return {"A", "H(curl)", "RT", "RT"};
}

std::string DiscretizationStrategy::Summary() const {
  std::ostringstream stream;
  stream << "primary_unknown=" << primary_unknown << ", primary_space=" << primary_space
         << ", magnetization_space=" << magnetization_space << ", flux_space=" << flux_space;
  return stream.str();
}

}  // namespace femsolver::fem
