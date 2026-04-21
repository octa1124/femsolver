#include "femsolver/excitation/excitation_def.hpp"

#include <sstream>

namespace femsolver::excitation {

bool ExcitationDef::IsConfigured() const {
  return !region_name.empty() && magnitude > 0.0;
}

std::string ExcitationDef::Summary() const {
  std::ostringstream stream;
  stream << "kind=" << ToString(kind) << ", region=" << region_name << ", magnitude=" << magnitude
         << ", local_frame=" << (defined_in_local_frame ? "true" : "false");
  return stream.str();
}

std::string ToString(const ExcitationKind kind) {
  switch (kind) {
    case ExcitationKind::kCurrentDensity:
      return "current-density";
    case ExcitationKind::kRemanentFluxDensity:
      return "remanent-flux-density";
    case ExcitationKind::kMagnetization:
      return "magnetization";
  }
  return "unknown";
}

}  // namespace femsolver::excitation
