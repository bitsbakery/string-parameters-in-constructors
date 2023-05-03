cmake_minimum_required(VERSION 3.25)

set(CMAKE_USER_MAKE_RULES_OVERRIDE "CMakeFlags.cmake")

project(string-parameters-in-constructors)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(string-parameters-in-constructors
    "main.cpp"
)

add_library(pretty OBJECT
    "pretty.cpp"
)

target_link_libraries(string-parameters-in-constructors PRIVATE
    pretty
)