project(KVAN-project)
exec_program(git ARGS "rev-parse --show-toplevel" OUTPUT_VARIABLE top)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++20 -Wall")
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,-z,defs")

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(examples)

