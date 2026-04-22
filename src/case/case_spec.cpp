#include "femsolver/case/case_spec.hpp"

#include <map>
#include <sstream>

namespace femsolver::case_config {

namespace {

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
          {"preprocessing", "geometry_summary", "future_em_smoke"}};
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
         << ", validation_targets=[" << Join(validation_targets) << "]";
  return stream.str();
}

}  // namespace femsolver::case_config
