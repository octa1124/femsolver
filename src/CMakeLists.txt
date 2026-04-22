add_library(
  femsolver_kernel
  kernel/algebra/conjugate_gradient.cpp
  kernel/algebra/sparse_matrix.cpp
  kernel/assembly/curl_curl_assembler.cpp
  kernel/assembly/element_geometry.cpp
  kernel/assembly/linear_magnetostatic_assembler.cpp
  kernel/assembly/poisson_assembler.cpp
  kernel/basis/lagrange_p1_tetrahedron.cpp
  kernel/basis/nedelec_p1_tetrahedron.cpp
  kernel/benchmark/curl_curl_benchmark.cpp
  kernel/benchmark/poisson_benchmark.cpp
  kernel/mesh/tetra_mesh.cpp
  kernel/quadrature/tetrahedron_quadrature.cpp
  kernel/reference/reference_tetrahedron.cpp
  kernel/space/tetra_dof_map.cpp)

target_include_directories(femsolver_kernel PUBLIC ${CMAKE_SOURCE_DIR}/include)
femsolver_apply_target_defaults(femsolver_kernel)

add_library(
  femsolver_core
  case/case_spec.cpp
  excitation/excitation_def.cpp
  fem/discretization_strategy.cpp
  io/text_report.cpp
  material/material_catalog.cpp
  mesh/gmsh_tetra_mesh_import.cpp
  mesh/mesh_manifest.cpp
  nonlinear/nonlinear_policy.cpp
  post/linear_magnetostatic_summary.cpp
  post/solution_bundle.cpp
  solver/joint_motor_linear_smoke.cpp
  solver/project_status.cpp)

target_include_directories(femsolver_core PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(femsolver_core PUBLIC femsolver_kernel)
femsolver_apply_target_defaults(femsolver_core)

add_executable(motor_pre app/motor_pre_main.cpp)
target_link_libraries(motor_pre PRIVATE femsolver_core)
femsolver_apply_target_defaults(motor_pre)

add_executable(motor_solve app/motor_solve_main.cpp)
target_link_libraries(motor_solve PRIVATE femsolver_core)
femsolver_apply_target_defaults(motor_solve)

add_executable(motor_check app/motor_check_main.cpp)
target_link_libraries(motor_check PRIVATE femsolver_core)
femsolver_apply_target_defaults(motor_check)
