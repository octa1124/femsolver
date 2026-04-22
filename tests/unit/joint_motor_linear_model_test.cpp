#include <gtest/gtest.h>

#include "femsolver/case/case_spec.hpp"
#include "femsolver/solver/joint_motor_linear_model.hpp"

TEST(JointMotorLinearModelTest, BuildsLinearSmokeProfileFromBootstrapCase) {
  const auto spec = femsolver::case_config::CaseSpec::Bootstrap();

  const auto model = femsolver::solver::JointMotorLinearModel::FromCaseSpec(spec);
  const auto stator = model.Region("stator_core");
  const auto magnet = model.Region("magnet_ring");

  EXPECT_TRUE(model.HasRegion("outer_air"));
  EXPECT_EQ(stator.material_kind, femsolver::material::MaterialKind::kLinearLaminatedSteel);
  EXPECT_DOUBLE_EQ(stator.reluctivity, 6.0);
  EXPECT_DOUBLE_EQ(stator.current_density[2], 1.0);
  EXPECT_EQ(magnet.material_kind, femsolver::material::MaterialKind::kPermanentMagnet);
  EXPECT_DOUBLE_EQ(magnet.remanent_flux_density[2], 0.8);
  EXPECT_FALSE(model.warnings().empty());
}

TEST(JointMotorLinearModelTest, RejectsEnvelopeOnlyProfilesForSmokeSolve) {
  auto spec = femsolver::case_config::CaseSpec::Bootstrap();
  spec.materials_profile["profile_id"] = "industrial_envelope_placeholder";
  spec.excitation_profile["profile_id"] = "envelope_only_placeholder";
  spec.excitation_profile["current_density_model"] = "not_declared";
  spec.excitation_profile["magnetization_model"] = "not_declared";

  EXPECT_THROW(femsolver::solver::JointMotorLinearModel::FromCaseSpec(spec),
               std::invalid_argument);
}
