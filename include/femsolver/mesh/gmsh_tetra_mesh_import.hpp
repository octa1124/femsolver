#pragma once

#include <filesystem>
#include <map>
#include <string>

#include "femsolver/kernel/mesh/tetra_mesh.hpp"

namespace femsolver::mesh {

struct ImportedTetraMesh {
  kernel::mesh::TetraMesh mesh;
  std::map<std::string, int> region_attributes;
  std::map<std::string, int> boundary_attributes;

  [[nodiscard]] std::string Summary() const;
};

ImportedTetraMesh ImportGmshMsh2TetraMesh(const std::filesystem::path& path);

}  // namespace femsolver::mesh
