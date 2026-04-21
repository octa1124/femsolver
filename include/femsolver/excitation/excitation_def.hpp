#pragma once

#include <string>

namespace femsolver::excitation {

enum class ExcitationKind { kCurrentDensity, kRemanentFluxDensity, kMagnetization };

struct ExcitationDef {
  ExcitationKind kind;
  std::string region_name;
  double magnitude;
  bool defined_in_local_frame;

  [[nodiscard]] bool IsConfigured() const;
  [[nodiscard]] std::string Summary() const;
};

std::string ToString(ExcitationKind kind);

}  // namespace femsolver::excitation
