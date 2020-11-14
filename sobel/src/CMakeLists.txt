cmake_minimum_required(VERSION 3.12)
project(Sobel VERSION 0.0.0)


file(GLOB_RECURSE SRCS *.cpp)
file(GLOB_RECURSE TSRCS *.inl)
file(GLOB_RECURSE HDRS *.h)

add_executable(Sobel ${SRCS} ${TSRCS} ${HDRS})

set_target_properties(
    Sobel PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

add_definitions(-D"STBI_WINDOWS_UTF8")
find_package(OpenMP)

target_link_libraries(Sobel PUBLIC
    OpenMP::OpenMP_CXX
    stb_image
)

# Include directories
target_include_directories(
    Sobel PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/stb_image
)

add_custom_command(TARGET Sobel PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${PROJECT_SOURCE_DIR}/resources
                       $<TARGET_FILE_DIR:Sobel>/resources)
