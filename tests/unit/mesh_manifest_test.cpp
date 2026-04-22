#include <filesystem>
#include <fstream>

#include <gtest/gtest.h>

#include "femsolver/mesh/mesh_manifest.hpp"

TEST(MeshManifestTest, ValidManifestRequiresPositiveAttributes) {
  femsolver::mesh::MeshManifest manifest;
  manifest.family = femsolver::mesh::MeshFamily::kTetrahedral;
  manifest.region_attributes = {{"air", 1}, {"rotor", 2}};
  manifest.boundary_attributes = {{"outer", 10}};
  manifest.periodic_pairs = {{"sector", 11, 12}};

  EXPECT_TRUE(manifest.IsValid());
  EXPECT_TRUE(manifest.HasRegion("rotor"));
  EXPECT_EQ(femsolver::mesh::ToString(manifest.family), "tetrahedral");
}

TEST(MeshManifestTest, InvalidWhenMissingRegions) {
  femsolver::mesh::MeshManifest manifest;
  manifest.family = femsolver::mesh::MeshFamily::kHexahedral;
  manifest.boundary_attributes = {{"outer", 10}};

  EXPECT_FALSE(manifest.IsValid());
}

TEST(MeshManifestTest, LoadsGeneratedPreprocessManifestSchemaFromFile) {
  const auto temp_path =
      std::filesystem::temp_directory_path() / "femsolver_mesh_manifest_test.yaml";
  std::ofstream(temp_path)
      << "case_id: demo_case\n"
      << "source_kind: paper\n"
      << "source_url: https://example.com/case\n"
      << "reconstruction_level: simplified_reconstruction\n"
      << "geometry_variant: extruded_3d\n"
      << "cell_type: tetrahedron\n"
      << "space_family: nedelec\n"
      << "polynomial_order: 1\n"
      << "region_names:\n"
      << "  - outer_air\n"
      << "  - magnet_ring\n"
      << "boundary_names:\n"
      << "  - outer_air_boundary\n"
      << "files:\n"
      << "  mesh_msh: /tmp/demo.msh\n"
      << "mesh_generation:\n"
      << "  generated: false\n"
      << "  status: skipped_gmsh_not_available\n"
      << "  dimension: 3\n"
      << "geometry_parameters:\n"
      << "  axial_length_mm: 20.0\n";

  const auto manifest = femsolver::mesh::MeshManifest::LoadFromFile(temp_path);

  EXPECT_TRUE(manifest.IsValid());
  EXPECT_TRUE(manifest.HasRegion("magnet_ring"));
  EXPECT_TRUE(manifest.HasBoundary("outer_air_boundary"));
  EXPECT_EQ(manifest.geometry_variant, "extruded_3d");
  EXPECT_EQ(manifest.mesh_dimension, 3);
  EXPECT_EQ(manifest.files.at("mesh_msh"), "/tmp/demo.msh");
}
