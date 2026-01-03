add_executable(tests main.cpp)
set_property(TARGET tests PROPERTY CXX_STANDARD 20)
target_link_libraries(tests ddraw)
add_test(NAME tests COMMAND tests)
