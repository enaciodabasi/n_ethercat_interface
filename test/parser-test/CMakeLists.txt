cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 17)

find_package(GTest REQUIRED)
find_package(yaml-cpp REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(parser_test test.cpp /home/naci/ethercat_interface/src/parser.cpp)
target_link_libraries(parser_test ${GTEST_LIBRARIES} ${YAML_CPP_LIBRARIES} pthread)
target_include_directories(parser_test PUBLIC /home/naci/ethercat_interface/include/ ${YAML_CPP_INCLUDE_DIRS})