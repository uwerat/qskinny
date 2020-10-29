/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_RGBVALUE_QML_H
#define QSK_RGBVALUE_QML_H

#include "QskQmlGlobal.h"
#include <QskRgbValue.h>
#include <qmetatype.h>

class QskRgbValueQml
{
    Q_GADGET

  public:
    enum Enum
    {
#define RGB( name, value ) name = value,
        QSK_RGB_VALUES
#undef RGB
    };

    Q_ENUM( Enum )
};

#endif
