# Shlomi Ben-Shushan [REDACTED_ID]
# Yiftach Neuman 208305359

include_directories(${CMAKE_SOURCE_DIR})

# If Qt+OpenGL dependencies were found, descend into these directories
if(ARGOS_COMPILE_QTOPENGL)
    add_subdirectory(id_loop_functions)
    add_subdirectory(trajectory_loop_functions)
endif(ARGOS_COMPILE_QTOPENGL)

