#include "femsolver/mesh/mesh_manifest.hpp"

#include <sstream>

namespace femsolver::mesh {

bool MeshManifest::IsValid() const {
  if (region_attributes.empty()) {
    return false;
  }

  for (const auto& [name, attribute] : region_attributes) {
    if (name.empty() || attribute <= 0) {
      return false;
    }
  }

  for (const auto& [name, attribute] : boundary_attributes) {
    if (name.empty() || attribute <= 0) {
      return false;
    }
  }

  for (const auto& pair : periodic_pairs) {
    if (pair.name.empty() || pair.master_boundary <= 0 || pair.slave_boundary <= 0) {
      return false;
    }
  }

  return true;
}

bool MeshManifest::HasRegion(const std::string& name) const {
  return region_attributes.find(name) != region_attributes.end();
}

std::string MeshManifest::Summary() const {
  std::ostringstream stream;
  stream << "mesh_family=" << ToString(family) << ", regions=" << region_attributes.size()
         << ", boundaries=" << boundary_attributes.size()
         << ", periodic_pairs=" << periodic_pairs.size();
  return stream.str();
}

std::string ToString(const MeshFamily family) {
  switch (family) {
    case MeshFamily::kTetrahedral:
      return "tetrahedral";
    case MeshFamily::kHexahedral:
      return "hexahedral";
  }
  return "unknown";
}

}  // namespace femsolver::mesh
