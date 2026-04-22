#include <filesystem>
#include <fstream>

#include <gtest/gtest.h>

#include "femsolver/mesh/gmsh_tetra_mesh_import.hpp"

namespace {

std::string MinimalImportedMsh2Fixture() {
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

std::filesystem::path WriteTemporaryImportedMesh() {
  const auto path = std::filesystem::temp_directory_path() / "femsolver_gmsh_import_fixture.msh";
  std::ofstream stream(path);
  stream << MinimalImportedMsh2Fixture();
  return path;
}

}  // namespace

TEST(GmshTetraMeshImportTest, ImportsMsh2TetraMeshWithNamedPhysicalTags) {
  const auto path = WriteTemporaryImportedMesh();

  const auto imported = femsolver::mesh::ImportGmshMsh2TetraMesh(path);

  EXPECT_TRUE(imported.mesh.IsValid());
  EXPECT_EQ(imported.mesh.nodes().size(), 5U);
  EXPECT_EQ(imported.mesh.cells().size(), 4U);
  EXPECT_EQ(imported.mesh.boundary_faces().size(), 4U);
  EXPECT_EQ(imported.mesh.edges().size(), 10U);
  EXPECT_EQ(imported.region_attributes.at("stator_core"), 2);
  EXPECT_EQ(imported.region_attributes.at("magnet_ring"), 3);
  EXPECT_EQ(imported.boundary_attributes.at("outer_air_boundary"), 11);
  EXPECT_NE(imported.Summary().find("region_attributes=6"), std::string::npos);

  std::error_code error_code;
  std::filesystem::remove(path, error_code);
}
