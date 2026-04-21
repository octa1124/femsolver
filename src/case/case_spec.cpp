#include "femsolver/case/case_spec.hpp"

#include <sstream>

namespace femsolver::case_config {

namespace {

std::string Join(const std::vector<std::string>& values) {
  std::ostringstream stream;
  for (std::size_t index = 0; index < values.size(); ++index) {
    if (index > 0U) {
      stream << ", ";
    }
    stream << values[index];
  }
  return stream.str();
}

}  // namespace

CaseSpec CaseSpec::Bootstrap() {
  return {"bootstrap-ipm-placeholder",
          "cases/machines/bootstrap/full_model.msh",
          false,
          {"vtk", "probes", "summary"}};
}

std::string CaseSpec::Summary() const {
  std::ostringstream stream;
  stream << "machine=" << machine_name << ", mesh=" << mesh_path
         << ", periodic_sector=" << (periodic_sector ? "true" : "false")
         << ", outputs=[" << Join(requested_outputs) << "]";
  return stream.str();
}

}  // namespace femsolver::case_config
