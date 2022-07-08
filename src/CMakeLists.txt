# Add executable.
set(SOURCE_FILES
	main.cpp
	${CMAKE_BINARY_DIR}/config.h
)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
add_subdirectory(test_dir)

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
	PRIVATE imgui::imgui
)
