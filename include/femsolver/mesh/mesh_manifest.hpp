#pragma once

#include <filesystem>
#include <map>
#include <string>
#include <vector>

namespace femsolver::mesh {

enum class MeshFamily { kTetrahedral, kHexahedral };

struct PeriodicPair {
  std::string name;
  int master_boundary;
  int slave_boundary;
};

struct MeshManifest {
  MeshFamily family = MeshFamily::kTetrahedral;
  std::map<std::string, int> region_attributes;
  std::map<std::string, int> boundary_attributes;
  std::vector<PeriodicPair> periodic_pairs;
  std::string case_id;
  std::string source_kind;
  std::string source_url;
  std::string reconstruction_level;
  std::string geometry_variant;
  std::string space_family;
  int polynomial_order = 1;
  std::vector<std::string> region_names;
  std::vector<std::string> boundary_names;
  std::map<std::string, double> geometry_parameters;
  std::map<std::string, std::string> geometry_metadata;
  std::map<std::string, std::string> files;
  bool mesh_generated = false;
  std::string mesh_status;
  int mesh_dimension = 0;

  [[nodiscard]] bool IsValid() const;
  [[nodiscard]] bool HasRegion(const std::string& name) const;
  [[nodiscard]] bool HasBoundary(const std::string& name) const;
  [[nodiscard]] std::string Summary() const;

  static MeshManifest LoadFromFile(const std::filesystem::path& path);
};

std::string ToString(MeshFamily family);

}  // namespace femsolver::mesh
