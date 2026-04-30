#include <gtest/gtest.h>

#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "femsolver/coupling/coupled_problem.hpp"

namespace {

femsolver::kernel::algebra::SparseMatrix DiagonalMatrix(const int size, const double value) {
  femsolver::kernel::algebra::SparseMatrixBuilder builder(size, size);
  for (int index = 0; index < size; ++index) {
    builder.Add(index, index, value);
  }
  return builder.Build();
}

femsolver::coupling::OperatorAssembly MakeDiagonalAssembly(
    const femsolver::coupling::FieldState& state,
    const double diagonal,
    const double residual_value,
    std::vector<std::string> warnings = {}) {
  const int size = state.TotalDofCount();
  return {DiagonalMatrix(size, diagonal),
          std::vector<double>(static_cast<std::size_t>(size), residual_value),
          std::move(warnings)};
}

}  // namespace

TEST(CouplingContractTest, FieldStateTracksNamedFieldBlocks) {
  femsolver::coupling::FieldState state;
  state.AddField({"A", femsolver::coupling::FieldKind::kMagneticVectorPotential, "Wb/m",
                  {0.1, 0.2, 0.3}});
  state.AddField({"T", femsolver::coupling::FieldKind::kTemperature, "K", {300.0}});

  EXPECT_TRUE(state.HasField("A"));
  EXPECT_TRUE(state.HasField("T"));
  EXPECT_EQ(state.TotalDofCount(), 4);
  EXPECT_NE(state.Summary().find("magnetic-vector-potential"), std::string::npos);
}

TEST(CouplingContractTest, PhysicsOperatorValidatesRequiredFields) {
  femsolver::coupling::FieldState state;
  state.AddField({"A", femsolver::coupling::FieldKind::kMagneticVectorPotential, "Wb/m",
                  {0.1, 0.2}});

  femsolver::coupling::PhysicsOperator thermal_operator(
      "thermal",
      femsolver::coupling::PhysicsOperatorKind::kThermal,
      {"T"},
      [](const femsolver::coupling::FieldState& field_state) {
        return MakeDiagonalAssembly(field_state, 1.0, 0.0);
      });

  EXPECT_FALSE(thermal_operator.CanAssemble(state));
  EXPECT_THROW(
      {
        const auto ignored = thermal_operator.Assemble(state);
        static_cast<void>(ignored);
      },
      std::invalid_argument);
}

TEST(CouplingContractTest, CoupledProblemAssemblesMultipleOperatorsWithoutInheritance) {
  femsolver::coupling::FieldState state;
  state.AddField({"A", femsolver::coupling::FieldKind::kMagneticVectorPotential, "Wb/m",
                  {0.1, 0.2}});
  state.AddField({"T", femsolver::coupling::FieldKind::kTemperature, "K", {300.0}});

  femsolver::coupling::CoupledProblem problem(std::move(state));
  problem.AddOperator(femsolver::coupling::PhysicsOperator(
      "magnetostatic",
      femsolver::coupling::PhysicsOperatorKind::kMagnetostatic,
      {"A"},
      [](const femsolver::coupling::FieldState& field_state) {
        return MakeDiagonalAssembly(field_state, 2.0, 1.0);
      }));
  problem.AddOperator(femsolver::coupling::PhysicsOperator(
      "magneto_thermal_exchange",
      femsolver::coupling::PhysicsOperatorKind::kMagnetoThermalCoupling,
      {"A", "T"},
      [](const femsolver::coupling::FieldState& field_state) {
        return MakeDiagonalAssembly(field_state, 0.5, -0.25, {"coupling-smoke"});
      }));

  const auto assembly = problem.Assemble();

  ASSERT_TRUE(assembly.IsDimensionallyConsistent());
  EXPECT_EQ(assembly.matrix.rows(), 3);
  EXPECT_NEAR(assembly.matrix.Entry(0, 0), 2.5, 1.0e-12);
  EXPECT_NEAR(assembly.residual.at(0), 0.75, 1.0e-12);
  EXPECT_EQ(assembly.warnings.size(), 1U);
  EXPECT_NE(problem.Summary().find("operators=2"), std::string::npos);
}
