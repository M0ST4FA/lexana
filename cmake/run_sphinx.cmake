
# Add the cmake folder so the FindSphinx module is found
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" 
${CMAKE_MODULE_PATH})

find_package(Sphinx REQUIRED)

set(SPHINX_SOURCE "${PROJECT_SOURCE_DIR}/docs/Sphinx/source")
set(SPHINX_BUILD "${PROJECT_BINARY_DIR}/docs/sphinx")

add_custom_target(Sphinx_${PROJECT_NAME} ALL
			  COMMAND
			  ${SPHINX_EXECUTABLE} -b html
			  ${SPHINX_SOURCE} ${SPHINX_BUILD}
			  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
			  COMMENT "Generating documentation with Sphinx"
			  )