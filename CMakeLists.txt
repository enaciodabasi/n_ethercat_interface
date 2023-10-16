cmake_minimum_required(VERSION 3.0)
project(ethercat_interface VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)

find_package(yaml-cpp REQUIRED)
find_package(Boost REQUIRED)

set(ETHERCAT_INCLUDE_PATH /home/${USER}/ethercat/include CACHE PATH "Path to the ecrt.h file of the IgH EtherCAT Master.")
set(ETHERCAT_LIB_PATH /usr/local/lib/libethercat.so CACHE FILEPATH "Path to the libethercat.so file.")

set(ethercat_INCLUDE_DIRS ${ETHERCAT_INCLUDE_PATH})
set(ethercat_LIBRARIES ${ETHERCAT_LIB_PATH})

#set(SOURCES 
#
#    src/master.cpp
#    src/slave.cpp
#    src/data.cpp
#    src/parser.cpp
#    src/comm_interface.cpp
#)

file(GLOB SOURCES src/*.cpp)

include(GNUInstallDirs)

add_library(
    ${PROJECT_NAME}
    SHARED
    ${SOURCES}
)
target_link_libraries(

    ${PROJECT_NAME}

    ${ethercat_LIBRARIES}
    ${YAML_CPP_LIBRARIES}
    ${Boost_LIBRARIES}
)
target_include_directories(
    
    ${PROJECT_NAME}
    
    PUBLIC
    
    ${ethercat_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${YAML_CPP_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)

install(
    TARGETS ${PROJECT_NAME}
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/${PROJECT_NAME}
)

add_subdirectory(test ${CMAKE_CURRENT_SOURCE_DIR}/build/test)

add_subdirectory(examples ${CMAKE_CURRENT_SOURCE_DIR}/build/examples)
