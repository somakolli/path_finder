cmake_minimum_required(VERSION 3.14)
set(PROJECT_NAME path_finder)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)
if( NOT CMAKE_BUILD_TYPE )
    set( CMAKE_BUILD_TYPE Release)
endif()

find_package(STXXL REQUIRED)

set(SOURCES
        src/Graph.cpp
        src/GraphReader.cpp
        src/Dijkstra.cpp
        src/CHGraph.cpp
        src/CHDijkstra.cpp
        src/HubLabels.cpp
        src/GraphStats.cpp
        src/PathFinderBase.cpp)

set(INCLUDES
        include/CHDijkstra.h
        include/CHGraph.h
        include/Dijkstra.h
        include/Graph.h
        include/GraphReader.h
        include/GraphStats.h
        include/HubLabels.h
        include/PathFinderBase.h)

find_package(Boost REQUIRED COMPONENTS system iostreams)

include_directories(${Boost_INCLUDE_DIRS} )
include_directories(${STXXL_INCLUDE_DIRS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${STXXL_CXX_FLAGS}")
add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} ${STXXL_LIBRARIES})
set(CMAKE_CXX_FLAGS "-pthread")
set(CMAKE_CXX_FLAGS "-fPIC")