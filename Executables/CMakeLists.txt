add_executable(Differentiator Differentiator.cpp)

target_link_libraries(Differentiator PRIVATE RecursiveDescent)

target_link_libraries(Differentiator PRIVATE differ_lib)
