cmake_minimum_required(VERSION 3.13.3)

# Download vcpkg if needed.
find_package(Git QUIET)
if (GIT_FOUND AND NOT EXISTS ${CMAKE_SOURCE_DIR}/vcpkg)
	message(STATUS "Downloading vcpkg...")
	execute_process(COMMAND ${GIT_EXECUTABLE} clone https://github.com/microsoft/vcpkg.git vcpkg
					WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
					RESULT_VARIABLE GIT_CLONE_RESULT)
	if (NOT GIT_CLONE_RESULT EQUAL "0")
		message(FATAL_ERROR "'git clone https://github.com/microsoft/vcpkg.git vcpkg' ended with ${GIT_SUBMODULE_RESULT}.")
	else()
		message(STATUS "Downloaded VCPKG")
	endif()
elseif (NOT GIT_FOUND AND NOT EXISTS ${CMAKE_SOURCE_DIR}/vcpkg)
	message(FATAL_ERROR "Cannot find git. VCPKG download failed.")
endif()

# Set toolchain to VCPKG
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Path to vcpkg cmake module")

# Begin project definitions.
project(SDLDemo)

# Specify the C++ standard.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

# Set includes.
include_directories(
	${CMAKE_BINARY_DIR}		# To find generated configure file.
)

# Define REN_DEBUG if building Debug configuration.
add_compile_definitions(
	"$<$<CONFIG:Debug,PackageDebug>:REN_DEBUG>"
	"$<$<CONFIG:PackageDebug,PackageRelease>:REN_INSTALL>"
	"$<$<PLATFORM_ID:Windows>:_BUILD_WINDOWS>"
	"$<$<PLATFORM_ID:Linux>:_BUILD_LINUX>"
)

# Add compile options to generate more warnings.
if (MSVC)
	add_compile_options(/W4 /WX)
else()
	add_compile_options(-Wall -Wextra -Wno-unused-parameter)
endif()

# Go to src subdirectory, to add all source files and executable.
add_subdirectory(src)

# Generate configure file.
set(SOURCE_DIR ${CMAKE_SOURCE_DIR})
configure_file(config.h.in config.h)

# Install defines.
install(TARGETS "${PROJECT_NAME}" DESTINATION bin)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/assets" DESTINATION .)
include(CPack)
