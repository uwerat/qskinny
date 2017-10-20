/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskTextNode.h"
#include "QskTextOptions.h"
#include "QskTextColors.h"
#include "QskTextRenderer.h"
#include "QskPlainTextRenderer.h"

#include <QFont>
#include <QColor>
#include <QString>

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

QskTextNode::QskTextNode():
    m_hash( 0 )
{
}

QskTextNode::~QskTextNode()
{
}

void QskTextNode::setTextData( const QQuickItem* item,
    const QString& text, const QRectF& rect, const QFont& font,
    const QskTextOptions& options, const QskTextColors& colors,
    Qt::Alignment alignment, Qsk::TextStyle textStyle )
{
    if ( m_rect != rect )
    {
        QMatrix4x4 matrix;
        matrix.translate( rect.left(), rect.top() );

        if ( matrix != this->matrix() ) // avoid setting DirtyMatrix accidently
            setMatrix( matrix );
    }

    const uint hash = qskHash( text, rect.size(), font,
        options, colors, alignment, textStyle );

    if ( hash == m_hash )
        return;

    m_hash = hash;

    const QRectF textRect( 0, 0, rect.width(), rect.height() );

    if ( options.format() == QskTextOptions::PlainText )
    {
#if 0
        if ( colors_only )
        {
            // doesn't work - we end up with a black rectangle TODO ...

            QskPlainTextRenderer::updateNodeColor( parentNode, textRgb,
                fontOptions.textStyle, styleRgb );
            return;
        }
#endif
        QskPlainTextRenderer renderer;
        renderer.setFont( font );
        renderer.setOptions( options );
        renderer.setAlignment( alignment );

        renderer.updateNode( text, textRect, textStyle, colors, item, this );
    }
    else
    {
        QskTextRenderer renderer;
        renderer.setFont( font );
        renderer.setOptions( options );
        renderer.setAlignment( alignment );

        renderer.updateNode( text, textRect, textStyle, colors, item, this );
    }
}

QSizeF QskTextNode::textSize( const QString& text,
    const QFont& font, const QskTextOptions& options )
{
    if ( options.effectiveFormat( text ) == QskTextOptions::PlainText )
    {
        QskPlainTextRenderer renderer;
        renderer.setFont( font );
        renderer.setOptions( options );

        return renderer.textSize( text );
    }
    else
    {
        QskTextRenderer renderer;
        renderer.setFont( font );
        renderer.setOptions( options );
    
        return renderer.textSize( text ); 
    }   
}

QSizeF QskTextNode::textSize( const QString& text, const QFont& font,
    const QSizeF& boundingSize, const QskTextOptions& options )
{
    if ( options.effectiveFormat( text ) == QskTextOptions::PlainText )
    {
        QskPlainTextRenderer renderer;
        renderer.setFont( font );
        renderer.setOptions( options );

        return renderer.textRect( boundingSize, text ).size();
    }
    else
    {
        QskTextRenderer renderer;
        renderer.setFont( font );
        renderer.setOptions( options );

        return renderer.textRect( boundingSize, text ).size();
    }
}

