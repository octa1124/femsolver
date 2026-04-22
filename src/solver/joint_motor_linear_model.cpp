#include "femsolver/solver/joint_motor_linear_model.hpp"

#include <sstream>
#include <stdexcept>

namespace femsolver::solver {

namespace {

std::string RequireField(const std::map<std::string, std::string>& values, const std::string& key) {
  const auto iterator = values.find(key);
  if (iterator == values.end() || iterator->second.empty()) {
    throw std::invalid_argument("Joint motor linear model requires " + key + " in case metadata");
  }
  return iterator->second;
}

JointMotorRegionLinearModel MakeRegion(const std::string& region_name,
                                       const material::MaterialKind material_kind,
                                       const double reluctivity) {
  JointMotorRegionLinearModel region;
  region.region_name = region_name;
  region.material_kind = material_kind;
  region.reluctivity = reluctivity;
  return region;
}

void AddBalancedThreePhasePlaceholder(JointMotorLinearModel* model) {
  auto& stator_core = model->Region("stator_core");
  stator_core.current_density = {0.0, 0.0, 1.0};
  stator_core.excitation_descriptors.push_back({excitation::ExcitationKind::kCurrentDensity,
                                                "stator_core",
                                                1.0,
                                                false});
}

void AddRadialPmPlaceholder(JointMotorLinearModel* model) {
  auto& magnet_ring = model->Region("magnet_ring");
  magnet_ring.remanent_flux_density = {0.0, 0.0, 0.8};
  magnet_ring.excitation_descriptors.push_back({excitation::ExcitationKind::kRemanentFluxDensity,
                                                "magnet_ring",
                                                0.8,
                                                false});
}

}  // namespace

JointMotorLinearModel JointMotorLinearModel::FromCaseSpec(const case_config::CaseSpec& case_spec) {
  if (!case_spec.IsValid()) {
    throw std::invalid_argument("Joint motor linear model requires a valid case specification");
  }

  JointMotorLinearModel model;
  const std::string material_profile = RequireField(case_spec.materials_profile, "profile_id");
  const std::string excitation_profile = RequireField(case_spec.excitation_profile, "profile_id");
  const std::string current_density_model =
      RequireField(case_spec.excitation_profile, "current_density_model");
  const std::string magnetization_model =
      RequireField(case_spec.excitation_profile, "magnetization_model");

  if (material_profile == "industrial_envelope_placeholder" ||
      excitation_profile == "envelope_only_placeholder") {
    throw std::invalid_argument(
        "The current joint motor linear smoke path does not support envelope-only case profiles");
  }

  if (material_profile == "simplified_surface_pm_joint_motor" ||
      material_profile == "simplified_outer_rotor_joint_motor" ||
      material_profile == "simplified_quadruped_joint_motor") {
    model.regions_.emplace("outer_air",
                           MakeRegion("outer_air", material::MaterialKind::kAir, 1.0));
    model.regions_.emplace("airgap", MakeRegion("airgap", material::MaterialKind::kAir, 1.0));
    model.regions_.emplace("stator_core", MakeRegion("stator_core",
                                                     material::MaterialKind::kLinearLaminatedSteel,
                                                     6.0));
    model.regions_.emplace("magnet_ring",
                           MakeRegion("magnet_ring", material::MaterialKind::kPermanentMagnet, 1.15));
    model.regions_.emplace("rotor_core", MakeRegion("rotor_core",
                                                    material::MaterialKind::kLinearLaminatedSteel,
                                                    5.0));
    model.regions_.emplace(
        "shaft", MakeRegion("shaft", material::MaterialKind::kLinearIsotropicSteel, 5.0));
    if (material_profile != "simplified_surface_pm_joint_motor") {
      model.warnings_.push_back(
          "machine-specific-material-profile-currently-reuses-shared-joint-motor-smoke-constants");
    }
  } else {
    throw std::invalid_argument("Unsupported joint motor material profile: " + material_profile);
  }

  if (current_density_model == "balanced_three_phase_placeholder") {
    AddBalancedThreePhasePlaceholder(&model);
    model.warnings_.push_back(
        "balanced-three-phase-placeholder-is-collapsed-to-uniform-stator-core-current-density");
  } else if (current_density_model != "not_declared") {
    throw std::invalid_argument("Unsupported current-density model: " + current_density_model);
  }

  if (magnetization_model == "radial_pm_placeholder" ||
      magnetization_model == "outer_rotor_radial_pm_placeholder") {
    AddRadialPmPlaceholder(&model);
    model.warnings_.push_back(
        "radial-pm-placeholder-is-collapsed-to-axial-remanent-flux-density");
  } else if (magnetization_model != "not_declared") {
    throw std::invalid_argument("Unsupported magnetization model: " + magnetization_model);
  }

  return model;
}

bool JointMotorLinearModel::HasRegion(const std::string& region_name) const {
  return regions_.find(region_name) != regions_.end();
}

JointMotorRegionLinearModel& JointMotorLinearModel::Region(const std::string& region_name) {
  return regions_.at(region_name);
}

const JointMotorRegionLinearModel& JointMotorLinearModel::Region(
    const std::string& region_name) const {
  return regions_.at(region_name);
}

kernel::assembly::LinearMagnetostaticCellData JointMotorLinearModel::CellDataForRegion(
    const std::string& region_name) const {
  const auto& region = Region(region_name);
  kernel::assembly::LinearMagnetostaticCellData cell_data;
  cell_data.reluctivity = region.reluctivity;
  cell_data.current_density = region.current_density;
  cell_data.remanent_flux_density = region.remanent_flux_density;
  return cell_data;
}

const std::vector<std::string>& JointMotorLinearModel::warnings() const {
  return warnings_;
}

std::string JointMotorLinearModel::Summary() const {
  std::ostringstream stream;
  stream << "regions=" << regions_.size() << ", warnings=" << warnings_.size();
  for (const auto& [name, region] : regions_) {
    stream << "; " << name << ":" << material::ToString(region.material_kind)
           << ", nu=" << region.reluctivity
           << ", excitations=" << region.excitation_descriptors.size();
  }
  return stream.str();
}

}  // namespace femsolver::solver
