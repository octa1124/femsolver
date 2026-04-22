#pragma once

#include <filesystem>
#include <map>
#include <string>
#include <vector>

namespace femsolver::case_config {

struct CaseSpec {
  std::string case_id;
  std::string source_kind;
  std::string source_url;
  std::string reconstruction_level;
  std::vector<std::string> geometry_variants;
  std::string cell_type;
  std::string space_family;
  int polynomial_order = 1;
  std::map<std::string, std::string> materials_profile;
  std::map<std::string, std::string> excitation_profile;
  std::vector<std::string> validation_targets;
  std::map<std::string, double> geometry_parameters;

  static CaseSpec Bootstrap();
  static CaseSpec LoadFromFile(const std::filesystem::path& path);
  [[nodiscard]] bool IsValid() const;
  [[nodiscard]] bool SupportsVariant(const std::string& variant) const;
  [[nodiscard]] std::string Summary() const;
};

}  // namespace femsolver::case_config
