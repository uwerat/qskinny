

set(libkirigami_SRCS
    platformtheme.cpp
    basictheme.cpp
    kirigamipluginfactory.cpp
    tabletmodewatcher.cpp
)

#use dbus on linux, bsd etc, but not andoid and apple stuff
if (UNIX AND NOT ANDROID AND NOT(APPLE) AND NOT(DISABLE_DBUS))
    qt5_add_dbus_interface(libkirigami_SRCS org.kde.KWin.TabletModeManager.xml tabletmodemanager_interface)
    set(LIBKIRIGAMKI_EXTRA_LIBS Qt5::DBus)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

ecm_qt_declare_logging_category(libkirigami_SRCS
    HEADER loggingcategory.h
    IDENTIFIER KirigamiLog
    CATEGORY_NAME kf.kirigami
    DEFAULT_SEVERITY Warning
)

add_library(KF5Kirigami2 ${libkirigami_SRCS})
generate_export_header(KF5Kirigami2 BASE_NAME Kirigami2)
add_library(KF5::Kirigami2 ALIAS KF5Kirigami2)

target_include_directories(KF5Kirigami2
    INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF5}/Kirigami2>"
)

target_link_libraries(KF5Kirigami2
    PUBLIC
        Qt5::Core
    PRIVATE
        Qt5::Qml
        Qt5::Quick
        Qt5::QuickControls2
        ${LIBKIRIGAMKI_EXTRA_LIBS}
)

set_target_properties(KF5Kirigami2 PROPERTIES
    VERSION     ${KIRIGAMI2_VERSION_STRING}
    SOVERSION   ${KIRIGAMI2_SOVERSION}
    EXPORT_NAME "Kirigami2"
)

ecm_generate_headers(Kirigami2_HEADERS
    HEADER_NAMES
    PlatformTheme
    KirigamiPluginFactory
    TabletModeWatcher

    REQUIRED_HEADERS Kirigami2_HEADERS
)

install(TARGETS KF5Kirigami2
        EXPORT KF5Kirigami2Targets
        ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})

if(BUILD_SHARED_LIBS)
    list(APPEND Kirigami2_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/kirigami2_export.h)
endif()

install(FILES ${Kirigami2_HEADERS}
        DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/Kirigami2
        COMPONENT Devel)

if(BUILD_QCH)
    ecm_add_qch(
        KF5Kirigami2_QCH
        NAME Kirigami2
        BASE_NAME KF5Kirigami2
        VERSION ${KF5_VERSION}
        ORG_DOMAIN org.kde
        SOURCES # using only public headers, to cover only public API
            ${Kirigami2_HEADERS}
        MD_MAINPAGE "${CMAKE_SOURCE_DIR}/README.md"
        LINK_QCHS
            Qt5Core_QCH
        BLANK_MACROS
            KIRIGAMI_EXPORT
            KIRIGAMI_DEPRECATED
        TAGFILE_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        QCH_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        COMPONENT Devel
    )
endif()

include(ECMGeneratePriFile)
ecm_generate_pri_file(BASE_NAME Kirigami2 LIB_NAME KF5Kirigami2 DEPS "core" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF5}/Kirigami2)
install(FILES ${PRI_FILENAME}
        DESTINATION ${ECM_MKSPECS_INSTALL_DIR})

