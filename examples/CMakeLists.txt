
file(GLOB EXAMPLE_SRCs "./*.cpp")
add_executable(lexanaExamples ${EXAMPLE_SRCs})
target_link_libraries(lexanaExamples PRIVATE lexana fsm)