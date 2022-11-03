find_package(PkgConfig)
pkg_check_modules(PKG_HUNSPELL QUIET hunspell)
find_path(HUNSPELL_INCLUDE_DIRS
        NAMES hunspell.hxx
        PATH_SUFFIXES hunspell
        HINTS ${PKG_HUNSPELL_INCLUDE_DIRS})
find_library(HUNSPELL_LIBRARIES
            NAMES ${PKG_HUNSPELL_LIBRARIES} hunspell hunspell-1.8 hunspell-1.7 hunspell-1.6 libhunspell
            HINTS ${PKG_HUNSPELL_LIBRARY_DIRS})
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(HUNSPELL
                                    REQUIRED_VARS HUNSPELL_LIBRARIES HUNSPELL_INCLUDE_DIRS
                                    VERSION_VAR PKG_HUNSPELL_VERSION)
mark_as_advanced(HUNSPELL_INCLUDE_DIRS HUNSPELL_LIBRARIES)

add_library(hunspell SHARED IMPORTED GLOBAL)
set_target_properties(hunspell PROPERTIES IMPORTED_LOCATION ${HUNSPELL_LIBRARIES})
target_include_directories(hunspell
    INTERFACE
        ${HUNSPELL_INCLUDE_DIRS})