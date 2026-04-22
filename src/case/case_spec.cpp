#include "femsolver/case/case_spec.hpp"

#include <fstream>
#include <map>
#include <stdexcept>
#include <sstream>

namespace femsolver::case_config {

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

std::string Join(const std::vector<std::string>& values) {
  std::ostringstream stream;
  for (std::size_t index = 0; index < values.size(); ++index) {
    if (index > 0U) {
      stream << ", ";
    }
    stream << values[index];
  }
  return stream.str();
}

std::string JoinMap(const std::map<std::string, std::string>& values) {
  std::ostringstream stream;
  std::size_t index = 0;
  for (const auto& [key, value] : values) {
    if (index > 0U) {
      stream << ", ";
    }
    stream << key << "=" << value;
    ++index;
  }
  return stream.str();
}

std::string JoinNumericMap(const std::map<std::string, double>& values) {
  std::ostringstream stream;
  std::size_t index = 0;
  for (const auto& [key, value] : values) {
    if (index > 0U) {
      stream << ", ";
    }
    stream << key << "=" << value;
    ++index;
  }
  return stream.str();
}

enum class Section {
  kNone,
  kGeometryVariants,
  kMaterialsProfile,
  kExcitationProfile,
  kValidationTargets,
  kGeometryParameters,
};

}  // namespace

CaseSpec CaseSpec::Bootstrap() {
  return {"joint_type_i_12s10p",
          "paper",
          "https://www.mdpi.com/1996-1073/14/20/6690",
          "simplified_reconstruction",
          {"section_2d", "extruded_3d"},
          "tetrahedron",
          "nedelec",
          1,
          {{"profile_id", "simplified_surface_pm_joint_motor"},
           {"stator_core", "electrical_steel_placeholder"},
           {"rotor_core", "electrical_steel_placeholder"},
           {"permanent_magnet", "ndfeb_placeholder"},
           {"conductor", "copper_placeholder"}},
          {{"profile_id", "placeholder_pm_current_excitation"},
           {"magnetization_model", "radial_pm_placeholder"},
           {"current_density_model", "balanced_three_phase_placeholder"}},
          {"preprocessing", "geometry_summary", "future_em_smoke"},
          {{"slot_count", 12.0},
           {"pole_count", 10.0},
           {"outer_diameter_mm", 92.0},
           {"stator_inner_diameter_mm", 58.0},
           {"rotor_outer_diameter_mm", 56.8},
           {"rotor_inner_diameter_mm", 24.0},
           {"shaft_diameter_mm", 20.0},
           {"airgap_mm", 0.6},
           {"magnet_thickness_mm", 3.0},
           {"axial_length_mm", 28.0},
           {"air_buffer_mm", 6.0},
           {"slot_opening_mm", 2.5}}};
}

CaseSpec CaseSpec::LoadFromFile(const std::filesystem::path& path) {
  std::ifstream stream(path);
  if (!stream) {
    throw std::runtime_error("Unable to open case file: " + path.string());
  }

  CaseSpec spec;
  Section current_section = Section::kNone;
  std::string line;
  while (std::getline(stream, line)) {
    const std::size_t indent = line.find_first_not_of(' ');
    const std::string trimmed = Trim(line);
    if (trimmed.empty() || trimmed.front() == '#') {
      continue;
    }

    if (indent == 0 || indent == std::string::npos) {
      current_section = Section::kNone;
      const auto [key, raw_value] = SplitKeyValue(trimmed);
      const std::string value = StripQuotes(raw_value);
      if (raw_value.empty()) {
        if (key == "geometry_variants") {
          current_section = Section::kGeometryVariants;
        } else if (key == "materials_profile") {
          current_section = Section::kMaterialsProfile;
        } else if (key == "excitation_profile") {
          current_section = Section::kExcitationProfile;
        } else if (key == "validation_targets") {
          current_section = Section::kValidationTargets;
        } else if (key == "geometry_parameters") {
          current_section = Section::kGeometryParameters;
        }
        continue;
      }

      if (key == "case_id") {
        spec.case_id = value;
      } else if (key == "source_kind") {
        spec.source_kind = value;
      } else if (key == "source_url") {
        spec.source_url = value;
      } else if (key == "reconstruction_level") {
        spec.reconstruction_level = value;
      } else if (key == "cell_type") {
        spec.cell_type = value;
      } else if (key == "space_family") {
        spec.space_family = value;
      } else if (key == "polynomial_order") {
        spec.polynomial_order = static_cast<int>(ParseDouble(value, key));
      }
      continue;
    }

    if (current_section == Section::kGeometryVariants ||
        current_section == Section::kValidationTargets) {
      if (trimmed.rfind("- ", 0) != 0) {
        throw std::runtime_error("Expected list item in case file: " + trimmed);
      }
      auto item = StripQuotes(Trim(trimmed.substr(2)));
      if (current_section == Section::kGeometryVariants) {
        spec.geometry_variants.push_back(item);
      } else {
        spec.validation_targets.push_back(item);
      }
      continue;
    }

    const auto [key, raw_value] = SplitKeyValue(trimmed);
    const std::string value = StripQuotes(raw_value);
    if (current_section == Section::kMaterialsProfile) {
      spec.materials_profile[key] = value;
    } else if (current_section == Section::kExcitationProfile) {
      spec.excitation_profile[key] = value;
    } else if (current_section == Section::kGeometryParameters) {
      spec.geometry_parameters[key] = ParseDouble(value, key);
    }
  }

  if (!spec.IsValid()) {
    throw std::runtime_error("Invalid case file: " + path.string());
  }
  return spec;
}

bool CaseSpec::IsValid() const {
  return !case_id.empty() && !source_kind.empty() && !source_url.empty() &&
         !reconstruction_level.empty() && !geometry_variants.empty() && !cell_type.empty() &&
         !space_family.empty() && polynomial_order >= 1 && !materials_profile.empty() &&
         !excitation_profile.empty() && !validation_targets.empty() &&
         !geometry_parameters.empty();
}

bool CaseSpec::SupportsVariant(const std::string& variant) const {
  for (const auto& candidate : geometry_variants) {
    if (candidate == variant) {
      return true;
    }
  }
  return false;
}

std::string CaseSpec::Summary() const {
  std::ostringstream stream;
  stream << "case_id=" << case_id << ", source_kind=" << source_kind
         << ", reconstruction_level=" << reconstruction_level
         << ", geometry_variants=[" << Join(geometry_variants) << "]"
         << ", cell_type=" << cell_type << ", space_family=" << space_family
         << ", polynomial_order=" << polynomial_order
         << ", materials_profile={" << JoinMap(materials_profile) << "}"
         << ", excitation_profile={" << JoinMap(excitation_profile) << "}"
         << ", validation_targets=[" << Join(validation_targets) << "]"
         << ", geometry_parameters={" << JoinNumericMap(geometry_parameters) << "}";
  return stream.str();
}

}  // namespace femsolver::case_config
