/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_BOX_HINTS_H
#define QSK_BOX_HINTS_H

#include "QskBoxBorderMetrics.h"
#include "QskBoxBorderColors.h"
#include "QskBoxShapeMetrics.h"
#include "QskShadowMetrics.h"
#include "QskGradient.h"
#include <qcolor.h>

class QSK_EXPORT QskBoxHints
{
    Q_GADGET

    Q_PROPERTY( QskBoxShapeMetrics shape MEMBER shape )
    Q_PROPERTY( QskBoxBorderMetrics borderMetrics MEMBER borderMetrics )
    Q_PROPERTY( QskBoxBorderColors borderColors MEMBER borderColors )
    Q_PROPERTY( QskGradient gradient MEMBER gradient )
    Q_PROPERTY( QskShadowMetrics shadowMetrics MEMBER shadowMetrics )
    Q_PROPERTY( QColor shadowColor MEMBER shadowColor )

  public:
    QskBoxHints();
    QskBoxHints( const QskBoxShapeMetrics&, const QskBoxBorderMetrics&,
        const QskBoxBorderColors&, const QskGradient&,
        const QskShadowMetrics&, const QColor& );

    QskBoxHints toAbsolute( const QSizeF& ) const noexcept;

    QskBoxHints interpolated(
        const QskBoxHints&, qreal value ) const noexcept;

    QskBoxShapeMetrics shape;
    QskBoxBorderMetrics borderMetrics;
    QskBoxBorderColors borderColors;
    QskGradient gradient;
    QskShadowMetrics shadowMetrics;
    QColor shadowColor;
};

#ifndef QT_NO_DEBUG_STREAM

class QDebug;
QSK_EXPORT QDebug operator<<( QDebug, const QskBoxHints& );

#endif

#endif
