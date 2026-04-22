#include "femsolver/kernel/mesh/tetra_mesh.hpp"

#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>

namespace femsolver::kernel::mesh {

namespace {

double SignedTetraVolume(const Point3D& a,
                         const Point3D& b,
                         const Point3D& c,
                         const Point3D& d) {
  const double bax = b.x - a.x;
  const double bay = b.y - a.y;
  const double baz = b.z - a.z;
  const double cax = c.x - a.x;
  const double cay = c.y - a.y;
  const double caz = c.z - a.z;
  const double dax = d.x - a.x;
  const double day = d.y - a.y;
  const double daz = d.z - a.z;

  const double determinant = bax * (cay * daz - caz * day) - bay * (cax * daz - caz * dax) +
                             baz * (cax * day - cay * dax);
  return determinant / 6.0;
}

bool HasUniqueIndices(const std::vector<int>& indices) {
  return std::set<int>(indices.begin(), indices.end()).size() == indices.size();
}

std::array<int, 2> CanonicalEdge(const int first, const int second) {
  return first < second ? std::array<int, 2>{first, second} : std::array<int, 2>{second, first};
}

}  // namespace

TetraMesh::TetraMesh(std::vector<Point3D> nodes,
                     std::vector<TetraCell> cells,
                     std::vector<BoundaryFace> boundary_faces)
    : nodes_(std::move(nodes)),
      cells_(std::move(cells)),
      boundary_faces_(std::move(boundary_faces)) {
  CanonicalizeCellOrientations();
  BuildEdges();
}

const std::vector<Point3D>& TetraMesh::nodes() const {
  return nodes_;
}

const std::vector<TetraCell>& TetraMesh::cells() const {
  return cells_;
}

const std::vector<BoundaryFace>& TetraMesh::boundary_faces() const {
  return boundary_faces_;
}

const std::vector<TetraEdge>& TetraMesh::edges() const {
  return edges_;
}

bool TetraMesh::IsValid() const {
  if (nodes_.size() < 4 || cells_.empty()) {
    return false;
  }

  for (const auto& cell : cells_) {
    if (!CellIndicesAreValid(cell)) {
      return false;
    }

    const auto& a = nodes_.at(static_cast<std::size_t>(cell.node_ids[0]));
    const auto& b = nodes_.at(static_cast<std::size_t>(cell.node_ids[1]));
    const auto& c = nodes_.at(static_cast<std::size_t>(cell.node_ids[2]));
    const auto& d = nodes_.at(static_cast<std::size_t>(cell.node_ids[3]));
    if (std::abs(SignedTetraVolume(a, b, c, d)) <= 1.0e-14) {
      return false;
    }
  }

  for (const auto& face : boundary_faces_) {
    if (!FaceIndicesAreValid(face)) {
      return false;
    }
  }

  return true;
}

std::vector<int> TetraMesh::BoundaryNodeIds() const {
  std::set<int> node_ids;
  for (const auto& face : boundary_faces_) {
    node_ids.insert(face.node_ids.begin(), face.node_ids.end());
  }
  return {node_ids.begin(), node_ids.end()};
}

std::vector<int> TetraMesh::BoundaryEdgeIds() const {
  std::map<std::array<int, 2>, int> edge_lookup;
  for (std::size_t edge_index = 0; edge_index < edges_.size(); ++edge_index) {
    edge_lookup[edges_.at(edge_index).node_ids] = static_cast<int>(edge_index);
  }

  std::set<int> boundary_edge_ids;
  for (const auto& face : boundary_faces_) {
    for (const auto& edge : {CanonicalEdge(face.node_ids[0], face.node_ids[1]),
                             CanonicalEdge(face.node_ids[1], face.node_ids[2]),
                             CanonicalEdge(face.node_ids[2], face.node_ids[0])}) {
      boundary_edge_ids.insert(edge_lookup.at(edge));
    }
  }
  return {boundary_edge_ids.begin(), boundary_edge_ids.end()};
}

double TetraMesh::SignedCellVolume(const int cell_index) const {
  const auto& cell = cells_.at(static_cast<std::size_t>(cell_index));
  return SignedTetraVolume(nodes_.at(static_cast<std::size_t>(cell.node_ids[0])),
                           nodes_.at(static_cast<std::size_t>(cell.node_ids[1])),
                           nodes_.at(static_cast<std::size_t>(cell.node_ids[2])),
                           nodes_.at(static_cast<std::size_t>(cell.node_ids[3])));
}

std::string TetraMesh::Summary() const {
  std::ostringstream stream;
  stream << "tetra_mesh(nodes=" << nodes_.size() << ", cells=" << cells_.size()
         << ", edges=" << edges_.size() << ", boundary_faces=" << boundary_faces_.size() << ")";
  return stream.str();
}

void TetraMesh::CanonicalizeCellOrientations() {
  for (auto& cell : cells_) {
    if (!CellIndicesAreValid(cell)) {
      continue;
    }

    const double signed_volume =
        SignedTetraVolume(nodes_.at(static_cast<std::size_t>(cell.node_ids[0])),
                          nodes_.at(static_cast<std::size_t>(cell.node_ids[1])),
                          nodes_.at(static_cast<std::size_t>(cell.node_ids[2])),
                          nodes_.at(static_cast<std::size_t>(cell.node_ids[3])));
    if (signed_volume < 0.0) {
      std::swap(cell.node_ids[0], cell.node_ids[1]);
    }
  }
}

void TetraMesh::BuildEdges() {
  std::set<std::array<int, 2>> unique_edges;
  for (const auto& cell : cells_) {
    unique_edges.insert(CanonicalEdge(cell.node_ids[0], cell.node_ids[1]));
    unique_edges.insert(CanonicalEdge(cell.node_ids[0], cell.node_ids[2]));
    unique_edges.insert(CanonicalEdge(cell.node_ids[0], cell.node_ids[3]));
    unique_edges.insert(CanonicalEdge(cell.node_ids[1], cell.node_ids[2]));
    unique_edges.insert(CanonicalEdge(cell.node_ids[1], cell.node_ids[3]));
    unique_edges.insert(CanonicalEdge(cell.node_ids[2], cell.node_ids[3]));
  }

  edges_.clear();
  for (const auto& edge : unique_edges) {
    edges_.push_back(TetraEdge{edge});
  }
}

bool TetraMesh::CellIndicesAreValid(const TetraCell& cell) const {
  if (cell.region_id <= 0) {
    return false;
  }

  const std::vector<int> indices(cell.node_ids.begin(), cell.node_ids.end());
  if (!HasUniqueIndices(indices)) {
    return false;
  }

  return std::all_of(indices.begin(), indices.end(), [this](const int node_id) {
    return node_id >= 0 && static_cast<std::size_t>(node_id) < nodes_.size();
  });
}

bool TetraMesh::FaceIndicesAreValid(const BoundaryFace& face) const {
  if (face.boundary_id <= 0) {
    return false;
  }

  const std::vector<int> indices(face.node_ids.begin(), face.node_ids.end());
  if (!HasUniqueIndices(indices)) {
    return false;
  }

  return std::all_of(indices.begin(), indices.end(), [this](const int node_id) {
    return node_id >= 0 && static_cast<std::size_t>(node_id) < nodes_.size();
  });
}

TetraMesh BuildCentroidRefinedReferenceTetraMesh() {
  std::vector<Point3D> nodes = {
      {0.0, 0.0, 0.0},
      {1.0, 0.0, 0.0},
      {0.0, 1.0, 0.0},
      {0.0, 0.0, 1.0},
      {0.25, 0.25, 0.25},
  };

  std::vector<TetraCell> cells = {
      {{0, 1, 2, 4}, 1},
      {{0, 1, 4, 3}, 1},
      {{1, 2, 4, 3}, 1},
      {{0, 4, 2, 3}, 1},
  };

  std::vector<BoundaryFace> boundary_faces = {
      {{0, 2, 1}, 1},
      {{0, 1, 3}, 1},
      {{1, 2, 3}, 1},
      {{0, 3, 2}, 1},
  };

  return TetraMesh(std::move(nodes), std::move(cells), std::move(boundary_faces));
}

}  // namespace femsolver::kernel::mesh
