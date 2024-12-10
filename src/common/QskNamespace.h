/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_NAMESPACE_H
#define QSK_NAMESPACE_H

#include "QskGlobal.h"
#include <qmetaobject.h>

namespace Qsk
{
    Q_NAMESPACE_EXPORT( QSK_EXPORT )

    enum Policy : quint8
    {
        Maybe,
        Always,
        Never
    };
    Q_ENUM_NS( Policy )

    enum Direction : quint8
    {
        LeftToRight,
        RightToLeft,
        TopToBottom,
        BottomToTop
    };
    Q_ENUM_NS( Direction )

    enum TextStyle : quint8
    {
        Normal,
        Outline,
        Raised,
        Sunken
    };
    Q_ENUM_NS( TextStyle )

    enum Visibility : quint8
    {
        Visible = 1 << 0,
        Hidden  = 1 << 1
    };
    Q_ENUM_NS( Visibility )

    Q_DECLARE_FLAGS( Visibilities, Visibility )
    Q_DECLARE_OPERATORS_FOR_FLAGS( Visibilities )
}

#endif
