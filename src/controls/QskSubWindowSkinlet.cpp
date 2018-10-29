/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSubWindowSkinlet.h"
#include "QskSubWindow.h"

#include "QskAspect.h"
#include "QskBoxBorderMetrics.h"
#include "QskGraphic.h"
#include "QskTextOptions.h"

#include <qfontmetrics.h>

QskSubWindowSkinlet::QskSubWindowSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    appendNodeRoles( { PanelRole, TitleBarRole, SymbolRole, TitleRole } );
}

QskSubWindowSkinlet::~QskSubWindowSkinlet() = default;

QRectF QskSubWindowSkinlet::subControlRect(
    const QskSkinnable* skinnable, QskAspect::Subcontrol subControl ) const
{
    const auto subWindow = static_cast< const QskSubWindow* >( skinnable );

    if ( subControl == QskSubWindow::Panel )
    {
        return subWindow->contentsRect();
    }
    else if ( subControl == QskSubWindow::TitleBar )
    {
        return titleBarRect( subWindow );
    }
    else if ( subControl == QskSubWindow::TitleBarSymbol )
    {
        return symbolRect( subWindow );
    }
    else if ( subControl == QskSubWindow::TitleBarText )
    {
        return titleRect( subWindow );
    }

    return Inherited::subControlRect( skinnable, subControl );
}

QSGNode* QskSubWindowSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto subWindow = static_cast< const QskSubWindow* >( skinnable );

    switch ( nodeRole )
    {
        case PanelRole:
        {
            return updateBoxNode( subWindow, node, QskSubWindow::Panel );
        }
        case TitleBarRole:
        {
            if ( subWindow->isDecorated() )
                return updateBoxNode( subWindow, node, QskSubWindow::TitleBar );

            return nullptr;
        }
        case SymbolRole:
        {
            if ( subWindow->isDecorated() )
            {
                return updateGraphicNode( subWindow, node,
                    subWindow->windowIcon(), QskSubWindow::TitleBarSymbol );
            }

            return nullptr;
        }
        case TitleRole:
        {
            if ( subWindow->isDecorated() )
            {
                return updateTextNode( subWindow, node, subWindow->windowTitle(),
                    subWindow->windowTitleTextOptions(), QskSubWindow::TitleBarText );
            }

            return nullptr;
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QRectF QskSubWindowSkinlet::titleBarRect( const QskSubWindow* subWindow ) const
{
    const auto border = subWindow->boxBorderMetricsHint( QskSubWindow::Panel );

    QRectF rect = subWindow->contentsRect().marginsRemoved( border.widths() );
    rect.setHeight( titleBarHeight( subWindow ) );

    return rect;
}

qreal QskSubWindowSkinlet::titleBarHeight( const QskSubWindow* subWindow ) const
{
    using namespace QskAspect;

    if ( !subWindow->isDecorated() )
        return 0;

    const QMarginsF margins = subWindow->marginsHint( QskSubWindow::TitleBar | Padding );
    const QFontMetricsF fm( subWindow->effectiveFont( QskSubWindow::TitleBarText ) );

    const qreal height = fm.height() + margins.top() + margins.bottom();
    const qreal minHeight = subWindow->metric( QskSubWindow::TitleBar | MinimumHeight );

    return qMax( height, minHeight );
}

QRectF QskSubWindowSkinlet::symbolRect( const QskSubWindow* subWindow ) const
{
    auto rect = subControlRect( subWindow, QskSubWindow::TitleBar );
    rect = subWindow->innerBox( QskSubWindow::TitleBar, rect );

    int w = 0;

    if ( !rect.isEmpty() )
    {
        const auto symbol = subWindow->windowIcon();
        if ( !symbol.isNull() )
            w = symbol.widthForHeight( rect.height() );

        rect.setWidth( w );
    }

    return rect;
}

QRectF QskSubWindowSkinlet::titleRect( const QskSubWindow* subWindow ) const
{
    auto rect = subControlRect( subWindow, QskSubWindow::TitleBar );
    rect = subWindow->innerBox( QskSubWindow::TitleBar, rect );

    if ( !rect.isEmpty() )
    {
        const auto spacing = subWindow->metric(
            QskSubWindow::TitleBar | QskAspect::Spacing );

        const auto symbolRect = subControlRect( subWindow, QskSubWindow::TitleBarSymbol );
        rect.setX( rect.x() + symbolRect.right() + spacing ); 

#if 0
        const QFontMetricsF fm( subWindow->effectiveFont( QskSubWindow::TitleBarText ) );
        rect.setHeight( fm.height() ); // TitleBarText | Alignment
#endif
    }

    return rect;
}

#include "moc_QskSubWindowSkinlet.cpp"
