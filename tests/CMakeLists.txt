
# VARIABLES ------------------------------

# TESTS ----------------------------------

# DFATests
add_executable(LATest "LATests.cpp" "universal.cpp" "fsm.h" "universal.h")
target_include_directories(LATest PRIVATE
"${GTEST_INCLUDE_DIR}/../"
)
target_link_libraries(LATest PRIVATE lexana gtest_main)
gtest_discover_tests(LATest
TEST_PREFIX LATests
)
