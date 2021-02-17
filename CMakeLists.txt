cmake_minimum_required(VERSION 3.17)


project(AODBC VERSION 0.1)


get_directory_property(hasParent PARENT_DIRECTORY)
if(NOT hasParent)
    include(dependencies/cmake/ForwardCMakeVars.cmake)
    ForwardCMakeArgs(to_forward)
    message(STATUS "[dependencies] executing dependencies with ${to_forward}")
    execute_process(COMMAND ${CMAKE_COMMAND}
            "-H${CMAKE_CURRENT_LIST_DIR}/dependencies"
            "-B${CMAKE_CURRENT_BINARY_DIR}/dependencies"
            ${to_forward})
    list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_BINARY_DIR}/dependencies/install)
endif()




find_package(Boost COMPONENTS thread log REQUIRED CONFIG)
find_package(Threads REQUIRED)
find_package(ODBC REQUIRED)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(Catch2 CONFIG REQUIRED)


file(GLOB_RECURSE cpp_files CONFIGURE_DEPENDS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.cpp")
file(GLOB_RECURSE hpp_files CONFIGURE_DEPENDS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.hpp")
set(spec_files ${cpp_files})
list(FILTER spec_files INCLUDE REGEX "^.*.\\.spec\\.[ch]pp$")
list(FILTER spec_files EXCLUDE REGEX "^.*.main\\.spec\\.cpp$")
list(FILTER cpp_files EXCLUDE REGEX "^.*.\\.spec\\.[ch]pp$")


add_library(AODBC ${cpp_files} ${hpp_files})
target_compile_definitions(AODBC
        PUBLIC
        -DBOOST_ASIO_NO_DEPRECATED
        -DBOOST_ASIO_DISABLE_CONCEPTS  # gcc10 and clang10 won't compile an asio program with coroutines and concepts
        )


target_include_directories(AODBC PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(AODBC PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>)

target_link_libraries(AODBC PUBLIC Boost::log Boost::thread Threads::Threads spdlog::spdlog fmt::fmt ${ODBC_LIBRARIES})
target_compile_options(AODBC PRIVATE -Werror -Wall -Wextra -pedantic)
#target_compile_definitions(aodbc-lib PUBLIC BOOST_ASIO_ENABLE_HANDLER_TRACKING=0)

add_library(AODBC::aodbc ALIAS AODBC)


# Test executable
file(GLOB_RECURSE tests_cpp_files CONFIGURE_DEPENDS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "tests/*.cpp")
#file(GLOB_RECURSE hpp_files CONFIGURE_DEPENDS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.hpp")
set(tests_spec_files ${tests_cpp_files})
list(FILTER tests_spec_files INCLUDE REGEX "^.*.\\.spec\\.[ch]pp$")
list(FILTER tests_spec_files EXCLUDE REGEX "^.*.main\\.spec\\.cpp$")
list(FILTER tests_cpp_files EXCLUDE REGEX "^.*.\\.spec\\.[ch]pp$")

add_executable(AODBC-tests tests/main.spec.cpp ${tests_spec_files})
target_link_libraries(AODBC-tests PRIVATE AODBC Catch2::Catch2)

