# Shlomi Ben-Shushan 311408264
# Yiftach Neuman 208305359

include_directories(${CMAKE_SOURCE_DIR}/controllers)

# Example Controllers:
add_subdirectory(program_template)
add_subdirectory(run_away_by_bumpers)
add_subdirectory(simple_demo)
add_subdirectory(porting_example)
add_subdirectory(sand_timer_test)

# User Controllers:
add_subdirectory(ex1)
add_subdirectory(ex2)
