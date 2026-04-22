#pragma once

#include <map>
#include <string>
#include <vector>

#include "femsolver/case/case_spec.hpp"
#include "femsolver/excitation/excitation_def.hpp"
#include "femsolver/kernel/assembly/linear_magnetostatic_assembler.hpp"
#include "femsolver/material/material_catalog.hpp"

namespace femsolver::solver {

struct JointMotorRegionLinearModel {
  std::string region_name;
  material::MaterialKind material_kind = material::MaterialKind::kAir;
  double reluctivity = 1.0;
  kernel::assembly::Vector3 current_density{0.0, 0.0, 0.0};
  kernel::assembly::Vector3 remanent_flux_density{0.0, 0.0, 0.0};
  std::vector<excitation::ExcitationDef> excitation_descriptors;
};

class JointMotorLinearModel {
 public:
  static JointMotorLinearModel FromCaseSpec(const case_config::CaseSpec& case_spec);

  [[nodiscard]] bool HasRegion(const std::string& region_name) const;
  JointMotorRegionLinearModel& Region(const std::string& region_name);
  [[nodiscard]] const JointMotorRegionLinearModel& Region(
      const std::string& region_name) const;
  [[nodiscard]] kernel::assembly::LinearMagnetostaticCellData CellDataForRegion(
      const std::string& region_name) const;
  [[nodiscard]] const std::vector<std::string>& warnings() const;
  [[nodiscard]] std::string Summary() const;

 private:
  std::map<std::string, JointMotorRegionLinearModel> regions_;
  std::vector<std::string> warnings_;
};

}  // namespace femsolver::solver
