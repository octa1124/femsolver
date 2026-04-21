option(FEMSOLVER_ENABLE_ASAN "Enable AddressSanitizer and UndefinedBehaviorSanitizer." OFF)
option(FEMSOLVER_ENABLE_COVERAGE "Enable coverage instrumentation." OFF)

function(femsolver_print_configuration)
  message(STATUS "Configuring ${PROJECT_NAME} ${PROJECT_VERSION}")
  message(STATUS "  BUILD_TESTING=${BUILD_TESTING}")
  message(STATUS "  FEMSOLVER_ENABLE_ASAN=${FEMSOLVER_ENABLE_ASAN}")
  message(STATUS "  FEMSOLVER_ENABLE_COVERAGE=${FEMSOLVER_ENABLE_COVERAGE}")
endfunction()

function(femsolver_apply_target_defaults target_name)
  target_compile_features(${target_name} PUBLIC cxx_std_17)

  if(MSVC)
    target_compile_options(${target_name} PRIVATE /W4 /permissive-)
  else()
    target_compile_options(
      ${target_name}
      PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
        -Wsign-conversion)
  endif()

  if(FEMSOLVER_ENABLE_ASAN AND NOT MSVC)
    target_compile_options(${target_name} PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(${target_name} PRIVATE -fsanitize=address,undefined)
  endif()

  if(FEMSOLVER_ENABLE_COVERAGE AND NOT MSVC)
    target_compile_options(${target_name} PRIVATE --coverage)
    target_link_options(${target_name} PRIVATE --coverage)
  endif()
endfunction()
