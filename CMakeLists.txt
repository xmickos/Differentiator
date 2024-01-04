cmake_minimum_required(VERSION 3.1...3.27)

project(Differentiator
        VERSION 04.01
        DESCRIPTION "This is a program for calculating mathematical expressions and their derivatives by entering them into a computational graph. Expressions are read using a Recursive descent algorithm."
        LANGUAGES CXX
        )

set(default_build_type "Release")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
endif()

set(CMAKE_CXX_STANDARD 11 CACHE STRING "The C++ standart to use")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_compile_options(-DDEBUG_=1 -ggdb3 -O0 -Wall -Wextra -Weffc++ -Wc++14-compat -Wmissing-declarations
	-Wcast-qual -Wchar-subscripts -Wconversion -Wctor-dtor-privacy -Wempty-body
	-Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat=2 -Winline -Wnon-virtual-dtor
	-Wopenmp -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion
	-Wstrict-overflow=2	-Wconversion -Werror
	-Wsuggest-override -Wswitch-default -Wswitch-enum -Wundef -Wunreachable-code -Wunused
	-Wvariadic-macros  -Wno-reserved-user-defined-literal -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast
	-Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow
	-fno-omit-frame-pointer -Wlarger-than=8192 -Wframe-larger-than=8192
    )

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero")


configure_file (
    "${PROJECT_SOURCE_DIR}/include/differ_header.h.in"
    "${PROJECT_BINARY_DIR}/include/differ_header.h"
)

add_subdirectory(src)

add_subdirectory(Executables)

# include(FetchContent)
# FetchContent_Declare(
#     googletest
#     URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
# )
#
# set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# FetchContent_MakeAvailable(googletest)


