cmake_minimum_required(VERSION 3.12)

set(PROJECT_NAME path_finder)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 20)
if( NOT CMAKE_BUILD_TYPE )
    set( CMAKE_BUILD_TYPE Release)
endif()

set(SOURCES
        src/Graph.cpp
        src/GraphReader.cpp
        src/Dijkstra.cpp
        src/CHGraph.cpp
        src/CHDijkstra.cpp
        src/HubLabels.cpp
        src/GraphStats.cpp
        src/PathFinderBase.cpp
        src/ChHlBenchmarker.cpp
        src/HubLabelStore.cpp
        src/MmapVector.cpp
        src/Static.cpp )

set(INCLUDES
        include/CHDijkstra.h
        include/CHGraph.h
        include/Dijkstra.h
        include/Graph.h
        include/GraphReader.h
        include/GraphStats.h
        include/HubLabels.h
        include/PathFinderBase.h
        include/ChHlBenchmarker.h
        include/HubLabelStore.h
        include/MmapVector.h
        include/Static.h)

find_package(Boost REQUIRED COMPONENTS system iostreams)
find_package(OpenMP)

include_directories(${Boost_INCLUDE_DIRS} )
include_directories(${STXXL_INCLUDE_DIRS})
include_directories(INCLUDES)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${STXXL_CXX_FLAGS}")
add_library(${PROJECT_NAME} STATIC ${SOURCES})

option(PACKAGE_TESTS "Build the tests" ON)
if(PACKAGE_TESTS)
    enable_testing()
    include(GoogleTest)
    add_subdirectory(tests)
endif()

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} ${STXXL_LIBRARIES} OpenMP::OpenMP_CXX)
set(CMAKE_CXX_FLAGS "-pthread")
set(CMAKE_CXX_FLAGS "-fPIC")
