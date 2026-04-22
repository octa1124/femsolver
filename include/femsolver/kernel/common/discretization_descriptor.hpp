#pragma once

#include <string>

namespace femsolver::kernel::common {

enum class CellType {
  kTetrahedron,
  kHexahedron,
};

enum class SpaceFamily {
  kH1Nodal,
  kNedelec,
  kRaviartThomas,
};

inline std::string ToString(CellType cell_type);
inline std::string ToString(SpaceFamily space_family);

struct PolynomialOrder {
  int value = 1;

  [[nodiscard]] bool IsValid() const {
    return value >= 1;
  }
};

struct ElementDescriptor {
  CellType cell_type = CellType::kTetrahedron;
  SpaceFamily space_family = SpaceFamily::kH1Nodal;
  PolynomialOrder order{};

  [[nodiscard]] bool IsValid() const {
    return order.IsValid();
  }

  [[nodiscard]] std::string Summary() const {
    return ToString(cell_type) + ":" + ToString(space_family) + ":p" +
           std::to_string(order.value);
  }
};

inline std::string ToString(const CellType cell_type) {
  switch (cell_type) {
    case CellType::kTetrahedron:
      return "tetrahedron";
    case CellType::kHexahedron:
      return "hexahedron";
  }
  return "unknown-cell";
}

inline std::string ToString(const SpaceFamily space_family) {
  switch (space_family) {
    case SpaceFamily::kH1Nodal:
      return "h1-nodal";
    case SpaceFamily::kNedelec:
      return "nedelec";
    case SpaceFamily::kRaviartThomas:
      return "raviart-thomas";
  }
  return "unknown-space";
}

}  // namespace femsolver::kernel::common
