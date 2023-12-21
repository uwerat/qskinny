############################################################################
# QSkinny - Copyright (C) 2016 Uwe Rathmann
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

macro(qsk_load_optional_build_flags)

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

endmacro()

macro(qsk_enable_pedantic_flags)

    if(QT_VERSION_MAJOR VERSION_EQUAL 5)
        add_compile_definitions(QT_STRICT_ITERATORS)
    endif()

    # note: setting CMAKE_CXX_COMPILER without CMAKE_CXX_COMPILER_ID
    #       will fail here

    if ( ( CMAKE_CXX_COMPILER_ID MATCHES "GNU" )
        OR ( CMAKE_CXX_COMPILER_ID MATCHES "Clang" ) )

            add_compile_options( -pedantic)
            add_compile_options( -pedantic-errors)

            add_compile_options( -Wformat -Werror=format-security)
            add_compile_options( -Wsuggest-override)

            add_compile_options( -Wextra-semi )
            add_compile_options( -Winline )

            add_compile_options( -Wmissing-declarations )
            add_compile_options( -Wredundant-decls )

            add_compile_options( -Wnon-virtual-dtor )
            add_compile_options( -Woverloaded-virtual )
            add_compile_options( -Wfloat-equal )

        if ( CMAKE_CXX_COMPILER_ID MATCHES "GNU" )

            add_compile_options( -Wsuggest-attribute=pure)
            add_compile_options( -Wsuggest-attribute=const)
            add_compile_options( -Wsuggest-attribute=noreturn)
            add_compile_options( -Wsuggest-attribute=malloc)
            add_compile_options( -Wsuggest-final-types)
            add_compile_options( -Wsuggest-final-methods)

            add_compile_options( -Wduplicated-branches )
            add_compile_options( -Wduplicated-cond )
            add_compile_options( -Wshadow=local)

            # we have a lot of useless casts, when qreal is a double,
            # that are needed when qreal is a float. 

            # add_compile_options( -Wuseless-cast )

            add_compile_options( -Wlogical-op)
            add_compile_options( -Wclass-memaccess )

        elseif ( CMAKE_CXX_COMPILER_ID MATCHES "Clang" )

            add_compile_options( -Weverything)
            add_compile_options( -Wno-c++98-compat)
            add_compile_options( -Wno-c++98-compat-pedantic )
            add_compile_options( -Wno-global-constructors )

            # We have a lot of dummy destructors with the intention to
            #     - a breakpoint for the debugger
            #     - being able to add some debug statements without recompiling
            #     - preparation for binary compatible changes
            # Needs to be reconsidered: TODO ...
            add_compile_options( -Wno-deprecated-copy-with-user-provided-dtor )

            add_compile_options( -Wno-signed-enum-bitfield )
            add_compile_options( -Wno-padded )

            # since Qt 6.3 Q_GLOBAL_STATIC seems to use what is not supported for < c++20 
            add_compile_options( -Wno-gnu-zero-variadic-macro-arguments )
        endif()
    endif()

endmacro()

macro(qsk_initialize_build_flags)

    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)

    set(CMAKE_C_VISIBILITY_PRESET hidden)
    set(CMAKE_CXX_VISIBILITY_PRESET hidden)
    set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

    add_compile_definitions(QT_NO_KEYWORDS)
    # add_compile_definitions(QT_DISABLE_DEPRECATED_BEFORE=0x000000)

    if ( NOT MSVC )
        add_compile_options( -Wall -Wextra )
    else()
        # add_compile_options(/W4 /WX)
        add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/MP>)
    endif()

endmacro()

macro(qsk_finalize_build_flags)

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

        include(ECMEnableSanitizers)

    endif()

endmacro()
