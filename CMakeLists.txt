cmake_minimum_required(VERSION 3.17)

# Include huntergate for fulfilling boost dependency
include(cmake/HunterGate.cmake)
HunterGate(
        URL "https://github.com/cpp-pm/hunter/archive/v0.23.271.tar.gz"
        SHA1 "2d6686024529b471e0cc3a4fc07d4d64b62a53ee"
)

project(AODBC)
set(CMAKE_CXX_STANDARD 20)

hunter_add_package(Boost)
find_package(Boost)



file(GLOB_RECURSE cpp_files CONFIGURE_DEPENDS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.cpp")
file(GLOB_RECURSE hpp_files CONFIGURE_DEPENDS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.hpp")


add_library(aodbc-lib ${cpp_files} ${hpp_files})
target_compile_definitions(aodbc-lib
        PUBLIC
        -DBOOST_ASIO_NO_DEPRECATED
        -DBOOST_ASIO_DISABLE_CONCEPTS  # gcc10 and clang10 won't compile an asio program with coroutines and concepts
        )

target_include_directories(aodbc-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(aodbc-lib PUBLIC Boost::boost)
target_compile_options(aodbc-lib PRIVATE -Werror -Wall -Wextra -pedantic)


# Test executable
add_executable(aodbc-test test_executable/main.cpp)
target_link_libraries(aodbc-test PRIVATE aodbc-lib)
