cmake_minimum_required (VERSION 3.10)

project("impl11")

# tests can only work if the dll and binary are in the same directory.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" CACHE PATH "")
enable_testing()
if (MSVC)
  add_compile_options(/arch:SSE2)
else()
  add_compile_options(-msse -msse2)
  add_compile_options(-Wall -Wextra -pedantic -Wno-unused-parameter -Wno-switch -Wno-missing-field-initializers -Wno-unknown-pragmas)
  add_compile_definitions(D3D11_MIN_DEPTH=0.0f D3D11_MAX_DEPTH=1.0f D3DCOLORVALUE_DEFINED)
endif()
add_subdirectory(shaders)
add_subdirectory(ddraw)
add_subdirectory(tests)
install(FILES ../LICENSE.txt ../readme.txt DESTINATION .)
