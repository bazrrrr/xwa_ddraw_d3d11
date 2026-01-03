cmake_minimum_required (VERSION 3.10)

project("impl11")

# tests can only work if the dll and binary are in the same directory.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" CACHE PATH "")
enable_testing()
add_subdirectory(shaders)
add_subdirectory(ddraw)
add_subdirectory(tests)
install(FILES ../LICENSE.txt ../readme.txt DESTINATION .)
