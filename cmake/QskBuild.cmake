############################################################################
# QSkinny - Copyright (C) 2016 Uwe Rathmann
# This file may be used under the terms of the QSkinny License, Version 1.0
############################################################################

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# TODO find compiler flag equivalent
list(APPEND CONFIG        warn_on)
#list(APPEND CONFIG           -= depend_includepath) # TODO was -=
list(APPEND CONFIG        pedantic)

set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

add_compile_definitions(QT_NO_KEYWORDS)

if ("pedantic" IN_LIST CONFIG)

    if(QT_VERSION_MAJOR VERSION_EQUAL "5")
        add_compile_definitions(QT_STRICT_ITERATORS)
    endif()

    if ("${CMAKE_CXX_COMPILER}" MATCHES ".*linux-g\\+\\+" OR "${CMAKE_CXX_COMPILER}" MATCHES ".*linux-g\\+\\+-64")
        set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -pedantic-errors)
        set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wextra)
        set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Werror=format-security)
        set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wsuggest-override)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wlogical-op)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wconversion)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wfloat-equal)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wshadow)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wsuggest-final-types)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wsuggest-final-methods)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -fanalyzer)
    endif()

    if ("${CMAKE_CXX_COMPILER}" MATCHES ".*linux-clang")

        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -pedantic-errors)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Weverything)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wno-c++98-compat-pedantic)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wno-global-constructors)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wno-exit-time-destructors)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wno-padded)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wno-float-equal)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wno-undefined-reinterpret-cast)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wno-deprecated)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wno-switch-enum)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wno-keyword-macro)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wno-old-style-cast)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wno-used-but-marked-unused)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wno-weak-vtables)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wno-shadow)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wno-double-promotion)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wno-conversion)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wno-documentation-unknown-command)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wno-unused-macros)
    endif()
endif()


# DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x000000

if( DEFINED ENV{QSK_LOCAL_CMAKE_RULES} )

    # When not working with the Qt/Creator it is often more convenient
    # to include the specific options of your local build, than passing
    # them all on the command line

    set(LOCAL_RULES $ENV{QSK_LOCAL_CMAKE_RULES})

    if(NOT "${LOCAL_RULES}" STREQUAL "")

        if(EXISTS ${LOCAL_RULES})
            message(STATUS "Loading build options from: ${LOCAL_RULES}")
            include(${LOCAL_RULES})
        else()
            message(Warning "Loading build options from: ${LOCAL_RULES}" - Failed)
        endif()

    endif()

endif()

message( STATUS "Build Type: ${CMAKE_BUILD_TYPE}" )

if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_compile_definitions(ITEM_STATISTICS=1)
endif()

if( (CMAKE_CXX_COMPILER_ID MATCHES "Clang") OR (CMAKE_CXX_COMPILER_ID MATCHES "GNU") )

    if (CMAKE_BUILD_TYPE MATCHES Debug)
        add_compile_options(-O0)
    else()
        add_compile_options(-O3)
    endif()

    add_compile_options(-ffast-math)

endif()


if ( NOT "${ECM_ENABLE_SANITIZERS}" STREQUAL "")

    # see: https://api.kde.org/ecm/module/ECMEnableSanitizers.html
    #
    # a semicolon-separated list of sanitizers:
    #   - address
    #   - memory
    #   - thread
    #   - leak
    #   - undefined
    #   - fuzzer
    #
    # where “address”, “memory” and “thread” are mutually exclusive
    # f.e: set(ECM_ENABLE_SANITIZERS address;leak;undefined)

    include(${CMAKE_SOURCE_DIR}/cmake/ECMEnableSanitizers.cmake)

endif()
