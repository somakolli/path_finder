add_subdirectory("${PROJECT_SOURCE_DIR}/vendor/googletest" "vendor/googletest")

#keep cache clean
mark_as_advanced(
        BUILD_GMOCK BUILD_GTEST BUILD_SHARED_LIBS
        gmock_build_tests gtest_build_samples gtest_build_tests
        gtest_disable_pthreads gtest_force_shared_crt gtest_hide_internal_symbols
)
macro(package_add_test TESTNAME)
    # create an exectuable in which the tests will be stored
    add_executable(${TESTNAME} ${ARGN})
    # link the Google test infrastructure, mocking library, and a default main function to
    # the test executable.  Remove g_test_main if writing your own main function.
    target_link_libraries(${TESTNAME} gtest gmock gtest_main ${Boost_LIBRARIES} nlohmann_json::nlohmann_json path_finder)
    gtest_discover_tests(${TESTNAME}
            # set a working directory so your project root so that you can find test data via paths relative to the project root
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            )
    set_target_properties(${TESTNAME} PROPERTIES FOLDER tests)
endmacro()


package_add_test(HubLabelStore HubLabelStore.cpp)
package_add_test(HubLabels HubLabels.cpp)
package_add_test(RoutingTest routing_tests.cpp)