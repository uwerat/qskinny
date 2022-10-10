/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_STANDARD_SYMBOL_H
#define QSK_STANDARD_SYMBOL_H

#include "QskGlobal.h"
#include <qmetatype.h>

class QskGraphic;

namespace QskStandardSymbol
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    Q_NAMESPACE_EXPORT(QSK_EXPORT)
#else
    QSK_EXPORT Q_NAMESPACE
#endif

    enum Type
    {
        NoSymbol = -1,

        Ok,
        Cancel,

        Information,
        Warning,
        Critical,
        Question,

        CheckMark,
        CrossMark,

        SymbolTypeCount
    };

    Q_ENUM_NS( Type )

    QSK_EXPORT QskGraphic graphic( Type );
}

#endif
