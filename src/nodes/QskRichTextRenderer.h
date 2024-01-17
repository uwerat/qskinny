/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_RICH_TEXT_RENDERER_H
#define QSK_RICH_TEXT_RENDERER_H

#include "QskNamespace.h"
#include <qnamespace.h>

class QskTextColors;
class QskTextOptions;

class QString;
class QFont;
class QRectF;
class QSizeF;
class QQuickItem;
class QSGTransformNode;

namespace QskRichTextRenderer
{
    QSK_EXPORT void updateNode(
        const QString&, const QFont&, const QskTextOptions&,
        Qsk::TextStyle, const QskTextColors&, Qt::Alignment,
        const QRectF&, const QQuickItem*, QSGTransformNode* );

    QSK_EXPORT QSizeF textSize(
        const QString&, const QFont&, const QskTextOptions& );

    QSK_EXPORT QRectF textRect(
        const QString&, const QFont&, const QskTextOptions&, const QSizeF& );
}

#endif
