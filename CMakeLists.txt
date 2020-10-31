cmake_minimum_required(VERSION 3.12)

project(path_finder)

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
	src/ThreadPool.cpp
	src/GeometricTypes.cpp
)

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
	include/path_finder/storage/DataConfig.h
	include/path_finder/graphs/Grid.h
	include/path_finder/storage/FileLoader.h
	include/path_finder/storage/FileWriter.h
	include/path_finder/helper/ThreadPool.h
	include/path_finder/graphs/GeometricType.h
	include/path_finder/helper/Types.h
)

find_package(OpenMP)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(TBB REQUIRED COMPONENTS tbb)

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(vendor/nlohmann_json)

option(PACKAGE_TESTS "Build the tests" OFF)
if(PACKAGE_TESTS)
    enable_testing()
    include(GoogleTest)
    add_subdirectory(tests)
endif()

option(PACKAGE_BENCHMARKS "Build benchmark" ON)


add_library(${PROJECT_NAME} ${SOURCES})
target_compile_definitions(${PROJECT_NAME} PRIVATE TEST=0)
target_link_libraries(${PROJECT_NAME}
	OpenMP::OpenMP_CXX
	nlohmann_json::nlohmann_json
	Threads::Threads
	TBB::tbb
)
target_include_directories(${PROJECT_NAME} PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_compile_options(${PROJECT_NAME} PRIVATE
	"-fPIC"
)
target_compile_features(${PROJECT_NAME} PUBLIC
	cxx_std_20
)

if(CMAKE_BUILD_TYPE=Release OR CMAKE_BUILD_TYPE=ultra)
    set_property(TARGET ${PROJECT_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

