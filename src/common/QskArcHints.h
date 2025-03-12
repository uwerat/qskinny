/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_ARC_HINTS_H
#define QSK_ARC_HINTS_H

#include "QskArcMetrics.h"
#include "QskGradient.h"
#include <qcolor.h>

class QSK_EXPORT QskArcHints
{
    Q_GADGET

    Q_PROPERTY( QskArcMetrics metrics MEMBER metrics )
    Q_PROPERTY( qreal borderWidth MEMBER borderWidth )
    Q_PROPERTY( QColor borderColor MEMBER borderColor )

  public:
    QskArcHints();
    QskArcHints( const QskArcMetrics&, qreal borderWidth,
        const QColor& borderColor, const QskGradient& );

    QskArcHints toAbsolute( const QSizeF& ) const noexcept;

    QskArcHints interpolated(
        const QskArcHints&, qreal value ) const noexcept;

    bool isVisible() const;

    QskArcMetrics metrics;
    qreal borderWidth = 0.0;

    QColor borderColor;
    QskGradient gradient;
};

#ifndef QT_NO_DEBUG_STREAM

class QDebug;
QSK_EXPORT QDebug operator<<( QDebug, const QskArcHints& );

#endif

#endif
