include(FetchContent)

set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.tar.gz)
FetchContent_MakeAvailable(googletest)

add_executable(
  femsolver_unit_tests
  unit/case_spec_test.cpp
  unit/excitation_def_test.cpp
  unit/kernel_discretization_descriptor_test.cpp
  unit/kernel_curl_curl_benchmark_test.cpp
  unit/kernel_element_geometry_test.cpp
  unit/kernel_linear_magnetostatic_assembler_test.cpp
  unit/kernel_nedelec_basis_test.cpp
  unit/kernel_poisson_assembly_test.cpp
  unit/kernel_poisson_benchmark_test.cpp
  unit/kernel_quadrature_basis_test.cpp
  unit/kernel_reference_tetrahedron_test.cpp
  unit/kernel_sparse_matrix_test.cpp
  unit/kernel_space_dof_map_test.cpp
  unit/kernel_tetra_mesh_test.cpp
  unit/material_catalog_test.cpp
  unit/mesh_manifest_test.cpp
  unit/nonlinear_policy_test.cpp
  unit/project_status_test.cpp
  unit/joint_motor_linear_smoke_test.cpp
  unit/solution_bundle_test.cpp)

target_link_libraries(femsolver_unit_tests PRIVATE femsolver_core GTest::gtest_main)
femsolver_apply_target_defaults(femsolver_unit_tests)

include(GoogleTest)
gtest_discover_tests(femsolver_unit_tests)

add_test(NAME motor_pre_help COMMAND motor_pre --help)
add_test(NAME motor_solve_help COMMAND motor_solve --help)
add_test(NAME motor_check_help COMMAND motor_check --help)
add_test(NAME motor_check_poisson_benchmark COMMAND motor_check --poisson-benchmark)
add_test(NAME motor_check_curl_curl_benchmark COMMAND motor_check --curl-curl-benchmark)
add_test(
  NAME motor_pre_joint_section_2d
  COMMAND motor_pre --case cases/machines/joint_type_i_12s10p/case.yaml --variant section_2d
          --output-dir ${CMAKE_BINARY_DIR}/generated/motor_pre_joint_section_2d)
set_tests_properties(motor_pre_joint_section_2d PROPERTIES WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
add_test(
  NAME motor_pre_tbm_extruded_3d
  COMMAND motor_pre --case cases/machines/tbm76_envelope/case.yaml --variant extruded_3d
          --output-dir ${CMAKE_BINARY_DIR}/generated/motor_pre_tbm_extruded_3d)
set_tests_properties(motor_pre_tbm_extruded_3d PROPERTIES WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
add_test(
  NAME motor_pre_joint_extruded_3d
  COMMAND motor_pre --case cases/machines/joint_type_i_12s10p/case.yaml --variant extruded_3d
          --output-dir ${CMAKE_BINARY_DIR}/generated/motor_pre_joint_extruded_3d)
set_tests_properties(
  motor_pre_joint_extruded_3d
  PROPERTIES WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} FIXTURES_SETUP joint_motor_manifest)
add_test(
  NAME motor_solve_joint_linear_smoke
  COMMAND motor_solve --case cases/machines/joint_type_i_12s10p/case.yaml --manifest
          ${CMAKE_BINARY_DIR}/generated/motor_pre_joint_extruded_3d/mesh_manifest.yaml)
set_tests_properties(
  motor_solve_joint_linear_smoke
  PROPERTIES WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} FIXTURES_REQUIRED joint_motor_manifest)

find_package(Python3 COMPONENTS Interpreter REQUIRED)
execute_process(
  COMMAND ${Python3_EXECUTABLE} -c "import pytest"
  RESULT_VARIABLE FEMSOLVER_PYTEST_STATUS
  OUTPUT_QUIET
  ERROR_QUIET)

if(FEMSOLVER_PYTEST_STATUS EQUAL 0)
  add_test(
    NAME python_tooling_tests
    COMMAND ${Python3_EXECUTABLE} -m pytest ${CMAKE_SOURCE_DIR}/tests/python)
else()
  message(STATUS "pytest not found; skipping python_tooling_tests in local CTest runs.")
endif()
