/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "LightDisplaySkinlet.h"
#include "LightDisplay.h"

#include <QskTextOptions.h>

#include <QFontMetrics>

LightDisplaySkinlet::LightDisplaySkinlet( QskSkin* skin )
    : QskSkinlet( skin )
{
    setNodeRoles( { GrooveRole, WarmPartRole, ColdPartRole, ValueTextRole,
                  LeftLabelRole, RightLabelRole } );
}

LightDisplaySkinlet::~LightDisplaySkinlet()
{
}

QRectF LightDisplaySkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    auto* display = static_cast< const LightDisplay* >( skinnable );
    QRectF rect = contentsRect;

    if( subControl == LightDisplay::Groove )
    {
        QSizeF size = textLabelsSize( display );

        const qreal x = size.width();
        const qreal w = contentsRect.width() - 2 * size.width();
        const qreal y = 0;
        const qreal h = contentsRect.height();

        const qreal diameter = qMin( w, h );

        rect = QRectF( x, y, diameter, diameter );
        return rect;
    }
    else if( subControl == LightDisplay::LeftLabel )
    {
        QRectF grooveRect = subControlRect( skinnable, contentsRect, LightDisplay::Groove );
        QSizeF size = textLabelsSize( display );

        rect.setWidth( size.width() );

        rect.setY( grooveRect.y() + ( grooveRect.height() - size.height() ) / 2 );
        rect.setHeight( size.height() );

        return rect;
    }
    else if( subControl == LightDisplay::RightLabel )
    {
        QRectF grooveRect = subControlRect( skinnable, contentsRect, LightDisplay::Groove );
        QSizeF size = textLabelsSize( display );

        rect.setX( rect.right() - size.width() );

        rect.setY( grooveRect.y() + ( grooveRect.height() - size.height() ) / 2 );
        rect.setHeight( size.height() );

        return rect;
    }

    return contentsRect;
}

QSGNode* LightDisplaySkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    switch( nodeRole )
    {
        case GrooveRole:
        {
            return updateArcNode( skinnable, node, 0, 5760,
                                  LightDisplay::Groove );
        }
        case WarmPartRole:
        {
//            const qreal startAngle = 90 * 16;
//            const auto bar = static_cast< const LightDisplay* >( skinnable );
//            const qreal spanAngle = bar->valueAsRatio() * -5760;
//            return updateArcNode( skinnable, node, startAngle, spanAngle,
//                LightDisplay::Bar );
        }
        case ColdPartRole:
        {
            return nullptr;
        }
        case ValueTextRole:
        {

            return nullptr;
        }
        case LeftLabelRole:
        {
            return updateTextNode( skinnable, node, QStringLiteral( "0" ), {},
                                   LightDisplay::LeftLabel );
        }
        case RightLabelRole:
        {
            return updateTextNode( skinnable, node, QStringLiteral( "100" ), {},
                                   LightDisplay::RightLabel );
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QSizeF LightDisplaySkinlet::textLabelsSize( const LightDisplay* display ) const
{
    QFont font = display->effectiveFont( LightDisplay::LeftLabel );
    QFontMetricsF fm( font );

    qreal w = fm.width( QStringLiteral( "100" ) );
    qreal h = fm.height();
    return { w, h };
}

#include "moc_LightDisplaySkinlet.cpp"
