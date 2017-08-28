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
#include "QskBorderGeometry.h"
#include "QskBoxOptions.h"
#include "QskTextNode.h"
#include "QskBoxNode.h"

#include <QMatrix4x4>

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

static inline QMarginsF qskRotatedMargins( QMarginsF margins, int count )
{
    count = count % 4;
    if ( count < 0 )
        count += 4;
    
    // counter clockwise

    switch( count )
    {
        case 1:
        {
            return QMarginsF( margins.top(), margins.right(),
                margins.bottom(), margins.left() );
        }
        case 2:
        {
            return QMarginsF( margins.right(), margins.bottom(),
                margins.left(), margins.top() );
        }
        case 3:
        {
            return QMarginsF( margins.bottom(), margins.left(),
                margins.top(), margins.right() );
        }
        default:
            return margins;
    }
}

static inline QskAspect::Edge qskRotateEdge( QskAspect::Edge edge, int count )
{
    edge = ( count > 0 ) ? edge << ( count % 4 )
        : edge >> ( std::abs( count ) % 4 );

    if ( edge < QskAspect::LeftEdge )
        edge = edge << 4;

    if ( edge > QskAspect::BottomEdge )
        edge = edge >> 4;

    return edge;
}

QMarginsF QskSkinRenderer::margins( const QskSkinnable* skinnable,
    QskAspect::Subcontrol subControl, int rotation )
{
    const QMarginsF m = skinnable->marginsHint( subControl | QskAspect::Margin );
    return qskRotatedMargins( m, rotation );
}

static inline qreal qskRadius( const QskSkinnable* skinnable,
    const QRectF& rect, QskAspect::Aspect aspect )
{
    using namespace QskAspect;

    qreal radius = skinnable->metric( aspect );
    if ( radius <= 0.0 )
        return 0.0;

    const auto mode = skinnable->flagHint< Qt::SizeMode >(
        aspect | SizeMode, Qt::AbsoluteSize );

    if ( mode == Qt::RelativeSize )
    {
        // radius is a percentage

        if ( radius > 100.0 )
            radius = 100.0;

        radius /= 100.0;

        if ( aspect.boxPrimitive() & RadiusX )
            radius *= 0.5 * rect.width();
        else
            radius *= 0.5 * rect.height();
    }

    return radius;
}

QskBoxOptions QskSkinRenderer::boxOptions( const QskSkinnable* skinnable,
    const QRectF& rect, QskAspect::Subcontrol subControl, int rotation )
{
    using namespace QskAspect;

    QskBoxOptions options; 

    options.metrics.setWidths( qskRotatedMargins( 
        skinnable->borderMetrics( subControl ), rotation ) );

    options.shadows = qskRotatedMargins(
        skinnable->marginsHint( subControl | Shadow ), rotation );

    const auto leftEdge = qskRotateEdge( LeftEdge, rotation );
    const auto topEdge = qskRotateEdge( TopEdge, rotation );
    const auto rightEdge = qskRotateEdge( RightEdge, rotation );
    const auto bottomEdge = qskRotateEdge( BottomEdge, rotation );

    // Edge/Corner are the same bits
    const auto topLeft = static_cast<Corner>( leftEdge );
    const auto topRight = static_cast<Corner>( topEdge );
    const auto bottomRight = static_cast<Corner>( rightEdge );
    const auto bottomLeft = static_cast<Corner>( bottomEdge );

    // corner radii
    options.metrics.setRadius(
        qskRadius( skinnable, rect, subControl | RadiusX | topLeft ),
        qskRadius( skinnable, rect, subControl | RadiusY | topLeft ),
        qskRadius( skinnable, rect, subControl | RadiusX | topRight ),
        qskRadius( skinnable, rect, subControl | RadiusY | topRight ),
        qskRadius( skinnable, rect, subControl | RadiusX | bottomLeft ),
        qskRadius( skinnable, rect, subControl | RadiusY | bottomLeft ),
        qskRadius( skinnable, rect, subControl | RadiusX | bottomRight ),
        qskRadius( skinnable, rect, subControl | RadiusY | bottomRight ) );

    // border colors
    options.colors.setBorderColor(
        skinnable->color( subControl | Border | leftEdge ),
        skinnable->color( subControl | Border | topEdge ),
        skinnable->color( subControl | Border | rightEdge ),
        skinnable->color( subControl | Border | bottomEdge ) );

    // background colors
    options.colors.setFillColor(
        skinnable->color( subControl | topLeft ),
        skinnable->color( subControl | topRight ),
        skinnable->color( subControl | bottomLeft ),
        skinnable->color( subControl | bottomRight ) );

    return options;
}

void QskSkinRenderer::updateBox(
    const QskSkinnable* skinnable, QskBoxNode* node,
    const QRectF& rect, QskAspect::Subcontrol subControl, int rotation )
{
    const auto options = QskSkinRenderer::boxOptions(
        skinnable, rect, subControl, rotation );

    // The shadow is inside the margins - in case of having
    // no margins then outside the bounding rectangle.
    // Is this really how we want it ???

    QRectF boxRect = rect.marginsRemoved( margins( skinnable, subControl, rotation ) );
    boxRect = boxRect.marginsAdded( options.shadows );

    node->setBoxData( boxRect, options );
}
