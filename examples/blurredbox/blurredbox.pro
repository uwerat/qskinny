CONFIG += qskexample
CONFIG += opengl

QT += quick_private opengl-private # TODO: examples should not use private headers

SOURCES += \
    main.cpp \
    MainWindow.cpp \
    BlurredBox.cpp \
    BlurredBoxMaterial.cpp \
    BlurredBoxMaterialShader.cpp \
    BlurredBoxNode.cpp \
    BlurredBoxSkinlet.cpp \
    BlurredBoxTextureProvider.cpp

HEADERS += \
    MainWindow.h \
    BlurredBox.h \
    BlurredBoxMaterial.h \
    BlurredBoxMaterialShader.h \
    BlurredBoxNode.h \
    BlurredBoxSkinlet.h \
    BlurredBoxTextureProvider.h

RESOURCES += \
    blurringshaders.qrc
