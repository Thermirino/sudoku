add_library(unity STATIC unity/src/unity.c unity/src/unity.h unity/src/unity_internals.h)
target_include_directories(unity PUBLIC unity/src)
target_compile_definitions(unity PUBLIC -DUNITY_INCLUDE_DOUBLE)

add_executable(test_solver test_solver.c)
target_link_libraries(test_solver PRIVATE sudoku unity)

add_executable(test_sudoku test_sudoku.c)
target_link_libraries(test_sudoku PRIVATE sudoku draw unity)

add_executable(test_draw test_draw.c)
target_link_libraries(test_draw PRIVATE sudoku draw unity)

# Copy assets
add_custom_command(
	TARGET test_draw POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/tests/assets
)

add_test(NAME test_solver COMMAND test_solver)
add_test(NAME test_sudoku COMMAND test_sudoku)
add_test(NAME test_draw COMMAND test_draw)
