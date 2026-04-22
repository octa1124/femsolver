#include "femsolver/mesh/gmsh_tetra_mesh_import.hpp"

#include <fstream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace femsolver::mesh {

namespace {

std::string Trim(const std::string& value) {
  const auto first = value.find_first_not_of(" \t\r\n");
  if (first == std::string::npos) {
    return "";
  }
  const auto last = value.find_last_not_of(" \t\r\n");
  return value.substr(first, last - first + 1);
}

std::string StripQuotes(const std::string& value) {
  if (value.size() >= 2U &&
      ((value.front() == '"' && value.back() == '"') ||
       (value.front() == '\'' && value.back() == '\''))) {
    return value.substr(1, value.size() - 2U);
  }
  return value;
}

void ExpectLine(std::ifstream& stream, const std::string& expected) {
  std::string line;
  if (!std::getline(stream, line) || Trim(line) != expected) {
    throw std::runtime_error("Expected line " + expected);
  }
}

std::vector<std::string> SplitWhitespace(const std::string& line) {
  std::istringstream stream(line);
  std::vector<std::string> tokens;
  std::string token;
  while (stream >> token) {
    tokens.push_back(token);
  }
  return tokens;
}

}  // namespace

std::string ImportedTetraMesh::Summary() const {
  std::ostringstream stream;
  stream << mesh.Summary() << ", region_attributes=" << region_attributes.size()
         << ", boundary_attributes=" << boundary_attributes.size();
  return stream.str();
}

ImportedTetraMesh ImportGmshMsh2TetraMesh(const std::filesystem::path& path) {
  std::ifstream stream(path);
  if (!stream) {
    throw std::runtime_error("Unable to open Gmsh mesh file: " + path.string());
  }

  std::map<int, std::pair<int, std::string>> physical_names;
  std::map<int, int> node_id_to_index;
  std::vector<kernel::mesh::Point3D> nodes;
  std::vector<kernel::mesh::TetraCell> cells;
  std::vector<kernel::mesh::BoundaryFace> boundary_faces;

  std::string line;
  while (std::getline(stream, line)) {
    const std::string trimmed = Trim(line);
    if (trimmed.empty()) {
      continue;
    }

    if (trimmed == "$MeshFormat") {
      if (!std::getline(stream, line)) {
        throw std::runtime_error("Unexpected end of file in $MeshFormat");
      }
      const auto tokens = SplitWhitespace(line);
      if (tokens.size() < 3U || tokens[0] != "2.2" || tokens[1] != "0") {
        throw std::runtime_error("Only ASCII Gmsh MSH 2.2 is supported");
      }
      ExpectLine(stream, "$EndMeshFormat");
      continue;
    }

    if (trimmed == "$PhysicalNames") {
      if (!std::getline(stream, line)) {
        throw std::runtime_error("Unexpected end of file in $PhysicalNames");
      }
      const int count = std::stoi(Trim(line));
      for (int index = 0; index < count; ++index) {
        if (!std::getline(stream, line)) {
          throw std::runtime_error("Unexpected end of file in physical names");
        }
        const auto tokens = SplitWhitespace(line);
        if (tokens.size() < 3U) {
          throw std::runtime_error("Invalid physical name entry: " + line);
        }
        const int dimension = std::stoi(tokens[0]);
        const int physical_tag = std::stoi(tokens[1]);
        const auto name_start = line.find(tokens[2]);
        physical_names[physical_tag] = {dimension, StripQuotes(line.substr(name_start))};
      }
      ExpectLine(stream, "$EndPhysicalNames");
      continue;
    }

    if (trimmed == "$Nodes") {
      if (!std::getline(stream, line)) {
        throw std::runtime_error("Unexpected end of file in $Nodes");
      }
      const int node_count = std::stoi(Trim(line));
      nodes.clear();
      nodes.reserve(static_cast<std::size_t>(node_count));
      node_id_to_index.clear();
      for (int index = 0; index < node_count; ++index) {
        if (!std::getline(stream, line)) {
          throw std::runtime_error("Unexpected end of file in node list");
        }
        const auto tokens = SplitWhitespace(line);
        if (tokens.size() != 4U) {
          throw std::runtime_error("Invalid node entry: " + line);
        }
        const int node_id = std::stoi(tokens[0]);
        if (node_id_to_index.find(node_id) != node_id_to_index.end()) {
          throw std::runtime_error("Duplicate node id in Gmsh mesh: " + std::to_string(node_id));
        }
        node_id_to_index[node_id] = static_cast<int>(nodes.size());
        nodes.push_back({
            std::stod(tokens[1]),
            std::stod(tokens[2]),
            std::stod(tokens[3]),
        });
      }
      ExpectLine(stream, "$EndNodes");
      continue;
    }

    if (trimmed == "$Elements") {
      if (!std::getline(stream, line)) {
        throw std::runtime_error("Unexpected end of file in $Elements");
      }
      const int element_count = std::stoi(Trim(line));
      for (int index = 0; index < element_count; ++index) {
        if (!std::getline(stream, line)) {
          throw std::runtime_error("Unexpected end of file in element list");
        }
        const auto tokens = SplitWhitespace(line);
        if (tokens.size() < 4U) {
          throw std::runtime_error("Invalid element entry: " + line);
        }
        const int element_type = std::stoi(tokens[1]);
        const int tag_count = std::stoi(tokens[2]);
        if (tokens.size() < static_cast<std::size_t>(3 + tag_count + 1)) {
          throw std::runtime_error("Element entry has too few tokens: " + line);
        }

        const int physical_tag = tag_count >= 1 ? std::stoi(tokens[3]) : 0;
        const std::size_t nodes_offset = static_cast<std::size_t>(3 + tag_count);
        auto lookup_node_index = [&node_id_to_index](const std::string& token) -> int {
          const int node_id = std::stoi(token);
          const auto iterator = node_id_to_index.find(node_id);
          if (iterator == node_id_to_index.end()) {
            throw std::runtime_error("Element references unknown node id: " + std::to_string(node_id));
          }
          return iterator->second;
        };

        if (element_type == 2) {
          if (tokens.size() != nodes_offset + 3U) {
            throw std::runtime_error("Triangle element must have 3 nodes");
          }
          if (physical_tag > 0) {
            boundary_faces.push_back({{
                                         lookup_node_index(tokens[nodes_offset + 0]),
                                         lookup_node_index(tokens[nodes_offset + 1]),
                                         lookup_node_index(tokens[nodes_offset + 2]),
                                     },
                                     physical_tag});
          }
        } else if (element_type == 4) {
          if (tokens.size() != nodes_offset + 4U) {
            throw std::runtime_error("Tetrahedron element must have 4 nodes");
          }
          if (physical_tag <= 0) {
            throw std::runtime_error("Tetrahedron element is missing a physical tag");
          }
          cells.push_back({{
                               lookup_node_index(tokens[nodes_offset + 0]),
                               lookup_node_index(tokens[nodes_offset + 1]),
                               lookup_node_index(tokens[nodes_offset + 2]),
                               lookup_node_index(tokens[nodes_offset + 3]),
                           },
                           physical_tag});
        }
      }
      ExpectLine(stream, "$EndElements");
      continue;
    }
  }

  if (nodes.empty() || cells.empty()) {
    throw std::runtime_error("Imported Gmsh mesh does not contain tetrahedral volume data");
  }

  ImportedTetraMesh result{
      kernel::mesh::TetraMesh(std::move(nodes), std::move(cells), std::move(boundary_faces)), {}, {}};

  for (const auto& [physical_tag, descriptor] : physical_names) {
    const int dimension = descriptor.first;
    const std::string& name = descriptor.second;
    if (dimension == 3) {
      result.region_attributes[name] = physical_tag;
    } else if (dimension == 2) {
      result.boundary_attributes[name] = physical_tag;
    }
  }

  return result;
}

}  // namespace femsolver::mesh
