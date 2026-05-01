#pragma once

#include <string>
#include <vector>

#include "femsolver/coupling/physics_operator.hpp"

namespace femsolver::coupling {

struct MagnetostaticOperatorInput {
  std::string name = "magnetostatic";
  std::string magnetic_field_name = "A";
  kernel::algebra::SparseMatrix stiffness;
  std::vector<double> source;
};

struct MagnetothermalHeatInput {
  std::string name = "magnetothermal_heat";
  std::string magnetic_field_name = "A";
  std::string temperature_field_name = "T";
  double heat_scale = 1.0;
  double thermal_sink = 1.0;
};

PhysicsOperator MakeMagnetostaticOperator(MagnetostaticOperatorInput input);
PhysicsOperator MakeMagnetothermalHeatOperator(MagnetothermalHeatInput input);

}  // namespace femsolver::coupling
