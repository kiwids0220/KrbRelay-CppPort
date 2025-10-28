cmake_minimum_required(VERSION 3.15)

# Example program demonstrating KrbRelay library usage
add_executable(basic_example basic_example.cpp)

target_link_libraries(basic_example
    PRIVATE
        KrbRelay
)

# Set output directory
set_target_properties(basic_example PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
