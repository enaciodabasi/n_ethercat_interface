cmake_minimum_required(VERSION 3.0)
project(examples)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -frtti")

set(ethercat_interface_lib /home/naci/ethercat_interface/lib/ethercat_interface/libethercat_interface.so)
set(ethercat_interface_INCLUDE_DIRS /home/naci/ethercat_interface/include/)
add_library(libethercat_interface SHARED IMPORTED)
set_target_properties(libethercat_interface PROPERTIES IMPORTED_LOCATION ${ethercat_interface_lib})

add_executable(
    amr_example
    somanet_node_example/somanet_node_example.cpp
)
target_link_libraries(amr_example libethercat_interface ${ethercat_LIB})
target_include_directories(amr_example PUBLIC ${ethercat_interface_INCLUDE_DIRS})

