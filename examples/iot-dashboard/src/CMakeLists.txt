project(kirigami)

if (BUILD_SHARED_LIBS)
    ecm_create_qm_loader(kirigami_QM_LOADER libkirigami2plugin_qt)
else()
    set(KIRIGAMI_STATIC_FILES
        libkirigami/basictheme.cpp
        libkirigami/platformtheme.cpp
        libkirigami/tabletmodewatcher.cpp
        libkirigami/kirigamipluginfactory.cpp)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libkirigami ${CMAKE_CURRENT_BINARY_DIR}/libkirigami)

set(kirigami_SRCS
    kirigamiplugin.cpp
    columnview.cpp
    enums.cpp
    delegaterecycler.cpp
    icon.cpp
    settings.cpp
    formlayoutattached.cpp
    pagepool.cpp
    imagecolors.cpp
    scenepositionattached.cpp
    mnemonicattached.cpp
    wheelhandler.cpp
    shadowedrectangle.cpp
    shadowedtexture.cpp
    colorutils.cpp
    pagerouter.cpp
    avatar.cpp
    toolbarlayout.cpp
    toolbarlayoutdelegate.cpp
    sizegroup.cpp
    scenegraph/managedtexturenode.cpp
    scenegraph/shadowedrectanglenode.cpp
    scenegraph/shadowedrectanglematerial.cpp
    scenegraph/shadowedborderrectanglematerial.cpp
    scenegraph/paintedrectangleitem.cpp
    scenegraph/shadowedtexturenode.cpp
    scenegraph/shadowedtexturematerial.cpp
    scenegraph/shadowedbordertexturematerial.cpp
    ${kirigami_QM_LOADER}
    ${KIRIGAMI_STATIC_FILES}
    )

qt5_add_resources(SHADERS scenegraph/shaders/shaders.qrc)

add_subdirectory(libkirigami)

if(NOT BUILD_SHARED_LIBS)
    # `rcc` is a bit dumb and isn't designed to use auto generated files, to
    # avoid poluting the source directory, use absolute paths
    set(kirigami_QML_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../)

    # First, pre-process the QRC to add the files associated with the right Qt
    # version.
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/../kirigami.qrc.in
        ${CMAKE_CURRENT_BINARY_DIR}/../kirigami.qrc
        @ONLY
    )

    # When using the static library, all QML files need to be shipped within the
    # .a file.
    qt5_add_resources(
        RESOURCES ${CMAKE_CURRENT_BINARY_DIR}/../kirigami.qrc
    )

    if (UNIX AND NOT ANDROID AND NOT(APPLE) AND NOT(DISABLE_DBUS))
        qt5_add_dbus_interface(kirigami_SRCS libkirigami/org.kde.KWin.TabletModeManager.xml tabletmodemanager_interface)
    endif()
endif()


add_library(kirigamiplugin ${kirigami_SRCS} ${RESOURCES} ${SHADERS})

if(NOT BUILD_SHARED_LIBS)
    SET_TARGET_PROPERTIES(kirigamiplugin PROPERTIES
        AUTOMOC_MOC_OPTIONS -Muri=org.kde.kirigami)
    if (UNIX AND NOT ANDROID AND NOT(APPLE) AND NOT(DISABLE_DBUS))
        set(Kirigami_EXTRA_LIBS Qt5::DBus)
    else()
        set(Kirigami_EXTRA_LIBS "")
    endif()
else()
    set(Kirigami_EXTRA_LIBS KF5::Kirigami2)
endif()


target_link_libraries(kirigamiplugin
        PUBLIC
            Qt5::Core
        PRIVATE
            ${Kirigami_EXTRA_LIBS} Qt5::Qml Qt5::Quick Qt5::QuickControls2 Qt5::Concurrent
    )

if (BUILD_SHARED_LIBS)
    add_custom_target(copy_to_bin ALL
        COMMAND ${CMAKE_COMMAND} -E
                make_directory ${CMAKE_BINARY_DIR}/bin/org/kde/kirigami.2/
        COMMAND ${CMAKE_COMMAND} -E
                copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/controls ${CMAKE_BINARY_DIR}/bin/org/kde/kirigami.2/
        COMMAND ${CMAKE_COMMAND} -E
                copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/styles  ${CMAKE_BINARY_DIR}/bin/org/kde/kirigami.2/styles
        COMMAND ${CMAKE_COMMAND} -E
                copy $<TARGET_FILE:kirigamiplugin> ${CMAKE_BINARY_DIR}/bin/org/kde/kirigami.2/
    )

    install(DIRECTORY controls/ DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/kirigami.2)

    if (DESKTOP_ENABLED)
        install(DIRECTORY styles/org.kde.desktop DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/kirigami.2/styles)
    endif()
    install(DIRECTORY styles/Material DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/kirigami.2/styles)

    install(FILES ${platformspecific} DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/kirigami.2)

    include(ECMGeneratePriFile)
    ecm_generate_pri_file(BASE_NAME Kirigami2 LIB_NAME KF5Kirigami2 DEPS "core qml quick svg" FILENAME_VAR PRI_FILENAME )
    install(FILES ${PRI_FILENAME}
            DESTINATION ${ECM_MKSPECS_INSTALL_DIR})

endif()

install(TARGETS kirigamiplugin DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/kirigami.2)
