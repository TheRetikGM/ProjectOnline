# Find dependencies.
find_package(freetype CONFIG REQUIRED)
find_package(glm REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(sdl2-image CONFIG REQUIRED)
find_package(sdl2-ttf CONFIG REQUIRED)
find_package(sdl2-mixer CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(box2d CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)

# Link libraries for both Ren and sandbox.
link_libraries( 
	freetype
	SDL2::SDL2
	SDL2::SDL2main
	SDL2::SDL2_image
	SDL2::SDL2_ttf
	SDL2::SDL2_mixer
	imgui::imgui
	box2d::box2d
	EnTT::EnTT
	yaml-cpp
)

# Set include directories.
include_directories(include)

########## Ren library ##########
set(REN_SOURCE_FILES
	${CMAKE_BINARY_DIR}/config.h
)
add_library(REN_LIB STATIC ${REN_SOURCE_FILES})
add_subdirectory(Ren)


########## Sandbox ##########
add_executable(${PROJECT_NAME} "")
add_subdirectory(sandbox)
target_include_directories(${PROJECT_NAME} PRIVATE sandbox/include)
target_link_libraries(${PROJECT_NAME} PRIVATE REN_LIB)
