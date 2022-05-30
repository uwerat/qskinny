/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "LightDisplaySkinlet.h"
#include "LightDisplay.h"

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
    #include "nodes/BoxShadowNode.h"
#endif

#include "nodes/RadialTickmarksNode.h"

#include <QskArcMetrics.h>
#include <QskTextOptions.h>
#include <QskScaleTickmarks.h>

#include <QFontMetrics>
#include <QtMath>

LightDisplaySkinlet::LightDisplaySkinlet( QskSkin* skin )
    : QskSkinlet( skin )
{
    setNodeRoles( { GrooveRole, PanelRole, ColdAndWarmArcRole, TickmarksRole,
                    ValueTextRole, LeftLabelRole, RightLabelRole, KnobRole } );
}

LightDisplaySkinlet::~LightDisplaySkinlet()
{
}

QRectF LightDisplaySkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    auto* display = static_cast< const LightDisplay* >( skinnable );
    QRectF rect = contentsRect;
    const qreal ticksSpacing = 4; // space between the ticks and the arc

    if( subControl == LightDisplay::Groove || subControl == LightDisplay::Panel )
    {
        QSizeF textSize = textLabelsSize( display );
        QskArcMetrics arcMetrics = display->arcMetricsHint( LightDisplay::ColdAndWarmArc );
        const qreal ticksWidth = display->arcMetricsHint( LightDisplay::Tickmarks ).width() + ticksSpacing;

        const qreal x = textSize.width() + arcMetrics.width() + ticksWidth;
        const qreal w = contentsRect.width() -
            ( 2 * ( textSize.width() + arcMetrics.width() + ticksWidth ) );
        const qreal y = arcMetrics.width() + ticksWidth;
        const qreal h = contentsRect.height() - 2 * ( arcMetrics.width() + ticksWidth );

        const qreal diameter = qMin( w, h );

        rect = QRectF( x, y, diameter, diameter );
        return rect;
    }
    else if( subControl == LightDisplay::ColdAndWarmArc )
    {
        const QRectF panelRect = subControlRect( skinnable, contentsRect, LightDisplay::Panel );
        auto barWidth = display->arcMetricsHint( LightDisplay::ColdAndWarmArc ).width();
        auto rect = panelRect.marginsAdded( { barWidth, barWidth, barWidth, barWidth } );
        return rect;
    }
    else if( subControl == LightDisplay::Tickmarks )
    {
        const QRectF arcRect = subControlRect(
            skinnable, contentsRect, LightDisplay::ColdAndWarmArc );
        const qreal ticksWidth = display->arcMetricsHint(
            LightDisplay::Tickmarks ).width() + ticksSpacing;
        const QRectF rect =
            arcRect.marginsAdded( { ticksWidth, ticksWidth, ticksWidth, ticksWidth } );
        return rect;
    }
    else if( subControl == LightDisplay::ValueText )
    {
        QRectF valueTextRect = subControlRect( skinnable, contentsRect, LightDisplay::Panel );
        const QFontMetricsF fm( skinnable->effectiveFont( subControl ) );
        const qreal fontWidth = qskHorizontalAdvance( fm, QStringLiteral( "100 %" ) );
        const QPointF center = valueTextRect.center();
        const QRectF rect( center.x() - fontWidth / 2, center.y() - fm.height() / 2, fontWidth, fm.height() );
        return rect;
    }
    else if( subControl == LightDisplay::LeftLabel )
    {
        const QRectF ticksRect = subControlRect( skinnable, contentsRect, LightDisplay::Tickmarks );
        QSizeF size = textLabelsSize( display );

        rect.setWidth( size.width() );

        rect.setY( ticksRect.y() + ( ticksRect.height() - size.height() ) / 2 );
        rect.setHeight( size.height() );

        return rect;
    }
    else if( subControl == LightDisplay::RightLabel )
    {
        QRectF ticksRect = subControlRect( skinnable, contentsRect, LightDisplay::Tickmarks );
        QSizeF size = textLabelsSize( display );

        rect.setX( ticksRect.x() + ticksRect.width() );

        rect.setY( ticksRect.y() + ( ticksRect.height() - size.height() ) / 2 );
        rect.setHeight( size.height() );

        return rect;
    }
    else if( subControl == LightDisplay::Knob )
    {
        QRectF arcRect = subControlRect( skinnable, contentsRect, LightDisplay::ColdAndWarmArc );
        QskArcMetrics arcMetrics = display->arcMetricsHint( LightDisplay::ColdAndWarmArc );
        QSizeF knobSize = display->strutSizeHint( LightDisplay::Knob );

        const qreal radius = ( arcRect.width() - arcMetrics.width() ) / 2;
        const qreal angle = display->valueAsRatio() * 180;

        const qreal cos = qFastCos( qDegreesToRadians( angle ) );
        const qreal sin = qFastSin( qDegreesToRadians( angle ) );

        const auto x = arcRect.center().x() - knobSize.width() / 2 - radius * cos;
        const auto y = arcRect.center().y() - knobSize.height() / 2 - radius * sin;

        rect = QRectF( x, y, knobSize.width(), knobSize.height() );
        return rect;
    }

    return contentsRect;
}

QSGNode* LightDisplaySkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto* display = static_cast< const LightDisplay* >( skinnable );

    switch( nodeRole )
    {
        case PanelRole:
        {
            return updateBoxNode( skinnable, node, LightDisplay::Panel );
        }
#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
        case GrooveRole:
        {
            const QRectF grooveRect = display->subControlRect( LightDisplay::Groove );
            if ( grooveRect.isEmpty() )
                return nullptr;

            auto shadowNode = static_cast< BoxShadowNode* >( node );
            if ( shadowNode == nullptr )
                shadowNode = new BoxShadowNode();

            const auto& shadowMetrics = display->shadow();

            shadowNode->setRect( shadowMetrics.shadowRect( grooveRect ) );
            shadowNode->setShape( grooveRect.width() / 2 );
            shadowNode->setBlurRadius( shadowMetrics.blurRadius() );
            shadowNode->setColor( display->shadowColor() );
            shadowNode->setClipRect( grooveRect );

            shadowNode->updateGeometry();

            return shadowNode;
        }
#endif
        case ColdAndWarmArcRole:
        {
            return updateArcNode( skinnable, node, LightDisplay::ColdAndWarmArc );
        }
        case TickmarksRole:
        {
            auto ticksNode = static_cast< RadialTickmarksNode* >( node );
            if ( ticksNode == nullptr )
                ticksNode = new RadialTickmarksNode();

            const auto color = display->color( LightDisplay::Tickmarks );
            const auto ticksRect = display->subControlRect( LightDisplay::Tickmarks );
            const auto arcMetrics = display->arcMetricsHint( LightDisplay::Tickmarks );

            QskScaleTickmarks tickmarks;
            tickmarks.setMajorTicks( { 0, 22.5, 45, 67.5, 90, 112.5, 135, 157.5, 180 } );

            const auto tickLineWidth = display->metric( LightDisplay::Tickmarks );

            ticksNode->update( color, ticksRect, arcMetrics, tickmarks, tickLineWidth );

            return ticksNode;
        }
        case ValueTextRole:
        {
            const QString valueText = QString::number( display->value(), 'f', 0 )
                    + QStringLiteral( " %" );
            return updateTextNode( skinnable, node, valueText, {},
                                   LightDisplay::ValueText );
        }
        case LeftLabelRole:
        {
            return updateTextNode( skinnable, node, QStringLiteral( "0  " ), {},
                                   LightDisplay::LeftLabel );
        }
        case RightLabelRole:
        {
            return updateTextNode( skinnable, node, QStringLiteral( "  100" ), {},
                                   LightDisplay::RightLabel );
        }
        case KnobRole:
        {
            return updateBoxNode( skinnable, node, LightDisplay::Knob );
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QSizeF LightDisplaySkinlet::textLabelsSize( const LightDisplay* display ) const
{
    const QFontMetricsF fm( display->effectiveFont( LightDisplay::LeftLabel ) );

    qreal w = qskHorizontalAdvance( fm, QStringLiteral( "  100" ) );
    qreal h = fm.height();

    return { w, h };
}

#include "moc_LightDisplaySkinlet.cpp"
