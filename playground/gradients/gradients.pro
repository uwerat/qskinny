CONFIG += qskexample

HEADERS += \
    GradientView.h

SOURCES += \
    GradientView.cpp \
    main.cpp

qtHaveModule(quickshapes_private) {

    QT += quickshapes_private

    HEADERS += GradientQuickShape.h
    SOURCES += GradientQuickShape.cpp

    DEFINES += SHAPE_GRADIENT
}
