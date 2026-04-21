#include <gtest/gtest.h>

#include "femsolver/mesh/mesh_manifest.hpp"

TEST(MeshManifestTest, ValidManifestRequiresPositiveAttributes) {
  femsolver::mesh::MeshManifest manifest{
      femsolver::mesh::MeshFamily::kTetrahedral,
      {{"air", 1}, {"rotor", 2}},
      {{"outer", 10}},
      {{"sector", 11, 12}}};

  EXPECT_TRUE(manifest.IsValid());
  EXPECT_TRUE(manifest.HasRegion("rotor"));
  EXPECT_EQ(femsolver::mesh::ToString(manifest.family), "tetrahedral");
}

TEST(MeshManifestTest, InvalidWhenMissingRegions) {
  femsolver::mesh::MeshManifest manifest{
      femsolver::mesh::MeshFamily::kHexahedral, {}, {{"outer", 10}}, {}};

  EXPECT_FALSE(manifest.IsValid());
}
