# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.10)

project ("restsdk_https_client")

include("paths.cmake")
include("dependencies.cmake")
include("flags.cmake")

if(MSVC)
	configure_msvc_runtime()
endif()

print_compile_flags()

# Include sub-projects.
add_subdirectory ("restsdk_https_client")


