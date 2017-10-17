/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_BOX_OPTIONS_H
#define QSK_BOX_OPTIONS_H

#include "QskGlobal.h"
#include "QskBoxShapeMetrics.h"
#include "QskBoxBorderMetrics.h"
#include "QskBoxBorderColors.h"
#include "QskGradient.h"

class QSK_EXPORT QskBoxOptions
{
public:
    bool isVisible() const;

    QskBoxShapeMetrics shape;
    QskBoxBorderMetrics border;

    QskBoxBorderColors borderColors;
    QskGradient fillGradient;
};

#endif
