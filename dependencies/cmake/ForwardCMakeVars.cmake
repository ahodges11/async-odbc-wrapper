if (FORWARD_CMAKE_ARGS_INCLUDED)
    return()
else()
    set(FORWARD_CMAKE_ARGS_INCLUDED 1)
endif()

function(ForwardToList list_name var_name)

    if(DEFINED ${var_name})
        list(APPEND ${list_name} "-D${var_name}=${${var_name}}")
    endif()

    set(${list_name} ${${list_name}} PARENT_SCOPE)
endfunction()

function(ForwardCMakeArgs out_list)
    set(result)

    foreach(var_name
            CMAKE_C_COMPILER
            CMAKE_C_COMPILER_TARGET
            CMAKE_C_FLAGS
            CMAKE_CXX_COMPILER
            CMAKE_CXX_COMPILER_TARGET
            CMAKE_CXX_FLAGS
            CMAKE_CXX_STANDARD
            CMAKE_FIND_ROOT_PATH_MODE_PROGRAM
            CMAKE_FIND_ROOT_PATH_MODE_LIBRARY
            CMAKE_FIND_ROOT_PATH_MODE_INCLUDE
            CMAKE_FIND_ROOT_PATH_MODE_PACKAGE
            CMAKE_IOS_INSTALL_COMBINED
            CMAKE_OSX_ARCHITECTURES
            CMAKE_OSX_DEPLOYMENT_TARGET
            CMAKE_STAGING_PREFIX
            CMAKE_SYSROOT
            CMAKE_SYSTEM_NAME
            CMAKE_SYSTEM_PROCESSOR
            CMAKE_SYSTEM_VERSION
            CMAKE_TOOLCHAIN_FILE
            CMAKE_XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH
            )
        ForwardToList(result ${var_name})
    endforeach()

    set(${out_list} ${result} PARENT_SCOPE)
endfunction()
