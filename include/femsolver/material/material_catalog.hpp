#pragma once

#include <string>
#include <vector>

namespace femsolver::material {

enum class MaterialKind {
  kAir,
  kCopperCurrentDensity,
  kPermanentMagnet,
  kLinearIsotropicSteel,
  kLinearLaminatedSteel,
  kOrthotropicNonlinearSteel
};

struct MaterialDescriptor {
  std::string name;
  MaterialKind kind;
  bool uses_bh_curve;
  bool requires_consistent_tangent;
};

std::vector<MaterialDescriptor> BootstrapMaterials();
bool UsesBhCurve(MaterialKind kind);
std::string ToString(MaterialKind kind);

}  // namespace femsolver::material
