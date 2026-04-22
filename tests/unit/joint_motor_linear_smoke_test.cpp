#include <gtest/gtest.h>

#include "femsolver/case/case_spec.hpp"
#include "femsolver/mesh/mesh_manifest.hpp"
#include "femsolver/solver/joint_motor_linear_smoke.hpp"

TEST(JointMotorLinearSmokeTest, SmokeSolveConvergesOnManifestTaggedSyntheticMesh) {
  auto spec = femsolver::case_config::CaseSpec::Bootstrap();

  femsolver::mesh::MeshManifest manifest;
  manifest.family = femsolver::mesh::MeshFamily::kTetrahedral;
  manifest.case_id = spec.case_id;
  manifest.source_kind = spec.source_kind;
  manifest.source_url = spec.source_url;
  manifest.reconstruction_level = spec.reconstruction_level;
  manifest.geometry_variant = "extruded_3d";
  manifest.space_family = "nedelec";
  manifest.polynomial_order = 1;
  manifest.region_names = {"outer_air", "stator_core", "airgap", "magnet_ring", "rotor_core",
                           "shaft"};
  manifest.boundary_names = {"outer_air_boundary", "shaft_boundary", "axial_minus",
                             "axial_plus"};
  manifest.files["mesh_msh"] = "synthetic";
  manifest.mesh_generated = false;
  manifest.mesh_status = "synthetic-smoke";
  manifest.mesh_dimension = 3;

  const auto bundle = femsolver::solver::RunJointMotorLinearSmokeSolve(spec, manifest);

  EXPECT_EQ(bundle.case_id, spec.case_id);
  EXPECT_TRUE(bundle.linear_converged);
  EXPECT_EQ(bundle.primary_dof_count, 10);
  EXPECT_GT(bundle.magnetic_energy, 0.0);
  EXPECT_FALSE(bundle.warnings.empty());
}
