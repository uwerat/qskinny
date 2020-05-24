TEMPLATE = lib
TARGET   = $$qskLibraryTarget(qsktestsupport)

target.path    = $${QSK_INSTALL_EXAMPLES}/lib
INSTALLS       = target

CONFIG += ensure_skins
CONFIG += fontconfig
CONFIG += qskinny

contains(QSK_CONFIG, QskDll): DEFINES += SKINNY_MAKEDLL 

HEADERS += \
    SkinnyGlobal.h \
    SkinnyFont.h \
    SkinnyShapeFactory.h \
    SkinnyShapeProvider.h \
    SkinnyShortcut.h

SOURCES += \
    SkinnyFont.cpp \
    SkinnyPlugin.cpp \
    SkinnyShapeFactory.cpp \
    SkinnyShapeProvider.cpp \
    SkinnyShortcut.cpp

DEFINES += PLUGIN_PATH=$${QSK_PLUGIN_DIR}

ensure_skins {

    # Enabling fall back code, that inserts some skin factories manually
    # when not finding skin factories as plugins

    INCLUDEPATH *= $${QSK_ROOT}/skins
    DEPENDPATH *= $${QSK_ROOT}/skins

    DEFINES += ENSURE_SKINS

    use_install_rpath: QMAKE_RPATHDIR *= $${QSK_INSTALL_PLUGINS}/skins
    use_local_rpath: QMAKE_RPATHDIR *= $${QSK_PLUGIN_DIR}/skins

    qskAddLibrary($${QSK_PLUGIN_DIR}/skins, squiekskin)
    qskAddLibrary($${QSK_PLUGIN_DIR}/skins, materialskin)
}

fontconfig {

    QSK_FONTDIR=$${PWD}/fonts
    QSK_FONTCACHEDIR=$${OUT_PWD}/fontconfig
    QSK_FONTCONF_FILE=$${QSK_FONTCACHEDIR}/fonts.conf

    QSK_FONTCONF_DATA = "$$cat( $${PWD}/fonts.conf.in, false )"
    QSK_FONTCONF_DATA = $$replace(QSK_FONTCONF_DATA, FONTCACHEDIR, $${QSK_FONTCACHEDIR} )
    QSK_FONTCONF_DATA = $$replace(QSK_FONTCONF_DATA, FONTDIR, $${QSK_FONTDIR} )
    write_file( $${QSK_FONTCONF_FILE}, QSK_FONTCONF_DATA )

    DEFINES += FONTCONFIG_FILE=$$clean_path( $$QSK_FONTCONF_FILE )
}
