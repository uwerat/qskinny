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

QRectF QskSubWindowSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    const auto subWindow = static_cast< const QskSubWindow* >( skinnable );

    if ( subControl == QskSubWindow::Panel )
    {
        return contentsRect;
    }
    else if ( subControl == QskSubWindow::TitleBar )
    {
        return titleBarRect( subWindow, contentsRect );
    }
    else if ( subControl == QskSubWindow::TitleBarSymbol )
    {
        return symbolRect( subWindow, contentsRect );
    }
    else if ( subControl == QskSubWindow::TitleBarText )
    {
        return titleRect( subWindow, contentsRect );
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
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

QRectF QskSubWindowSkinlet::titleBarRect(
    const QskSubWindow* subWindow, const QRectF& contentsRect ) const
{
    const auto border = subWindow->boxBorderMetricsHint( QskSubWindow::Panel );

    QRectF r = contentsRect.marginsRemoved( border.widths() );
    r.setHeight( titleBarHeight( subWindow ) );

    return r;
}

qreal QskSubWindowSkinlet::titleBarHeight( const QskSubWindow* subWindow ) const
{
    using Q = QskSubWindow;

    if ( !subWindow->isDecorated() )
        return 0;

    const auto margins = subWindow->paddingHint( Q::TitleBar );
    const QFontMetricsF fm( subWindow->effectiveFont( Q::TitleBarText ) );

    const qreal height = fm.height() + margins.top() + margins.bottom();
    const auto strutSize = subWindow->strutSizeHint( Q::TitleBar );

    return qMax( height, strutSize.height() );
}

QRectF QskSubWindowSkinlet::symbolRect(
    const QskSubWindow* subWindow, const QRectF& contentsRect ) const
{
    using Q = QskSubWindow;

    auto rect = subControlRect( subWindow, contentsRect, Q::TitleBar );
    rect = subWindow->innerBox( Q::TitleBar, rect );

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

QRectF QskSubWindowSkinlet::titleRect(
    const QskSubWindow* subWindow, const QRectF& contentsRect ) const
{
    using Q = QskSubWindow;

    auto rect = subControlRect( subWindow, contentsRect, Q::TitleBar );
    rect = subWindow->innerBox( Q::TitleBar, rect );

    if ( !rect.isEmpty() )
    {
        const auto spacing = subWindow->spacingHint( Q::TitleBar );
        const auto symbolRect = subControlRect( subWindow, rect, Q::TitleBarSymbol );

        rect.setX( rect.x() + symbolRect.right() + spacing );

#if 0
        const QFontMetricsF fm( subWindow->effectiveFont( Q::TitleBarText ) );
        rect.setHeight( fm.height() ); // TitleBarText | Alignment
#endif
    }

    return rect;
}

#include "moc_QskSubWindowSkinlet.cpp"
