/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskPushButtonSkinlet.h"
#include "QskPushButton.h"
#include "QskGraphic.h"
#include "QskTextOptions.h"

#include <qfontmetrics.h>
#include <qmath.h>

QskPushButtonSkinlet::QskPushButtonSkinlet( QskSkin* skin ):
    Inherited( skin )
{
    setNodeRoles( { PanelRole, GraphicRole, TextRole } );
}

QskPushButtonSkinlet::~QskPushButtonSkinlet() = default;

QRectF QskPushButtonSkinlet::subControlRect(
    const QskSkinnable* skinnable, QskAspect::Subcontrol subControl ) const
{
    const auto button = static_cast< const QskPushButton* >( skinnable );

    if ( subControl == QskPushButton::Text )
    {
        return textRect( button );
    }
    else if ( subControl == QskPushButton::Graphic )
    {
        return graphicRect( button );
    }
    else if ( subControl == QskPushButton::Panel )
    {
        return button->contentsRect();
    }

    return Inherited::subControlRect( skinnable, subControl );
}

QSGNode* QskPushButtonSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto button = static_cast< const QskPushButton* >( skinnable );

    switch( nodeRole )
    {
        case PanelRole:
        {
            return updateBoxNode( button, node, QskPushButton::Panel );
        }

        case TextRole:
        {
            return updateTextNode( button, node );
        }

        case GraphicRole:
        {
            return updateGraphicNode( button, node,
                button->graphic(), QskPushButton::Graphic );
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QRectF QskPushButtonSkinlet::textRect( const QskPushButton* button ) const
{
    // buttonShift -> TODO

    QRectF r = button->innerBox( QskPushButton::Panel,
        subControlRect( button, QskPushButton::Panel ) );

    if ( button->hasGraphic() )
    {
        // in case of having text + graphic we put the text at the bottom

        qreal h = QFontMetrics( button->effectiveFont( QskPushButton::Text ) ).height();
        if ( h < r.height() )
            r.setTop( r.bottom() - h );
    }

    return r;
}

QRectF QskPushButtonSkinlet::graphicRect( const QskPushButton* button ) const
{
    // buttonShift -> TODO
    QRectF r = button->innerBox( QskPushButton::Panel, button->contentsRect() );

    if ( !button->text().isEmpty() )
    {
        qreal h = textRect( button ).height()
            + button->metric( QskPushButton::Panel | QskAspect::Spacing );

        if ( h < r.height() )
            r.setBottom( r.bottom() - h );
        else
            r.setHeight( 0 );
    }

    const QSizeF sz = button->graphic().defaultSize();
    if ( r.isEmpty() || sz.isEmpty() )
        return r;

    const double scaleFactor =
        qMin( r.width() / sz.width(), r.height() / sz.height() );

    // early aligning to avoid pointless operations, that finally will
    // have no effect, when drawing to an integer based paint device

    const int w = qFloor( scaleFactor * sz.width() );
    const int h = qFloor( scaleFactor * sz.height() );
    const int x = qFloor( r.center().x() - 0.5 * w );
    const int y = qFloor( r.center().y() - 0.5 * h );

    return QRectF( x, y, w, h );
}

QSGNode* QskPushButtonSkinlet::updateTextNode(
    const QskPushButton* button, QSGNode* node ) const
{
    const auto rect = subControlRect( button, QskPushButton::Text ).toAlignedRect();

    const QFontMetricsF fm( button->effectiveFont( QskPushButton::Text ) );
    if ( !button->clip() && ( rect.height() < fm.height() ) )
        return nullptr;

    const auto alignment = button->flagHint< Qt::Alignment >(
        QskPushButton::Text | QskAspect::Alignment, Qt::AlignCenter );

    return QskSkinlet::updateTextNode( button, node, rect, alignment,
        button->text(), button->textOptions(), QskPushButton::Text );
}

#include "moc_QskPushButtonSkinlet.cpp"
