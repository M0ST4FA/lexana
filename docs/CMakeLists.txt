

if (CMAKE_BUILD_TYPE MATCHES [Dd]ebug)

	include("${PROJECT_SOURCE_DIR}/cmake/run_doxygen.cmake")
	include("${PROJECT_SOURCE_DIR}/cmake/run_sphinx.cmake")

endif()