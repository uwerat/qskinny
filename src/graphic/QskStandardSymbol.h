/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_STANDARD_SYMBOL_H
#define QSK_STANDARD_SYMBOL_H

#include "QskGlobal.h"
#include <qmetatype.h>

class QskGraphic;

namespace QskStandardSymbol
{
    Q_NAMESPACE_EXPORT( QSK_EXPORT )

    enum Type : qint16
    {
        NoSymbol = -1,

        Ok,
        Cancel,

        CheckMark,
        CrossMark,

        SegmentedBarCheckMark,

        Bullet,

        TriangleUp,
        TriangleDown,
        TriangleLeft,
        TriangleRight,

        SymbolTypeCount
    };

    Q_ENUM_NS( Type )

    QSK_EXPORT QskGraphic graphic( Type );
}

#endif
