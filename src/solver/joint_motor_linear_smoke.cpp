#include "femsolver/solver/joint_motor_linear_smoke.hpp"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <map>
#include <stdexcept>

#include "femsolver/kernel/algebra/conjugate_gradient.hpp"
#include "femsolver/kernel/assembly/linear_magnetostatic_assembler.hpp"
#include "femsolver/kernel/mesh/tetra_mesh.hpp"
#include "femsolver/kernel/quadrature/tetrahedron_quadrature.hpp"
#include "femsolver/kernel/space/tetra_dof_map.hpp"
#include "femsolver/mesh/gmsh_tetra_mesh_import.hpp"
#include "femsolver/post/linear_magnetostatic_summary.hpp"

namespace femsolver::solver {

namespace {

double Dot(const std::vector<double>& a, const std::vector<double>& b) {
  if (a.size() != b.size()) {
    throw std::invalid_argument("Vector dot product dimension mismatch");
  }
  double result = 0.0;
  for (std::size_t index = 0; index < a.size(); ++index) {
    result += a[index] * b[index];
  }
  return result;
}

int AttributeId(const std::map<std::string, int>& attributes,
                const std::string& name,
                const int fallback) {
  const auto iterator = attributes.find(name);
  if (iterator != attributes.end()) {
    return iterator->second;
  }
  return fallback;
}

std::map<std::string, int> BuildSequentialAttributes(const std::vector<std::string>& names) {
  std::map<std::string, int> attributes;
  int next_attribute = 1;
  for (const auto& name : names) {
    if (!name.empty()) {
      attributes.emplace(name, next_attribute++);
    }
  }
  return attributes;
}

bool HasNamedAttribute(const std::map<std::string, int>& attributes, const std::string& name) {
  return attributes.find(name) != attributes.end();
}

kernel::mesh::TetraMesh BuildJointMotorSmokeMesh(const std::map<std::string, int>& region_attributes,
                                                 const std::map<std::string, int>& boundary_attributes) {
  using kernel::mesh::BoundaryFace;
  using kernel::mesh::Point3D;
  using kernel::mesh::TetraCell;
  using kernel::mesh::TetraMesh;

  std::vector<Point3D> nodes = {
      {0.0, 0.0, 0.0},
      {1.0, 0.0, 0.0},
      {0.0, 1.0, 0.0},
      {0.0, 0.0, 1.0},
      {0.25, 0.25, 0.25},
  };

  std::vector<TetraCell> cells = {
      {{0, 1, 2, 4}, AttributeId(region_attributes, "outer_air", 1)},
      {{0, 1, 4, 3}, AttributeId(region_attributes, "stator_core", 2)},
      {{1, 2, 4, 3}, AttributeId(region_attributes, "magnet_ring", 3)},
      {{0, 4, 2, 3}, AttributeId(region_attributes, "rotor_core", 4)},
  };

  std::vector<BoundaryFace> boundary_faces = {
      {{0, 2, 1}, AttributeId(boundary_attributes, "outer_air_boundary", 1)},
      {{0, 1, 3}, AttributeId(boundary_attributes, "outer_air_boundary", 1)},
      {{1, 2, 3}, AttributeId(boundary_attributes, "outer_air_boundary", 1)},
      {{0, 3, 2}, AttributeId(boundary_attributes, "outer_air_boundary", 1)},
  };

  return TetraMesh(std::move(nodes), std::move(cells), std::move(boundary_faces));
}

struct ResolvedSmokeMesh {
  kernel::mesh::TetraMesh tetra_mesh;
  std::map<std::string, int> region_attributes;
  std::map<std::string, int> boundary_attributes;
  std::vector<std::string> warnings;
  bool imported_from_manifest = false;
};

ResolvedSmokeMesh ResolveJointMotorSmokeMesh(const mesh::MeshManifest& manifest) {
  const std::map<std::string, int> synthetic_region_attributes =
      manifest.region_attributes.empty() ? BuildSequentialAttributes(manifest.region_names)
                                         : manifest.region_attributes;
  const std::map<std::string, int> synthetic_boundary_attributes =
      manifest.boundary_attributes.empty() ? BuildSequentialAttributes(manifest.boundary_names)
                                           : manifest.boundary_attributes;

  if (manifest.mesh_generated) {
    const auto mesh_path_iterator = manifest.files.find("mesh_msh");
    if (mesh_path_iterator != manifest.files.end() && !mesh_path_iterator->second.empty()) {
      const std::filesystem::path mesh_path = mesh_path_iterator->second;
      if (std::filesystem::exists(mesh_path)) {
        try {
          auto imported_mesh = mesh::ImportGmshMsh2TetraMesh(mesh_path);
          if (!imported_mesh.mesh.IsValid()) {
            throw std::runtime_error("Imported Gmsh tetra mesh failed validity checks");
          }
          return {std::move(imported_mesh.mesh),
                  std::move(imported_mesh.region_attributes),
                  std::move(imported_mesh.boundary_attributes),
                  {},
                  true};
        } catch (const std::exception&) {
          return {BuildJointMotorSmokeMesh(synthetic_region_attributes, synthetic_boundary_attributes),
                  synthetic_region_attributes,
                  synthetic_boundary_attributes,
                  {"manifest-mesh-import-failed-using-synthetic-smoke-mesh",
                   "joint-motor-smoke-path-uses-synthetic-region-tagged-kernel-mesh"},
                  false};
        }
      }
      return {BuildJointMotorSmokeMesh(synthetic_region_attributes, synthetic_boundary_attributes),
              synthetic_region_attributes,
              synthetic_boundary_attributes,
              {"manifest-mesh-path-missing-using-synthetic-smoke-mesh",
               "joint-motor-smoke-path-uses-synthetic-region-tagged-kernel-mesh"},
              false};
    }
  }

  std::vector<std::string> warnings;
  if (!manifest.mesh_generated) {
    warnings.push_back("manifest-mesh-not-generated-using-synthetic-smoke-mesh");
  } else {
    warnings.push_back("manifest-mesh-path-not-declared-using-synthetic-smoke-mesh");
  }
  warnings.push_back("joint-motor-smoke-path-uses-synthetic-region-tagged-kernel-mesh");
  return {BuildJointMotorSmokeMesh(synthetic_region_attributes, synthetic_boundary_attributes),
          synthetic_region_attributes,
          synthetic_boundary_attributes,
          warnings,
          false};
}

kernel::assembly::LinearMagnetostaticCellData MakeCellData(
    const int region_id,
    const std::map<std::string, int>& region_attributes) {
  const int outer_air = AttributeId(region_attributes, "outer_air", 1);
  const int stator_core = AttributeId(region_attributes, "stator_core", 2);
  const int magnet_ring = AttributeId(region_attributes, "magnet_ring", 3);
  const int rotor_core = AttributeId(region_attributes, "rotor_core", 4);
  const int airgap = AttributeId(region_attributes, "airgap", 5);
  const int shaft = AttributeId(region_attributes, "shaft", 6);

  kernel::assembly::LinearMagnetostaticCellData data;
  if (region_id == outer_air || region_id == airgap) {
    data.reluctivity = 1.0;
  } else if (region_id == stator_core) {
    data.reluctivity = 6.0;
    data.current_density = {0.0, 0.0, 1.0};
  } else if (region_id == magnet_ring) {
    data.reluctivity = 1.15;
    data.remanent_flux_density = {0.0, 0.0, 0.8};
  } else if (region_id == rotor_core || region_id == shaft) {
    data.reluctivity = 5.0;
  }
  return data;
}

}  // namespace

post::SolutionBundle RunJointMotorLinearSmokeSolve(const case_config::CaseSpec& case_spec,
                                                   const mesh::MeshManifest& mesh_manifest) {
  if (!case_spec.IsValid()) {
    throw std::invalid_argument("Joint motor smoke solve requires a valid case specification");
  }
  if (!mesh_manifest.IsValid()) {
    throw std::invalid_argument("Joint motor smoke solve requires a valid mesh manifest");
  }
  if (case_spec.space_family != "nedelec") {
    throw std::invalid_argument("Joint motor smoke solve currently expects a nedelec case");
  }

  const auto resolved_mesh = ResolveJointMotorSmokeMesh(mesh_manifest);
  if (!HasNamedAttribute(resolved_mesh.region_attributes, "stator_core") ||
      !HasNamedAttribute(resolved_mesh.region_attributes, "magnet_ring") ||
      !HasNamedAttribute(resolved_mesh.region_attributes, "rotor_core")) {
    throw std::invalid_argument(
        "Joint motor smoke solve requires stator_core, magnet_ring, and rotor_core regions");
  }

  const auto dof_map = kernel::space::BuildFirstOrderNedelecDofMap(resolved_mesh.tetra_mesh);
  auto system = kernel::assembly::AssembleLinearMagnetostaticSystem(
      resolved_mesh.tetra_mesh, dof_map, kernel::quadrature::MakeCentroidTetrahedronQuadrature(),
      [&resolved_mesh](const kernel::mesh::TetraCell& cell, const int) {
        return MakeCellData(cell.region_id, resolved_mesh.region_attributes);
      });

  const std::vector<double> zero_boundary_values(dof_map.boundary_dofs().size(), 0.0);
  kernel::assembly::ApplyEdgeDirichletBoundary(
      kernel::assembly::EdgeDirichletBoundaryCondition{dof_map.boundary_dofs(), zero_boundary_values},
      &system);

  const auto linear_system = system.Finalize();
  const auto result = kernel::algebra::SolveConjugateGradient(
      linear_system.matrix, linear_system.rhs,
      kernel::algebra::ConjugateGradientOptions{128, 1.0e-12});

  post::SolutionBundle bundle;
  bundle.case_id = case_spec.case_id;
  bundle.status = result.converged ? "linear-magnetostatic-smoke-ok"
                                   : "linear-magnetostatic-smoke-not-converged";
  bundle.primary_dof_count = static_cast<int>(result.solution.size());
  bundle.linear_iterations = result.iterations;
  bundle.residual_norm = result.residual_norm;
  bundle.linear_converged = result.converged;
  bundle.magnetic_energy = 0.5 * Dot(result.solution, linear_system.rhs);
  const auto flux_summary = post::SummarizeFirstOrderNedelecFluxDensity(
      resolved_mesh.tetra_mesh, dof_map, result.solution);
  bundle.average_flux_density_magnitude = flux_summary.average_magnitude;
  bundle.max_flux_density_magnitude = flux_summary.max_magnitude;
  bundle.torque_estimate = 0.0;
  bundle.warnings = resolved_mesh.warnings;
  bundle.warnings.push_back("joint-motor-smoke-path-uses-simplified-linear-region-models");
  if (std::abs(bundle.magnetic_energy) <= 1.0e-14) {
    bundle.warnings.push_back("smoke-energy-is-near-zero");
  }
  return bundle;
}

}  // namespace femsolver::solver
