# CMakeList.txt : CMake project for myacc, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.26)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project(lexana)

# SETUP VARIABLES

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(${PROJECT_NAME}_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")

# ADD THE LIBRARY
add_library(${PROJECT_NAME} INTERFACE
"${${PROJECT_NAME}_INCLUDE_DIR}/${PROJECT_NAME}/LADataStructs.h"
"${${PROJECT_NAME}_INCLUDE_DIR}/${PROJECT_NAME}/LexicalAnalyzer.h")

add_subdirectory("${PROJECT_SOURCE_DIR}/external/fsm/")


target_link_libraries(${PROJECT_NAME} INTERFACE fsm)
target_include_directories(${PROJECT_NAME} INTERFACE 
	$<BUILD_INTERFACE:${${PROJECT_NAME}_INCLUDE_DIR}>
	$<INSTALL_INTERFACE:include>
)

# ADD THE EXAMPLES
if(${BUILD_EXAMPLES})
	add_subdirectory("./examples/")
endif()

# ADD THE TESTS
if(${BUILD_TESTING})
	enable_testing()
	add_subdirectory("./tests/")
endif()

# BUILD THE DOCUMENTATION
if(${BUILD_DOCUMENTATION})
	add_subdirectory("./docs/")
endif()
