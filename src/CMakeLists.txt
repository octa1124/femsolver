add_library(
  femsolver_core
  case/case_spec.cpp
  excitation/excitation_def.cpp
  fem/discretization_strategy.cpp
  io/text_report.cpp
  material/material_catalog.cpp
  mesh/mesh_manifest.cpp
  nonlinear/nonlinear_policy.cpp
  post/solution_bundle.cpp
  solver/project_status.cpp)

target_include_directories(femsolver_core PUBLIC ${CMAKE_SOURCE_DIR}/include)
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
