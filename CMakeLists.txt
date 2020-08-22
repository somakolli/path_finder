cmake_minimum_required(VERSION 3.12)

project(path_finder)

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
        src/GraphStats.cpp
        src/PathFinderBase.cpp
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
        src/OscarIntegration.cpp
        src/CellIdStore.cpp
        src/FileLoader.cpp
        src/HubLabelCreator.cpp
        src/HybridPathFinder.cpp
        src/Types.cpp
        src/base64.cpp
        src/FileWriter.cpp
        src/Benchmarker.cpp
        src/ThreadPool.cpp)

set(INCLUDES
        include/path_finder/routing/CHDijkstra.h
        include/path_finder/graphs/CHGraph.h
        include/path_finder/routing/Dijkstra.h
        include/path_finder/graphs/Graph.h
        include/path_finder/storage/GraphReader.h
        include/path_finder/graphs/GraphStats.h
        include/path_finder/routing/PathFinderBase.h
        include/path_finder/storage/HubLabelStore.h
        include/path_finder/storage/MmapVector.h
        include/path_finder/helper/Static.h
        include/path_finder/helper/Stopwatch.h
        include/path_finder/helper/Timer.h
        include/path_finder/storage/DataConfig.h
        include/path_finder/graphs/Grid.h
        include/path_finder/storage/FileLoader.h
        include/path_finder/storage/FileWriter.h
        include/path_finder/helper/ThreadPool.h
        include/path_finder/helper/Types.h)

find_package(Boost REQUIRED COMPONENTS system)
find_package(OpenMP)

include_directories(${Boost_INCLUDE_DIRS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${STXXL_CXX_FLAGS}")

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(vendor/nlohmann_json)
add_subdirectory(vendor/tbb)

option(PACKAGE_TESTS "Build the tests" OFF)
if(PACKAGE_TESTS)
    enable_testing()
    include(GoogleTest)
    add_subdirectory(tests)
endif()

option(PACKAGE_BENCHMARKS "Build benchmark" ON)
if(PACKAGE_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()

add_library(${PROJECT_NAME} ${SOURCES})
target_compile_definitions(${PROJECT_NAME} PRIVATE TEST=0)
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} OpenMP::OpenMP_CXX nlohmann_json::nlohmann_json tbb)
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" tbb)
set(CMAKE_CXX_FLAGS "-pthread")
set(CMAKE_CXX_FLAGS "-fPIC")
