#include "femsolver/solver/joint_motor_linear_smoke.hpp"

#include <algorithm>
#include <cmath>
#include <map>
#include <stdexcept>

#include "femsolver/kernel/algebra/conjugate_gradient.hpp"
#include "femsolver/kernel/assembly/linear_magnetostatic_assembler.hpp"
#include "femsolver/kernel/mesh/tetra_mesh.hpp"
#include "femsolver/kernel/quadrature/tetrahedron_quadrature.hpp"
#include "femsolver/kernel/space/tetra_dof_map.hpp"

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

int RegionId(const mesh::MeshManifest& manifest, const std::string& name, const int fallback) {
  const auto iterator = manifest.region_attributes.find(name);
  if (iterator != manifest.region_attributes.end()) {
    return iterator->second;
  }
  return fallback;
}

kernel::mesh::TetraMesh BuildJointMotorSmokeMesh(const mesh::MeshManifest& manifest) {
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
      {{0, 1, 2, 4}, RegionId(manifest, "outer_air", 1)},
      {{0, 1, 4, 3}, RegionId(manifest, "stator_core", 2)},
      {{1, 2, 4, 3}, RegionId(manifest, "magnet_ring", 3)},
      {{0, 4, 2, 3}, RegionId(manifest, "rotor_core", 4)},
  };

  std::vector<BoundaryFace> boundary_faces = {
      {{0, 2, 1}, 1},
      {{0, 1, 3}, 1},
      {{1, 2, 3}, 1},
      {{0, 3, 2}, 1},
  };

  return TetraMesh(std::move(nodes), std::move(cells), std::move(boundary_faces));
}

kernel::assembly::LinearMagnetostaticCellData MakeCellData(const int region_id,
                                                           const mesh::MeshManifest& manifest) {
  const int outer_air = RegionId(manifest, "outer_air", 1);
  const int stator_core = RegionId(manifest, "stator_core", 2);
  const int magnet_ring = RegionId(manifest, "magnet_ring", 3);
  const int rotor_core = RegionId(manifest, "rotor_core", 4);
  const int airgap = RegionId(manifest, "airgap", 5);
  const int shaft = RegionId(manifest, "shaft", 6);

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
  if (!mesh_manifest.HasRegion("stator_core") || !mesh_manifest.HasRegion("magnet_ring") ||
      !mesh_manifest.HasRegion("rotor_core")) {
    throw std::invalid_argument(
        "Joint motor smoke solve requires stator_core, magnet_ring, and rotor_core regions");
  }
  if (case_spec.space_family != "nedelec") {
    throw std::invalid_argument("Joint motor smoke solve currently expects a nedelec case");
  }

  const auto tetra_mesh = BuildJointMotorSmokeMesh(mesh_manifest);
  const auto dof_map = kernel::space::BuildFirstOrderNedelecDofMap(tetra_mesh);
  auto system = kernel::assembly::AssembleLinearMagnetostaticSystem(
      tetra_mesh, dof_map, kernel::quadrature::MakeCentroidTetrahedronQuadrature(),
      [&mesh_manifest](const kernel::mesh::TetraCell& cell, const int) {
        return MakeCellData(cell.region_id, mesh_manifest);
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
  bundle.torque_estimate = 0.0;
  if (!mesh_manifest.mesh_generated) {
    bundle.warnings.push_back("manifest-mesh-not-imported-using-synthetic-smoke-mesh");
  }
  bundle.warnings.push_back("joint-motor-smoke-path-uses-synthetic-region-tagged-kernel-mesh");
  if (std::abs(bundle.magnetic_energy) <= 1.0e-14) {
    bundle.warnings.push_back("smoke-energy-is-near-zero");
  }
  return bundle;
}

}  // namespace femsolver::solver
