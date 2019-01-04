/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskTextNode.h"
#include "QskTextColors.h"
#include "QskTextOptions.h"
#include "QskTextRenderer.h"

#include <qfont.h>
#include <qstring.h>

static inline uint qskHash(
    const QString& text, const QSizeF& size, const QFont& font,
    const QskTextOptions& options, const QskTextColors& colors,
    Qt::Alignment alignment, Qsk::TextStyle textStyle )
{
    uint hash = 11000;

    hash = qHash( text, hash );
    hash = qHash( font, hash );
    hash = qHash( options, hash );
    hash = qHash( alignment, hash );
    hash = qHash( textStyle, hash );
    hash = colors.hash( hash );
    hash = qHashBits( &size, sizeof( QSizeF ), hash );

    return hash;
}

QskTextNode::QskTextNode()
    : m_hash( 0 )
{
}

QskTextNode::~QskTextNode()
{
}

void QskTextNode::setTextData(
    const QQuickItem* item, const QString& text, const QRectF& rect,
    const QFont& font, const QskTextOptions& options, const QskTextColors& colors,
    Qt::Alignment alignment, Qsk::TextStyle textStyle )
{
    QMatrix4x4 matrix;
    matrix.translate( rect.left(), rect.top() );

    if ( matrix != this->matrix() ) // avoid setting DirtyMatrix accidently
        setMatrix( matrix );

    const uint hash = qskHash( text, rect.size(), font,
        options, colors, alignment, textStyle );

    if ( hash != m_hash )
    {
        m_hash = hash;

        const QRectF textRect( 0, 0, rect.width(), rect.height() );

        /*
            In case of having color changes only we would could
            go a faster update path: see QskPlainTextRenderer::updateNodeColor.
            TODO ...
         */
        QskTextRenderer::updateNode( text, font, options, textStyle,
            colors, alignment, textRect, item, this );
    }
}
