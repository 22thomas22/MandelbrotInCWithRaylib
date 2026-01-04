# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.21)

# Add source to this project's executable.


# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("FallingSand" LANGUAGES C)

# Include sub-projects.
#add_subdirectory ("FallingSand")

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON) # Enforce the standard; fail if not available
set(CMAKE_C_EXTENSIONS OFF)       # Optional: Disable compiler-specific extensions


add_subdirectory(raylib)
add_executable (FallingSand "mandelbrot.c" )

# Enforce C11 standard for this target
set_target_properties(FallingSand PROPERTIES
    C_STANDARD 17
    C_STANDARD_REQUIRED True
)


target_link_libraries(FallingSand PRIVATE raylib winmm)