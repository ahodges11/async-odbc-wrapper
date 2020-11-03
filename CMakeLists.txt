cmake_minimum_required(VERSION 3.17)

include(ProcessorCount)
ProcessorCount(processors)

include(dependencies/cmake/ForwardCMakeVars.cmake)

project(AODBC VERSION 0.1)

option(ENABLE_COVERAGE "" OFF)

get_filename_component(build_leaf "${CMAKE_CURRENT_BINARY_DIR}" NAME)
message(STATUS "build_leaf=${build_leaf} from ${CMAKE_CURRENT_BINARY_DIR}")

ForwardCMakeArgs(to_forward)
message(STATUS "[dependencies] executing dependencies with ${to_forward}")
execute_process(COMMAND ${CMAKE_COMMAND}
        "-H${CMAKE_CURRENT_LIST_DIR}/dependencies"
        "-B${CMAKE_CURRENT_BINARY_DIR}/dependencies"
        ${to_forward})


list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_BINARY_DIR}/dependencies/install)
find_package(Boost COMPONENTS thread log REQUIRED CONFIG)
find_package(Threads REQUIRED)
find_package(ODBC REQUIRED)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(Catch2 CONFIG REQUIRED)

if (CMAKE_BUILD_TYPE MATCHES "[Dd]ebug")
    if (CMAKE_CXX_COMPILER_ID MATCHES "[Cc]lang")
        # using Clang
        if (ENABLE_COVERAGE)
            add_compile_options("--coverage")
            add_link_options("--coverage")
        endif ()
        add_compile_options(-Wall -Wextra -pedantic -Werror -fchar8_t)
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "[Gg][Nn][Uu]")
        # using GCC
        if (ENABLE_COVERAGE)
            add_compile_options("--coverage")
            add_link_options(-lgcov)
        endif ()
        add_compile_options(-Wall -Wextra -pedantic -Werror)
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
        # using Intel C++
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        # using Visual Studio C++
        add_compile_options(/W4 /WX)
    endif ()
endif ()


file(GLOB_RECURSE cpp_files CONFIGURE_DEPENDS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.cpp")
file(GLOB_RECURSE hpp_files CONFIGURE_DEPENDS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.hpp")
set(spec_files ${cpp_files})
list(FILTER spec_files INCLUDE REGEX "^.*.\\.spec\\.[ch]pp$")
list(FILTER spec_files EXCLUDE REGEX "^.*.main\\.spec\\.cpp$")
list(FILTER cpp_files EXCLUDE REGEX "^.*.\\.spec\\.[ch]pp$")




add_library(aodbc-lib ${cpp_files} ${hpp_files})
target_compile_definitions(aodbc-lib
        PUBLIC
        -DBOOST_ASIO_NO_DEPRECATED
        -DBOOST_ASIO_DISABLE_CONCEPTS  # gcc10 and clang10 won't compile an asio program with coroutines and concepts
        )


target_include_directories(aodbc-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(aodbc-lib PUBLIC Boost::log Boost::thread Threads::Threads fmt::fmt spdlog::spdlog ${ODBC_LIBRARIES})
target_compile_options(aodbc-lib PRIVATE -Werror -Wall -Wextra -pedantic)


add_executable(aodbc-lib-tests src/main.spec.cpp ${spec_files})
target_link_libraries(aodbc-lib-tests PUBLIC aodbc-lib Catch2::Catch2)



# Test executable
file(GLOB_RECURSE tests_cpp_files CONFIGURE_DEPENDS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "tests/*.cpp")
#file(GLOB_RECURSE hpp_files CONFIGURE_DEPENDS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.hpp")
set(tests_spec_files ${tests_cpp_files})
list(FILTER tests_spec_files INCLUDE REGEX "^.*.\\.spec\\.[ch]pp$")
list(FILTER tests_spec_files EXCLUDE REGEX "^.*.main\\.spec\\.cpp$")
list(FILTER tests_cpp_files EXCLUDE REGEX "^.*.\\.spec\\.[ch]pp$")

add_executable(aodbc-tests tests/main.spec.cpp ${tests_spec_files})
target_link_libraries(aodbc-tests PRIVATE aodbc-lib Catch2::Catch2)
