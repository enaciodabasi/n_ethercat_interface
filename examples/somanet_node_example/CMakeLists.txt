cmake_minimum_required(VERSION 3.0)
project(somanet_node_example)
set(CMAKE_CXX_STANDARD 17)

set(ethercat_interface_lib /home/naci/ethercat_interface/lib/ethercat_interface/libethercat_interface.so)
set(ethercat_interface_INCLUDE_DIRS /home/naci/ethercat_interface/include/)
add_library(libethercat_interface SHARED IMPORTED)
set_target_properties(libethercat_interface PROPERTIES IMPORTED_LOCATION ${ethercat_interface_lib})

add_executable(somanet_node_example somanet_node_example.cpp)
target_link_libraries(somanet_node_example libethercat_interface ${GTEST_LIBRARIES} ${YAML_CPP_LIBRARIES} pthread)
target_include_directories(somanet_node_example PUBLIC ${ethercat_interface_INCLUDE_DIRS} ${YAML_CPP_INCLUDE_DIRS})