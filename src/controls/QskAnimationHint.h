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
    enum UpdateFlag
    {
        UpdateAuto = 0, // depending on the animated aspect

        UpdateNode     = 1 << 0,
        UpdatePolish   = 1 << 1,
        UpdateSizeHint = 1 << 2,

        UpdateAll = UpdateNode | UpdatePolish | UpdateSizeHint
    };

    Q_DECLARE_FLAGS( UpdateFlags, UpdateFlag )

    inline constexpr QskAnimationHint() noexcept
        : duration( 0 )
        , type( QEasingCurve::Linear )
        , updateFlags( UpdateAuto )
    {
    }

    inline constexpr QskAnimationHint( uint duration,
            QEasingCurve::Type type = QEasingCurve::Linear ) noexcept
        : duration( duration )
        , type( type )
        , updateFlags( UpdateAuto )
    {
    }

    uint duration;
    QEasingCurve::Type type;
    UpdateFlags updateFlags;
};

Q_DECLARE_METATYPE( QskAnimationHint )
Q_DECLARE_TYPEINFO( QskAnimationHint, Q_PRIMITIVE_TYPE );
Q_DECLARE_OPERATORS_FOR_FLAGS( QskAnimationHint::UpdateFlags )

#endif
