cmake_minimum_required(VERSION 3.12)

project(path_finder)

set(CMAKE_CXX_STANDARD 17)
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
        src/Static.cpp
        src/Stopwatch.cpp
        src/Timer.cpp
        src/SpaceMeasurer.cpp
        src/DataConfig.cpp
        src/VectorBase.cpp
        src/RamVector.cpp
        src/Grid.cpp
        src/OscarIntegration.cpp)

set(INCLUDES
        include/path_finder/CHDijkstra.h
        include/path_finder/CHGraph.h
        include/path_finder/Dijkstra.h
        include/path_finder/Graph.h
        include/path_finder/GraphReader.h
        include/path_finder/GraphStats.h
        include/path_finder/HubLabels.h
        include/path_finder/PathFinderBase.h
        include/path_finder/ChHlBenchmarker.h
        include/path_finder/HubLabelStore.h
        include/path_finder/MmapVector.h
        include/path_finder/Static.h
        include/path_finder/Stopwatch.h
        include/path_finder/Timer.h
        include/path_finder/DataConfig.h
        include/path_finder/Grid.h)

find_package(Boost REQUIRED COMPONENTS system iostreams)
find_package(OpenMP)

include_directories(${Boost_INCLUDE_DIRS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${STXXL_CXX_FLAGS}")

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(vendor/nlohmann_json)

option(PACKAGE_TESTS "Build the tests" ON)
if(PACKAGE_TESTS)
    enable_testing()
    include(GoogleTest)
    add_subdirectory(tests)
endif()

add_library(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} OpenMP::OpenMP_CXX nlohmann_json::nlohmann_json)
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(CMAKE_CXX_FLAGS "-pthread")
set(CMAKE_CXX_FLAGS "-fPIC")
