
add_library(RecursiveDescent RD_funcs.cpp "${Differentiator_SOURCE_DIR}/include/RD_header.hpp")

target_include_directories(RecursiveDescent PUBLIC "${Differentiator_SOURCE_DIR}/include")

add_library(differ_lib differentiator_funcs.cpp root_funcs.cpp debug_funcs.cpp "${Differentiator_SOURCE_DIR}/include/differ_header.h")

target_include_directories(differ_lib PUBLIC "${Differentiator_SOURCE_DIR}/include")
