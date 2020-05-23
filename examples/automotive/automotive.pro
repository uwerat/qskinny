CONFIG += qskexample qskqvg

HEADERS += \
    ButtonBar.h \
    SoundControl.h \
    SkinFactory.h \
    DefaultSkin.h \
    OtherSkin.h \
    MainWindow.h \
    Speedometer.h \
    SpeedometerSkinlet.h \
    SpeedometerDisplay.h

SOURCES += \
    ButtonBar.cpp \
    SoundControl.cpp \
    SkinFactory.cpp \
    DefaultSkin.cpp \
    OtherSkin.cpp \
    MainWindow.cpp \
    main.cpp \
    Speedometer.cpp \
    SpeedometerSkinlet.cpp \
    SpeedometerDisplay.cpp

RESOURCES += \
    images.qrc

QVGRESOURCES += \
    qvgfiles.qrc

SVGSOURCES = \
    images/car.svg \
    images/left.svg \
    images/down.svg \
    images/right.svg \
    images/up.svg \
    images/bluetooth.svg \
    images/bookmark.svg \
    images/bus.svg \
    images/cloud.svg \
    images/compass.svg \
    images/location.svg \
    images/man.svg \
    images/menu.svg \
    images/phone.svg \
    images/plane.svg \
    images/train.svg \
    images/user.svg
