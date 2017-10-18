/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSkinRenderer.h"
#include "QskSkinnable.h"
#include "QskControl.h"
#include "QskRgbValue.h"

#include "QskTextRenderer.h"
#include "QskPlainTextRenderer.h"
#include "QskBoxOptions.h"
#include "QskTextNode.h"

#include <QMatrix4x4>

static inline qreal qskRadiusAt( const QskBoxShapeMetrics& shape, Qt::Edge edge )
{
    switch( edge )
    {
        case Qt::LeftEdge:
        {
            return qMax(
                shape.radius( Qt::TopLeftCorner ).width(),
                shape.radius( Qt::BottomLeftCorner ).width()
            );
        }
        case Qt::TopEdge:
        {
            return qMax(
                shape.radius( Qt::TopLeftCorner ).height(),
                shape.radius( Qt::TopRightCorner ).height()
            );
        }
        case Qt::RightEdge:
        {
            return qMax(
                shape.radius( Qt::TopRightCorner ).width(),
                shape.radius( Qt::BottomRightCorner ).width()
            );
        }
        case Qt::BottomEdge:
        {
            return qMax(
                shape.radius( Qt::BottomLeftCorner ).height(),
                shape.radius( Qt::BottomRightCorner ).height()
            );
        }
    }

    return 0.0;
}

QSizeF QskSkinRenderer::textSize( const QskSkinnable* skinnable,
    const QString& text, const QskTextOptions& options,
    QskAspect::Subcontrol subControl )
{
    using namespace QskAspect;

    auto font = skinnable->effectiveFont( subControl );

    if ( options.isRichText( text ) )
    {
        QskTextRenderer renderer;
        renderer.setFont( font );
        renderer.setOptions( options );

        return renderer.implicitSizeHint( text );
    }
    else
    {
        QskPlainTextRenderer renderer;
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

    if ( options.isRichText( text ) )
    {
        QskTextRenderer renderer;
        renderer.setFont( font );
        renderer.setOptions( options );

        return renderer.textRect( boundingSize, text ).size();
    }
    else
    {
        QskPlainTextRenderer renderer;
        renderer.setFont( font );
        renderer.setOptions( options );

        return renderer.textRect( boundingSize, text ).size();
    }
}

void QskSkinRenderer::updateTextAt( const QskSkinnable* skinnable,
    const QPointF& pos, const QString& text, const QskTextOptions& options,
    QskTextNode* textNode, QskAspect::Subcontrol subControl )
{
    const QRectF r( pos.x(), pos.y(), 0.0, 0.0 );
    updateText( skinnable, r, Qt::AlignLeft, text, options, textNode, subControl );
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

    const auto isRichText = options.isRichText( text ) ||
        options.format() == QskTextOptions::StyledText;

    // doesn't work - we end up with a black rectangle TODO ...
#if 0
    // Optimization: only update the color if that is all that has changed
    if ( !isRichText && ( ( skinnable->dirtyAspects() & TypeMask ) == Color ) )
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
    if ( isRichText )
        linkColor = skinnable->color( subControl | LinkColor );

    if ( textNode->setTextData( text, bounds.size(), font,
        options, alignment, textStyle, textColor, styleColor, linkColor ) )
    {
        if ( isRichText )
        {
            QskTextRenderer renderer;
            renderer.setFont( font );
            renderer.setOptions( options );
            renderer.setAlignment( alignment );

            renderer.updateNode( skinnable->owningControl(), bounds.size(),
                text, textNode, textColor, textStyle, styleColor, linkColor );
        }
        else
        {
            QskPlainTextRenderer renderer;
            renderer.setFont( font );
            renderer.setOptions( options );
            renderer.setAlignment( alignment );

            renderer.updateNode( skinnable->owningControl(), bounds.size(),
                text, textNode, textColor, textStyle, styleColor );
        }
    }
}

QskBoxOptions QskSkinRenderer::boxOptions( const QskSkinnable* skinnable,
    const QSizeF& size, QskAspect::Subcontrol subControl )
{
    using namespace QskAspect;

    QskBoxOptions options; 

    options.shape = skinnable->boxShapeHint( subControl ).toAbsolute( size );
    options.border = skinnable->boxBorderMetricsHint( subControl ).toAbsolute( size );
    options.borderColors = skinnable->boxBorderColorsHint( subControl );
    options.fillGradient = skinnable->gradientHint( subControl );

    return options;
}

QMarginsF QskSkinRenderer::paddingHint(
    const QskBoxShapeMetrics& shape, const QskBoxBorderMetrics& border,
    const QSizeF& size, bool inner )
{
    const auto shapeAbsolute = shape.toAbsolute( size );
    const auto borderAbsolute = border.toAbsolute( size );

    QMarginsF padding(
        qskRadiusAt( shapeAbsolute, Qt::LeftEdge ),
        qskRadiusAt( shapeAbsolute, Qt::TopEdge ),
        qskRadiusAt( shapeAbsolute, Qt::RightEdge ),
        qskRadiusAt( shapeAbsolute, Qt::BottomEdge )
    );

    // half of the border goes to the inner side
    const QMarginsF b = borderAbsolute.widths() * 0.5;

    /*
        not correct for calculating the outer padding,
        but to get things started. TODO ...
     */

    if ( inner )
        padding -= b;
    else
        padding += b;

    // sin 45° ceiled : 0.70710678;
    constexpr double f = 1.0 - 0.70710678;
    return f * padding;
}
