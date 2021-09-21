/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_NAMESPACE_H
#define QSK_NAMESPACE_H

#include "QskGlobal.h"
#include <qmetaobject.h>

#if QT_VERSION < QT_VERSION_CHECK( 5, 8, 0 )
    // hack to run moc over a namespace

    #ifdef Q_MOC_RUN

        #define QSK_NAMESPACE( name ) struct name
        #define QSK_ENUM( name ) Q_GADGET Q_ENUM( name )

    #else

        #define QSK_NAMESPACE( name ) namespace name
        #define QSK_ENUM( name ) \
            inline constexpr const QMetaObject* qt_getEnumMetaObject(name) noexcept { return &staticMetaObject; } \
            inline constexpr const char* qt_getEnumName(name) noexcept { return #name; }

    #endif

    #define QSK_Q_NAMESPACE extern const QMetaObject staticMetaObject;

#else

    #define QSK_NAMESPACE( name ) namespace name
    #define QSK_ENUM Q_ENUM_NS

    #define QSK_Q_NAMESPACE Q_NAMESPACE
#endif

QSK_NAMESPACE( Qsk )
{
    QSK_EXPORT QSK_Q_NAMESPACE

    enum Direction
    {
        LeftToRight,
        RightToLeft,
        TopToBottom,
        BottomToTop
    };
    QSK_ENUM( Direction )

    enum Position
    {
        Top,
        Left,
        Right,
        Bottom
    };
    QSK_ENUM( Position )

    enum TextStyle
    {
        Normal,
        Outline,
        Raised,
        Sunken
    };
    QSK_ENUM( TextStyle )
}

#undef QSK_NAMESPACE
#undef QSK_ENUM
#undef QSK_Q_NAMESPACE

#endif
