#include "femsolver/material/material_catalog.hpp"

namespace femsolver::material {

std::vector<MaterialDescriptor> BootstrapMaterials() {
  return {
      {"air", MaterialKind::kAir, false, false},
      {"copper-current-density", MaterialKind::kCopperCurrentDensity, false, false},
      {"permanent-magnet", MaterialKind::kPermanentMagnet, false, false},
      {"linear-isotropic-steel", MaterialKind::kLinearIsotropicSteel, false, false},
      {"linear-laminated-steel", MaterialKind::kLinearLaminatedSteel, false, false},
      {"orthotropic-nonlinear-steel", MaterialKind::kOrthotropicNonlinearSteel, true, true}};
}

bool UsesBhCurve(const MaterialKind kind) {
  return kind == MaterialKind::kOrthotropicNonlinearSteel;
}

std::string ToString(const MaterialKind kind) {
  switch (kind) {
    case MaterialKind::kAir:
      return "air";
    case MaterialKind::kCopperCurrentDensity:
      return "copper-current-density";
    case MaterialKind::kPermanentMagnet:
      return "permanent-magnet";
    case MaterialKind::kLinearIsotropicSteel:
      return "linear-isotropic-steel";
    case MaterialKind::kLinearLaminatedSteel:
      return "linear-laminated-steel";
    case MaterialKind::kOrthotropicNonlinearSteel:
      return "orthotropic-nonlinear-steel";
  }
  return "unknown";
}

}  // namespace femsolver::material
