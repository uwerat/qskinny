/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskTextRenderer.h"
#include "QskPlainTextRenderer.h"
#include "QskRichTextRenderer.h"
#include "QskTextOptions.h"
#include "QskFunctions.h"

#include <qrect.h>

/*
    Since Qt 5.7 QQuickTextNode is exported as Q_QUICK_PRIVATE_EXPORT
    and could be used. TODO ...
 */
QSizeF QskTextRenderer::textSize(
    const QString& text, const QFont& font, const QskTextOptions& options )
{
    if ( options.effectiveFormat( text ) == QskTextOptions::PlainText )
        return QskPlainTextRenderer::textSize( text, font, options );
    else
        return QskRichTextRenderer::textSize( text, font, options );
}

qreal QskTextRenderer::textHeightForWidth( const QString& text,
    const QFont& font, const QskTextOptions& options, qreal width )
{
    qreal height;

    if ( options.effectiveFormat( text ) == QskTextOptions::PlainText )
    {
        height = QskPlainTextRenderer::textHeightForWidth(
            text, font, options, width );
    }
    else
    {
        height = QskRichTextRenderer::textHeightForWidth(
            text, font, options, width );
    }
    return height;
}

qreal QskTextRenderer::textWidthForHeight( const QString& text,
    const QFont& font, const QskTextOptions& options, qreal height )
{
    const qreal lineHeight = qskFontHeight( font );
    const int lineCount = qFloor( height / lineHeight );

    auto textOptions = options;
    if ( lineCount < textOptions.maximumLineCount() )
        textOptions.setMaximumLineCount( lineCount );

    return textSize( text, font, textOptions ).width();
}

void QskTextRenderer::updateNode(
    const QString& text, const QFont& font, const QskTextOptions& options,
    Qsk::TextStyle style, const QskTextColors& colors, Qt::Alignment alignment,
    const QRectF& rect, const QQuickItem* item, QSGTransformNode* node )
{
    if ( options.format() == QskTextOptions::PlainText )
    {
        QskPlainTextRenderer::updateNode(
            text, font, options, style, colors, alignment, rect, item, node );
    }
    else
    {
        QskRichTextRenderer::updateNode(
            text, font, options, style, colors, alignment, rect, item, node );
    }
}
