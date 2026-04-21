#pragma once

#include <string>
#include <vector>

namespace femsolver::case_config {

struct CaseSpec {
  std::string machine_name;
  std::string mesh_path;
  bool periodic_sector;
  std::vector<std::string> requested_outputs;

  static CaseSpec Bootstrap();
  [[nodiscard]] std::string Summary() const;
};

}  // namespace femsolver::case_config
