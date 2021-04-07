cmake_minimum_required(VERSION 3.17)
project(PRL_1)

set(CMAKE_CXX_STANDARD 14)

find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})

#add_definitions(-DOMPI_SKIP_MPICXX)

add_executable(PRL_1 pms.cpp)

#target_link_libraries(PRL_1 PRIVATE MPI_C)
target_link_libraries(PRL_1 ${MPI_LIBRARIES})

if(MPI_COMPILE_FLAGS)
    set_target_properties(PRL_1 PROPERTIES
            COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if(MPI_LINK_FLAGS)
    set_target_properties(PRL_1 PROPERTIES
            LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()
