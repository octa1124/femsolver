#pragma once

#include "femsolver/case/case_spec.hpp"
#include "femsolver/mesh/mesh_manifest.hpp"
#include "femsolver/post/solution_bundle.hpp"

namespace femsolver::solver {

post::SolutionBundle RunJointMotorLinearSmokeSolve(const case_config::CaseSpec& case_spec,
                                                   const mesh::MeshManifest& mesh_manifest);

}  // namespace femsolver::solver
