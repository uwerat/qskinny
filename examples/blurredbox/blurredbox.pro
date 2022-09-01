CONFIG += qskexample

QT += quick_private # TODO: examples should not use private headers

SOURCES += \
    main.cpp \
    MainWindow.cpp \
    BlurredBox.cpp \
    BlurredBoxMaterial.cpp \
    BlurredBoxMaterialShader.cpp \
    BlurredBoxNode.cpp \
    BlurredBoxSkinlet.cpp \

HEADERS += \
    MainWindow.h \
    BlurredBox.h \
    BlurredBoxMaterial.h \
    BlurredBoxMaterialShader.h \
    BlurredBoxNode.h \
    BlurredBoxSkinlet.h \

RESOURCES += \
    blurringshaders.qrc
