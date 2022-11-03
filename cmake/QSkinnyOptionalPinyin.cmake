find_package(PkgConfig)
pkg_check_modules(PKG_PINYIN QUIET pinyin)
find_path(PINYIN_INCLUDE_DIRS
        NAMES pinyinime.h
        PATH_SUFFIXES pinyin LibIME/libime/pinyin
        HINTS ${PKG_PINYIN_INCLUDE_DIRS})
find_library(PINYIN_LIBRARIES
            NAMES ${PKG_PINYIN_LIBRARIES} IMEPinyin imepinyin ime libime libimepinyin
            HINTS ${PKG_PINYIN_LIBRARY_DIRS})
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PINYIN
                                  REQUIRED_VARS PINYIN_LIBRARIES PINYIN_INCLUDE_DIRS
                                  VERSION_VAR PKG_PINYIN_VERSION)
mark_as_advanced(PINYIN_INCLUDE_DIRS PINYIN_LIBRARIES)

add_library(pinyin SHARED IMPORTED GLOBAL)
set_target_properties(pinyin PROPERTIES IMPORTED_LOCATION ${PINYIN_LIBRARIES})
target_include_directories(pinyin
    INTERFACE
        ${PINYIN_INCLUDE_DIRS})