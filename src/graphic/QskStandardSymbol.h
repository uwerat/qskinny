/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_STANDARD_SYMBOL_H
#define QSK_STANDARD_SYMBOL_H 1

#include "QskGlobal.h"
#include <qmetatype.h>

class QskGraphic;

class QSK_EXPORT QskStandardSymbol
{
    Q_GADGET

  public:
    enum Type
    {
        NoSymbol = -1,

        Ok,
        Cancel,

        Information,
        Warning,
        Critical,
        Question,

        SymbolTypeCount
    };

    Q_ENUM( Type )

    static QskGraphic graphic( Type );
};

#endif
