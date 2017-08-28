/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_BOX_OPTIONS_H
#define QSK_BOX_OPTIONS_H

#include "QskGlobal.h"
#include "QskBoxColors.h"
#include "QskBorderMetrics.h"

#include <QMarginsF>

class QSK_EXPORT QskBoxOptions
{
public:
    QskBoxOptions();

    bool isVisible() const;

    QMarginsF unitedMargins() const;
    QMarginsF padding() const;

public:
    QskMargins shadows;

    QskBorderMetrics metrics;
    QskBoxColors colors;
};

#endif
