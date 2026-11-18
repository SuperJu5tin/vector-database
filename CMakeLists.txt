cmake_minimum_required(VERSION 3.16)
project(assignment LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)

add_executable(assignment
  src/main.cpp
  src/file_input.cpp
  src/chunking_component.cpp
)
target_include_directories(assignment PRIVATE include)
target_compile_options(assignment PRIVATE -Wall -Wextra -Wpedantic)

if(BUILD_TESTING)
  add_executable(chunking_component_test
    tests/chunking_component_test.cpp
    src/chunking_component.cpp
  )
  target_include_directories(chunking_component_test PRIVATE include)
  target_compile_options(chunking_component_test PRIVATE -Wall -Wextra -Wpedantic)
  add_test(NAME chunking_component_test COMMAND chunking_component_test)

  add_test(
    NAME frankenstein_chunk_output
    COMMAND ${CMAKE_COMMAND}
      -DASSIGNMENT_EXECUTABLE=$<TARGET_FILE:assignment>
      -DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
      -DOUTPUT_DIR=${CMAKE_CURRENT_BINARY_DIR}
      -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/check_main_output.cmake
  )
endif()
