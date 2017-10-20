/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSkinRenderer.h"
#include "QskSkinnable.h"
#include "QskControl.h"

#include "QskTextRenderer.h"
#include "QskPlainTextRenderer.h"
#include "QskTextNode.h"

#include <QMatrix4x4>

QSizeF QskSkinRenderer::textSize( const QskSkinnable* skinnable,
    const QString& text, const QskTextOptions& options,
    QskAspect::Subcontrol subControl )
{
    using namespace QskAspect;

    auto font = skinnable->effectiveFont( subControl );

    if ( options.effectiveFormat( text ) == QskTextOptions::PlainText )
    {
        QskPlainTextRenderer renderer;
        renderer.setFont( font );
        renderer.setOptions( options );

        return renderer.implicitSizeHint( text );
    }
    else
    {
        QskTextRenderer renderer;
        renderer.setFont( font );
        renderer.setOptions( options );

        return renderer.implicitSizeHint( text );
    }
}

QSizeF QskSkinRenderer::textSize( const QskSkinnable* skinnable,
    const QSizeF& boundingSize, const QString& text,
    const QskTextOptions& options, QskAspect::Subcontrol subControl )
{
    const auto font = skinnable->effectiveFont( subControl );

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

void QskSkinRenderer::updateText( const QskSkinnable* skinnable,
    const QRectF& bounds, Qt::Alignment alignment,
    const QString& text, const QskTextOptions& options,
    QskTextNode* textNode, QskAspect::Subcontrol subControl )
{
    using namespace QskAspect;

    QMatrix4x4 matrix;
    matrix.translate( bounds.left(), bounds.top() );
    textNode->setMatrix( matrix );

    QskSkinHintStatus status;
    auto textColor = skinnable->color( subControl, &status );
    if ( !status.isValid() )
        textColor = skinnable->color( subControl | QskAspect::TextColor );

    auto font = skinnable->effectiveFont( subControl );

    auto styleColor = skinnable->color( subControl | StyleColor );

    auto textStyle = Qsk::Normal;
    if ( styleColor.alpha() == 0 )
    {
        textStyle = skinnable->flagHint< Qsk::TextStyle >(
            subControl | Style, Qsk::Normal );
    }

    const auto isPlainText = options.effectiveFormat( text ) == QskTextOptions::PlainText;

    // doesn't work - we end up with a black rectangle TODO ...
#if 0
    // Optimization: only update the color if that is all that has changed
    if ( isPlainText && color_is_dirty )
    {
        QskPlainTextRenderer::updateNodeColor( parentNode, textRgb,
            fontOptions.textStyle, styleRgb );
        return;
    }
#endif

    switch ( options.fontSizeMode() )
    {
        case QskTextOptions::FixedSize:
            break;

        case QskTextOptions::HorizontalFit:
            Q_UNIMPLEMENTED();
            break;

        case QskTextOptions::VerticalFit:
            font.setPixelSize( bounds.height() * 0.5 );
            break;

        case QskTextOptions::Fit:
            Q_UNIMPLEMENTED();
            break;
    }

    QColor linkColor;
    if ( !isPlainText )
        linkColor = skinnable->color( subControl | LinkColor );

    if ( textNode->setTextData( text, bounds.size(), font,
        options, alignment, textStyle, textColor, styleColor, linkColor ) )
    {
        if ( isPlainText )
        {
            QskPlainTextRenderer renderer;
            renderer.setFont( font );
            renderer.setOptions( options );
            renderer.setAlignment( alignment );

            renderer.updateNode( skinnable->owningControl(), bounds.size(),
                text, textNode, textColor, textStyle, styleColor );
        }
        else
        {
            QskTextRenderer renderer;
            renderer.setFont( font );
            renderer.setOptions( options );
            renderer.setAlignment( alignment );

            renderer.updateNode( skinnable->owningControl(), bounds.size(),
                text, textNode, textColor, textStyle, styleColor, linkColor );
        }
    }
}
