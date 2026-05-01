#include "femsolver/coupling/operator_adapters.hpp"

#include <cstddef>
#include <stdexcept>
#include <utility>

namespace femsolver::coupling {

namespace {

kernel::algebra::SparseMatrix EmbedBlock(const kernel::algebra::SparseMatrix& block,
                                         const int total_size,
                                         const int row_offset,
                                         const int col_offset) {
  kernel::algebra::SparseMatrixBuilder builder(total_size, total_size);
  for (int row = 0; row < block.rows(); ++row) {
    for (int entry = block.row_offsets().at(static_cast<std::size_t>(row));
         entry < block.row_offsets().at(static_cast<std::size_t>(row + 1)); ++entry) {
      builder.Add(row_offset + row,
                  col_offset + block.col_indices().at(static_cast<std::size_t>(entry)),
                  block.values().at(static_cast<std::size_t>(entry)));
    }
  }
  return builder.Build();
}

void ValidateSquareBlock(const kernel::algebra::SparseMatrix& matrix,
                         const int expected_size,
                         const std::string& label) {
  if (matrix.rows() != matrix.cols() || matrix.rows() != expected_size) {
    throw std::invalid_argument(label + " block size does not match field size");
  }
}

}  // namespace

PhysicsOperator MakeMagnetostaticOperator(MagnetostaticOperatorInput input) {
  if (input.source.empty()) {
    throw std::invalid_argument("Magnetostatic operator requires a non-empty source vector");
  }

  return PhysicsOperator(
      input.name,
      PhysicsOperatorKind::kMagnetostatic,
      {input.magnetic_field_name},
      [input = std::move(input)](const FieldState& state) {
        const auto& field = state.Field(input.magnetic_field_name);
        const int field_size = static_cast<int>(field.values.size());
        if (static_cast<int>(input.source.size()) != field_size) {
          throw std::invalid_argument("Magnetostatic source size does not match field size");
        }
        ValidateSquareBlock(input.stiffness, field_size, "Magnetostatic stiffness");

        const int total_size = state.TotalDofCount();
        const int offset = state.FieldOffset(input.magnetic_field_name);
        std::vector<double> residual(static_cast<std::size_t>(total_size), 0.0);
        const auto product = input.stiffness.Multiply(field.values);
        for (int local = 0; local < field_size; ++local) {
          residual.at(static_cast<std::size_t>(offset + local)) =
              product.at(static_cast<std::size_t>(local)) -
              input.source.at(static_cast<std::size_t>(local));
        }

        return OperatorAssembly{EmbedBlock(input.stiffness, total_size, offset, offset), residual, {}};
      });
}

PhysicsOperator MakeMagnetothermalHeatOperator(MagnetothermalHeatInput input) {
  if (input.heat_scale < 0.0 || input.thermal_sink < 0.0) {
    throw std::invalid_argument("Magnetothermal heat operator coefficients must be non-negative");
  }

  return PhysicsOperator(
      input.name,
      PhysicsOperatorKind::kMagnetoThermalCoupling,
      {input.magnetic_field_name, input.temperature_field_name},
      [input = std::move(input)](const FieldState& state) {
        const auto& magnetic_field = state.Field(input.magnetic_field_name);
        const auto& temperature_field = state.Field(input.temperature_field_name);
        const int total_size = state.TotalDofCount();
        const int magnetic_offset = state.FieldOffset(input.magnetic_field_name);
        const int temperature_offset = state.FieldOffset(input.temperature_field_name);

        double magnetic_norm_squared = 0.0;
        for (const double value : magnetic_field.values) {
          magnetic_norm_squared += value * value;
        }

        kernel::algebra::SparseMatrixBuilder builder(total_size, total_size);
        std::vector<double> residual(static_cast<std::size_t>(total_size), 0.0);
        const double heat_per_temperature_dof =
            input.heat_scale * magnetic_norm_squared /
            static_cast<double>(temperature_field.values.size());

        for (int local_t = 0; local_t < static_cast<int>(temperature_field.values.size());
             ++local_t) {
          const int row = temperature_offset + local_t;
          residual.at(static_cast<std::size_t>(row)) =
              input.thermal_sink * temperature_field.values.at(static_cast<std::size_t>(local_t)) -
              heat_per_temperature_dof;
          builder.Add(row, row, input.thermal_sink);
          for (int local_a = 0; local_a < static_cast<int>(magnetic_field.values.size());
               ++local_a) {
            builder.Add(row, magnetic_offset + local_a,
                        -2.0 * input.heat_scale *
                            magnetic_field.values.at(static_cast<std::size_t>(local_a)) /
                            static_cast<double>(temperature_field.values.size()));
          }
        }

        return OperatorAssembly{
            builder.Build(), residual, {"magnetothermal-heat-operator-uses-lumped-temperature"}};
      });
}

}  // namespace femsolver::coupling
