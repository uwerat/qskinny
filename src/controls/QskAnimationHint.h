/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_ANIMATION_HINT_H
#define QSK_ANIMATION_HINT_H

#include "QskGlobal.h"

#include <qeasingcurve.h>
#include <qmetatype.h>

class QSK_EXPORT QskAnimationHint
{
  public:
    inline constexpr QskAnimationHint() noexcept
        : duration( 0 )
        , type( QEasingCurve::Linear )
    {
    }

    inline constexpr QskAnimationHint( uint duration,
            QEasingCurve::Type type = QEasingCurve::Linear ) noexcept
        : duration( duration )
        , type( type )
    {
    }

    uint duration : 24;
    QEasingCurve::Type type : 8;
};

Q_DECLARE_METATYPE( QskAnimationHint )
Q_DECLARE_TYPEINFO( QskAnimationHint, Q_PRIMITIVE_TYPE );

#endif
