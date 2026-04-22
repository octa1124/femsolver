#include "femsolver/mesh/mesh_manifest.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace femsolver::mesh {

namespace {

std::string Trim(const std::string& value) {
  const auto first = value.find_first_not_of(" \t\r\n");
  if (first == std::string::npos) {
    return "";
  }
  const auto last = value.find_last_not_of(" \t\r\n");
  return value.substr(first, last - first + 1);
}

std::pair<std::string, std::string> SplitKeyValue(const std::string& line) {
  const auto separator = line.find(':');
  if (separator == std::string::npos) {
    throw std::runtime_error("Expected key:value line, got: " + line);
  }
  return {Trim(line.substr(0, separator)), Trim(line.substr(separator + 1))};
}

std::string StripQuotes(const std::string& value) {
  if (value.size() >= 2U &&
      ((value.front() == '"' && value.back() == '"') ||
       (value.front() == '\'' && value.back() == '\''))) {
    return value.substr(1, value.size() - 2U);
  }
  return value;
}

double ParseDouble(const std::string& value, const std::string& key) {
  try {
    return std::stod(value);
  } catch (const std::exception&) {
    throw std::runtime_error("Expected numeric value for " + key + ", got: " + value);
  }
}

bool ParseBool(const std::string& value, const std::string& key) {
  if (value == "true") {
    return true;
  }
  if (value == "false") {
    return false;
  }
  throw std::runtime_error("Expected boolean value for " + key + ", got: " + value);
}

enum class Section {
  kNone,
  kRegionNames,
  kBoundaryNames,
  kFiles,
  kMeshGeneration,
  kGeometryParameters,
  kValidationTargets,
  kIgnoredMap,
};

}  // namespace

bool MeshManifest::IsValid() const {
  if (region_attributes.empty() && region_names.empty()) {
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

  for (const auto& name : region_names) {
    if (name.empty()) {
      return false;
    }
  }

  for (const auto& name : boundary_names) {
    if (name.empty()) {
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
  return region_attributes.find(name) != region_attributes.end() ||
         std::find(region_names.begin(), region_names.end(), name) != region_names.end();
}

bool MeshManifest::HasBoundary(const std::string& name) const {
  return boundary_attributes.find(name) != boundary_attributes.end() ||
         std::find(boundary_names.begin(), boundary_names.end(), name) != boundary_names.end();
}

std::string MeshManifest::Summary() const {
  std::ostringstream stream;
  stream << "mesh_family=" << ToString(family)
         << ", case_id=" << (case_id.empty() ? "unknown" : case_id)
         << ", variant=" << (geometry_variant.empty() ? "unknown" : geometry_variant)
         << ", regions=" << (region_names.empty() ? region_attributes.size() : region_names.size())
         << ", boundaries="
         << (boundary_names.empty() ? boundary_attributes.size() : boundary_names.size())
         << ", mesh_generated=" << (mesh_generated ? "true" : "false")
         << ", mesh_status=" << (mesh_status.empty() ? "unspecified" : mesh_status);
  return stream.str();
}

MeshManifest MeshManifest::LoadFromFile(const std::filesystem::path& path) {
  std::ifstream stream(path);
  if (!stream) {
    throw std::runtime_error("Unable to open mesh manifest: " + path.string());
  }

  MeshManifest manifest;
  Section current_section = Section::kNone;
  std::string line;
  while (std::getline(stream, line)) {
    const std::size_t indent = line.find_first_not_of(' ');
    const std::string trimmed = Trim(line);
    if (trimmed.empty() || trimmed.front() == '#') {
      continue;
    }

    if (trimmed.rfind("- ", 0) == 0) {
      if (current_section == Section::kRegionNames) {
        manifest.region_names.push_back(StripQuotes(Trim(trimmed.substr(2))));
        continue;
      }
      if (current_section == Section::kBoundaryNames) {
        manifest.boundary_names.push_back(StripQuotes(Trim(trimmed.substr(2))));
        continue;
      }
      if (current_section == Section::kValidationTargets) {
        continue;
      }
    }

    if (indent == 0 || indent == std::string::npos) {
      current_section = Section::kNone;
      const auto [key, raw_value] = SplitKeyValue(trimmed);
      const std::string value = StripQuotes(raw_value);
      if (raw_value.empty()) {
        if (key == "region_names") {
          current_section = Section::kRegionNames;
        } else if (key == "boundary_names") {
          current_section = Section::kBoundaryNames;
        } else if (key == "files") {
          current_section = Section::kFiles;
        } else if (key == "mesh_generation") {
          current_section = Section::kMeshGeneration;
        } else if (key == "geometry_parameters") {
          current_section = Section::kGeometryParameters;
        } else if (key == "validation_targets") {
          current_section = Section::kValidationTargets;
        } else if (key == "materials_profile" || key == "excitation_profile") {
          current_section = Section::kIgnoredMap;
        }
        continue;
      }

      if (key == "case_id") {
        manifest.case_id = value;
      } else if (key == "source_kind") {
        manifest.source_kind = value;
      } else if (key == "source_url") {
        manifest.source_url = value;
      } else if (key == "reconstruction_level") {
        manifest.reconstruction_level = value;
      } else if (key == "geometry_variant") {
        manifest.geometry_variant = value;
      } else if (key == "cell_type") {
        manifest.family = value == "hexahedron" ? MeshFamily::kHexahedral : MeshFamily::kTetrahedral;
      } else if (key == "space_family") {
        manifest.space_family = value;
      } else if (key == "polynomial_order") {
        manifest.polynomial_order = static_cast<int>(ParseDouble(value, key));
      }
      continue;
    }

    if (current_section == Section::kRegionNames || current_section == Section::kBoundaryNames) {
      if (trimmed.rfind("- ", 0) != 0) {
        throw std::runtime_error("Expected list item in mesh manifest: " + trimmed);
      }
      const std::string item = StripQuotes(Trim(trimmed.substr(2)));
      if (current_section == Section::kRegionNames) {
        manifest.region_names.push_back(item);
      } else {
        manifest.boundary_names.push_back(item);
      }
      continue;
    }

    if (current_section == Section::kValidationTargets) {
      if (trimmed.rfind("- ", 0) != 0) {
        throw std::runtime_error("Expected list item in mesh manifest: " + trimmed);
      }
      continue;
    }

    if (current_section == Section::kIgnoredMap) {
      continue;
    }

    const auto [key, raw_value] = SplitKeyValue(trimmed);
    const std::string value = StripQuotes(raw_value);
    if (current_section == Section::kFiles) {
      manifest.files[key] = value;
    } else if (current_section == Section::kMeshGeneration) {
      if (key == "generated") {
        manifest.mesh_generated = ParseBool(value, key);
      } else if (key == "status") {
        manifest.mesh_status = value;
      } else if (key == "dimension") {
        manifest.mesh_dimension = static_cast<int>(ParseDouble(value, key));
      }
    } else if (current_section == Section::kGeometryParameters) {
      try {
        manifest.geometry_parameters[key] = ParseDouble(value, key);
      } catch (const std::exception&) {
        manifest.geometry_metadata[key] = value;
      }
    }
  }

  if (!manifest.IsValid()) {
    throw std::runtime_error("Invalid mesh manifest: " + path.string());
  }
  return manifest;
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
