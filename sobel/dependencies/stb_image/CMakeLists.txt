
cmake_minimum_required(VERSION 3.12)
project(STB_IMAGE VERSION 0.0.0)


set(SOURCES
	stb_image.cpp
	stb_image.h
	stb_image_write.h
)

add_library(stb_image ${SOURCES})

# Defines
add_definitions(-D"STB_IMAGE_IMPLEMENTATION")
add_definitions(-D"STB_IMAGE_WRITE_IMPLEMENTATION")
add_definitions(-D"STBI_WINDOWS_UTF8")
