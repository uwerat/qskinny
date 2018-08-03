/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskTextRenderer.h"
#include "QskPlainTextRenderer.h"
#include "QskRichTextRenderer.h"
#include "QskTextOptions.h"

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

QSizeF QskTextRenderer::textSize(
    const QString& text, const QFont& font, const QskTextOptions& options,
    const QSizeF& size )
{
    if ( options.effectiveFormat( text ) == QskTextOptions::PlainText )
        return QskPlainTextRenderer::textRect( text, font, options, size ).size();
    else
        return QskRichTextRenderer::textRect( text, font, options, size ).size();
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
