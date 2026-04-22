#pragma once

#include <array>
#include <string>
#include <vector>

namespace femsolver::kernel::mesh {

struct Point3D {
  double x = 0.0;
  double y = 0.0;
  double z = 0.0;
};

struct TetraCell {
  std::array<int, 4> node_ids{};
  int region_id = 0;
};

struct BoundaryFace {
  std::array<int, 3> node_ids{};
  int boundary_id = 0;
};

// Minimal self-owned tetrahedral mesh container for the first scalar-kernel milestone.
class TetraMesh {
 public:
  TetraMesh(std::vector<Point3D> nodes,
            std::vector<TetraCell> cells,
            std::vector<BoundaryFace> boundary_faces);

  [[nodiscard]] const std::vector<Point3D>& nodes() const;
  [[nodiscard]] const std::vector<TetraCell>& cells() const;
  [[nodiscard]] const std::vector<BoundaryFace>& boundary_faces() const;

  [[nodiscard]] bool IsValid() const;
  [[nodiscard]] std::vector<int> BoundaryNodeIds() const;
  [[nodiscard]] double SignedCellVolume(int cell_index) const;
  [[nodiscard]] std::string Summary() const;

 private:
  void CanonicalizeCellOrientations();
  [[nodiscard]] bool CellIndicesAreValid(const TetraCell& cell) const;
  [[nodiscard]] bool FaceIndicesAreValid(const BoundaryFace& face) const;

  std::vector<Point3D> nodes_;
  std::vector<TetraCell> cells_;
  std::vector<BoundaryFace> boundary_faces_;
};

// First canonical mesh: one reference tetrahedron refined by its centroid.
TetraMesh BuildCentroidRefinedReferenceTetraMesh();

}  // namespace femsolver::kernel::mesh
