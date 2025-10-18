enable_testing()

file(GLOB TEST_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_executable(PokerEngine_tests
    ${TEST_SOURCES}
)

target_link_libraries(PokerEngine_tests
    PokerEngine
    GTest::gtest
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(PokerEngine_tests)