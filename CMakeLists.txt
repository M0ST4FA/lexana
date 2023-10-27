# CMakeList.txt : CMake project for myacc, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project(lexana)

# SETUP VARIABLES

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(lexana_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

# ADD THE LIBRARY
add_library(lexana INTERFACE
"${lexana_INCLUDE_DIR}/lexana/LADataStructs.h"
"${lexana_INCLUDE_DIR}/lexana/LexicalAnalyzer.h")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/external/fsm/")


target_link_libraries(lexana INTERFACE fsm)
target_include_directories(lexana INTERFACE 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
)

# add_executable(main main.cpp)
# target_link_libraries(main PRIVATE lexana)

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
