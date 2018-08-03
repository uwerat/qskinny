/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_PLAIN_TEXT_RENDERER_H
#define QSK_PLAIN_TEXT_RENDERER_H

#include "QskNamespace.h"
#include <qnamespace.h>

class QskTextColors;
class QskTextOptions;

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

    QSK_EXPORT QSizeF textSize( const QString&,
        const QFont&, const QskTextOptions& );

    QSK_EXPORT QRectF textRect( const QString&,
        const QFont&, const QskTextOptions&, const QSizeF& );
}

#endif
