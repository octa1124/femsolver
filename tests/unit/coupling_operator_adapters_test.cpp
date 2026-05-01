#include <gtest/gtest.h>

#include "femsolver/coupling/coupled_problem.hpp"
#include "femsolver/coupling/operator_adapters.hpp"

namespace {

femsolver::kernel::algebra::SparseMatrix MakeTwoByTwoStiffness() {
  femsolver::kernel::algebra::SparseMatrixBuilder builder(2, 2);
  builder.Add(0, 0, 3.0);
  builder.Add(0, 1, -1.0);
  builder.Add(1, 0, -1.0);
  builder.Add(1, 1, 2.0);
  return builder.Build();
}

}  // namespace

TEST(CouplingOperatorAdaptersTest, FieldStateReportsStableOffsetsInAssemblyOrder) {
  femsolver::coupling::FieldState state;
  state.AddField({"A", femsolver::coupling::FieldKind::kMagneticVectorPotential, "Wb/m",
                  {0.5, 0.25}});
  state.AddField({"T", femsolver::coupling::FieldKind::kTemperature, "K", {300.0}});

  EXPECT_EQ(state.FieldOffset("A"), 0);
  EXPECT_EQ(state.FieldSize("A"), 2);
  EXPECT_EQ(state.FieldOffset("T"), 2);
  EXPECT_EQ(state.FieldSize("T"), 1);
}

TEST(CouplingOperatorAdaptersTest, MagnetostaticOperatorEmbedsBlockAndComputesResidual) {
  femsolver::coupling::FieldState state;
  state.AddField({"A", femsolver::coupling::FieldKind::kMagneticVectorPotential, "Wb/m",
                  {0.5, 0.25}});
  state.AddField({"T", femsolver::coupling::FieldKind::kTemperature, "K", {300.0}});

  auto magnetic_operator = femsolver::coupling::MakeMagnetostaticOperator(
      {"magnetic", "A", MakeTwoByTwoStiffness(), {1.0, 0.5}});
  const auto assembly = magnetic_operator.Assemble(state);

  ASSERT_TRUE(assembly.IsDimensionallyConsistent());
  EXPECT_EQ(assembly.matrix.rows(), 3);
  EXPECT_NEAR(assembly.matrix.Entry(0, 0), 3.0, 1.0e-12);
  EXPECT_NEAR(assembly.matrix.Entry(1, 1), 2.0, 1.0e-12);
  EXPECT_NEAR(assembly.residual[0], 0.25, 1.0e-12);
  EXPECT_NEAR(assembly.residual[1], -0.5, 1.0e-12);
  EXPECT_NEAR(assembly.residual[2], 0.0, 1.0e-12);
}

TEST(CouplingOperatorAdaptersTest, MagnetothermalCanonicalProblemAssemblesCoupledResidual) {
  femsolver::coupling::FieldState state;
  state.AddField({"A", femsolver::coupling::FieldKind::kMagneticVectorPotential, "Wb/m",
                  {0.5, 0.25}});
  state.AddField({"T", femsolver::coupling::FieldKind::kTemperature, "K", {300.0}});

  femsolver::coupling::CoupledProblem problem(std::move(state));
  problem.AddOperator(femsolver::coupling::MakeMagnetostaticOperator(
      {"magnetic", "A", MakeTwoByTwoStiffness(), {1.0, 0.5}}));
  problem.AddOperator(femsolver::coupling::MakeMagnetothermalHeatOperator(
      {"heat_from_A", "A", "T", 4.0, 0.1}));

  const auto assembly = problem.Assemble();

  ASSERT_TRUE(assembly.IsDimensionallyConsistent());
  EXPECT_EQ(assembly.matrix.rows(), 3);
  EXPECT_NEAR(assembly.matrix.Entry(2, 2), 0.1, 1.0e-12);
  EXPECT_NEAR(assembly.matrix.Entry(2, 0), -4.0, 1.0e-12);
  EXPECT_NEAR(assembly.matrix.Entry(2, 1), -2.0, 1.0e-12);
  EXPECT_NEAR(assembly.residual[0], 0.25, 1.0e-12);
  EXPECT_NEAR(assembly.residual[1], -0.5, 1.0e-12);
  EXPECT_NEAR(assembly.residual[2], 28.75, 1.0e-12);
  EXPECT_EQ(assembly.warnings.size(), 1U);
}
