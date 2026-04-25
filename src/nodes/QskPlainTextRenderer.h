/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_PLAIN_TEXT_RENDERER_H
#define QSK_PLAIN_TEXT_RENDERER_H

#include "QskNamespace.h"
#include "QskTextOptions.h"
#include <qnamespace.h>

class QskTextColors;

class QString;
class QFont;
class QRectF;
class QSizeF;
class QQuickItem;
class QColor;
class QSGTransformNode;
class QSGNode;

namespace QskPlainTextRenderer
{
    QSK_EXPORT void updateNode(
        const QString&, const QFont&, const QskTextOptions&,
        Qsk::TextStyle, const QskTextColors&, Qt::Alignment, const QRectF&,
        const QQuickItem*, QSGTransformNode* );

    QSK_EXPORT void updateNodeColor(
        QSGNode* parentNode, const QColor& textColor,
        Qsk::TextStyle, const QColor& styleColor );

    QSK_EXPORT qreal textWidth( const QString&,
        const QFont&, const QskTextOptions& = QskTextOptions() );

    QSK_EXPORT QSizeF textSize( const QString&,
        const QFont&, const QskTextOptions& = QskTextOptions() );

    QSK_EXPORT QRectF textRect( const QString&, const QFont&,
        const QskTextOptions&, Qt::Alignment, const QSizeF& );
}

#endif
