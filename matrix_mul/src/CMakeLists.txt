cmake_minimum_required(VERSION 3.12)
project(MatrixMul VERSION 0.0.0)


file(GLOB_RECURSE SRCS *.cpp)
file(GLOB_RECURSE TSRCS *.inl)
file(GLOB_RECURSE HDRS *.h)

add_executable(MatrixMul ${SRCS} ${TSRCS} ${HDRS})

set_target_properties(
    MatrixMul PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Include directories
target_include_directories(
    MatrixMul PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../MPI/Include
)

target_link_directories(
    MatrixMul PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../MPI/Lib/X64
)

  target_link_libraries(
        MatrixMul

        msmpi.lib
        msmpifec.lib
        msmpifmc.lib
)