#pragma once

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
  MeshFamily family;
  std::map<std::string, int> region_attributes;
  std::map<std::string, int> boundary_attributes;
  std::vector<PeriodicPair> periodic_pairs;

  [[nodiscard]] bool IsValid() const;
  [[nodiscard]] bool HasRegion(const std::string& name) const;
  [[nodiscard]] std::string Summary() const;
};

std::string ToString(MeshFamily family);

}  // namespace femsolver::mesh
