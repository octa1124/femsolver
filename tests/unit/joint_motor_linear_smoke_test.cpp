#include <algorithm>
#include <filesystem>
#include <fstream>

#include <gtest/gtest.h>

#include "femsolver/case/case_spec.hpp"
#include "femsolver/mesh/mesh_manifest.hpp"
#include "femsolver/solver/joint_motor_linear_smoke.hpp"

namespace {

std::string MinimalJointMotorMsh2Fixture() {
  return R"($MeshFormat
2.2 0 8
$EndMeshFormat
$PhysicalNames
10
3 1 "outer_air"
3 2 "stator_core"
3 3 "magnet_ring"
3 4 "rotor_core"
3 5 "airgap"
3 6 "shaft"
2 11 "outer_air_boundary"
2 12 "shaft_boundary"
2 13 "axial_minus"
2 14 "axial_plus"
$EndPhysicalNames
$Nodes
5
10 0 0 0
20 1 0 0
30 0 1 0
40 0 0 1
50 0.25 0.25 0.25
$EndNodes
$Elements
8
1 2 2 11 0 10 30 20
2 2 2 11 0 10 20 40
3 2 2 11 0 20 30 40
4 2 2 11 0 10 40 30
5 4 2 1 0 10 20 30 50
6 4 2 2 0 10 20 50 40
7 4 2 3 0 20 30 50 40
8 4 2 4 0 10 50 30 40
$EndElements
)";
}

std::filesystem::path WriteTemporaryMshFixture(const std::string& file_name) {
  const auto path = std::filesystem::temp_directory_path() / file_name;
  std::ofstream stream(path);
  stream << MinimalJointMotorMsh2Fixture();
  return path;
}

bool ContainsWarning(const femsolver::post::SolutionBundle& bundle, const std::string& warning) {
  return std::find(bundle.warnings.begin(), bundle.warnings.end(), warning) != bundle.warnings.end();
}

}  // namespace

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
  EXPECT_GT(bundle.average_flux_density_magnitude, 0.0);
  EXPECT_GT(bundle.max_flux_density_magnitude, 0.0);
  EXPECT_FALSE(bundle.warnings.empty());
}

TEST(JointMotorLinearSmokeTest, SmokeSolveImportsManifestMeshWhenAvailable) {
  const auto mesh_path = WriteTemporaryMshFixture("femsolver_joint_motor_linear_smoke_import.msh");
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
  manifest.files["mesh_msh"] = mesh_path.string();
  manifest.mesh_generated = true;
  manifest.mesh_status = "generated";
  manifest.mesh_dimension = 3;

  const auto bundle = femsolver::solver::RunJointMotorLinearSmokeSolve(spec, manifest);

  EXPECT_TRUE(bundle.linear_converged);
  EXPECT_EQ(bundle.primary_dof_count, 10);
  EXPECT_GT(bundle.magnetic_energy, 0.0);
  EXPECT_GT(bundle.average_flux_density_magnitude, 0.0);
  EXPECT_GT(bundle.max_flux_density_magnitude, 0.0);
  EXPECT_FALSE(ContainsWarning(bundle, "joint-motor-smoke-path-uses-synthetic-region-tagged-kernel-mesh"));
  EXPECT_TRUE(ContainsWarning(bundle, "joint-motor-smoke-path-uses-simplified-linear-region-models"));

  std::error_code error_code;
  std::filesystem::remove(mesh_path, error_code);
}
