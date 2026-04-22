#include <filesystem>
#include <fstream>

#include <gtest/gtest.h>

#include "femsolver/case/case_spec.hpp"

TEST(CaseSpecTest, BootstrapCaseHasExpectedDefaults) {
  const auto spec = femsolver::case_config::CaseSpec::Bootstrap();
  EXPECT_EQ(spec.case_id, "joint_type_i_12s10p");
  EXPECT_EQ(spec.source_kind, "paper");
  EXPECT_EQ(spec.geometry_variants.size(), 2U);
  EXPECT_TRUE(spec.SupportsVariant("section_2d"));
  EXPECT_FALSE(spec.SupportsVariant("full_3d"));
  EXPECT_NE(spec.Summary().find("materials_profile={"), std::string::npos);
}

TEST(CaseSpecTest, LoadsCurrentCaseYamlSchemaFromFile) {
  const auto temp_path = std::filesystem::temp_directory_path() / "femsolver_case_spec_test.yaml";
  std::ofstream(temp_path)
      << "case_id: demo_case\n"
      << "source_kind: paper\n"
      << "source_url: https://example.com/case\n"
      << "reconstruction_level: simplified_reconstruction\n"
      << "geometry_variants:\n"
      << "  - section_2d\n"
      << "  - extruded_3d\n"
      << "cell_type: tetrahedron\n"
      << "space_family: nedelec\n"
      << "polynomial_order: 1\n"
      << "materials_profile:\n"
      << "  air: air_placeholder\n"
      << "excitation_profile:\n"
      << "  current_density_model: constant_demo\n"
      << "validation_targets:\n"
      << "  - preprocessing\n"
      << "geometry_parameters:\n"
      << "  outer_diameter_mm: 90.0\n";

  const auto spec = femsolver::case_config::CaseSpec::LoadFromFile(temp_path);

  EXPECT_EQ(spec.case_id, "demo_case");
  EXPECT_TRUE(spec.IsValid());
  EXPECT_TRUE(spec.SupportsVariant("extruded_3d"));
  EXPECT_EQ(spec.geometry_parameters.at("outer_diameter_mm"), 90.0);
}
