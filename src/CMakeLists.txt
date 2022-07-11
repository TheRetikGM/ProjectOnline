# Find dependencies.
find_package(freetype CONFIG REQUIRED)
find_package(glm REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(sdl2-image CONFIG REQUIRED)
find_package(sdl2-ttf CONFIG REQUIRED)
find_package(sdl2-net CONFIG REQUIRED)
find_package(sdl2-mixer CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(box2d CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)

# Add executable and add its sources in subdirectories.
set(REN_SOURCE_FILES
	main.cpp
	${CMAKE_BINARY_DIR}/config.h
)
add_executable(${PROJECT_NAME} ${REN_SOURCE_FILES})
add_subdirectory(Ren)

# Set include directories.
target_include_directories(${PROJECT_NAME}
	PRIVATE include
)

# Link executable.
target_link_libraries(${PROJECT_NAME} 
	PRIVATE freetype
	PRIVATE SDL2::SDL2
	PRIVATE SDL2::SDL2main
	PRIVATE SDL2::SDL2_image
	PRIVATE SDL2::SDL2_ttf
	PRIVATE SDL2::SDL2_net
	PRIVATE SDL2::SDL2_mixer
	PRIVATE imgui::imgui
	PRIVATE box2d::box2d
	PRIVATE EnTT::EnTT
)
