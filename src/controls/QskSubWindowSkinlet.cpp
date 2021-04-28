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
    else if ( subControl == QskSubWindow::TitleBarPanel )
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
    using Q = QskSubWindow;

    const auto subWindow = static_cast< const QskSubWindow* >( skinnable );

    switch ( nodeRole )
    {
        case PanelRole:
        {
            return updateBoxNode( subWindow, node, Q::Panel );
        }
        case TitleBarRole:
        {
            const auto decorations = subWindow->decorations();

            if ( decorations & Q::TitleBar )
                return updateBoxNode( subWindow, node, Q::TitleBarPanel );

            return nullptr;
        }
        case SymbolRole:
        {
            const auto decorations = subWindow->decorations();

            if ( ( decorations & Q::TitleBar ) && ( decorations & Q::Symbol ) )
            {
                return updateGraphicNode( subWindow, node,
                    subWindow->windowIcon(), Q::TitleBarSymbol );
            }

            return nullptr;
        }
        case TitleRole:
        {
            const auto decorations = subWindow->decorations();

            if ( ( decorations & Q::TitleBar ) && ( decorations & Q::Title ) )
            {
                return updateTextNode( subWindow, node, subWindow->windowTitle(),
                    subWindow->windowTitleTextOptions(), Q::TitleBarText );
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

    const auto decorations = subWindow->decorations();
    if ( !( decorations & Q::TitleBar ) )
        return 0;

    auto height = subWindow->strutSizeHint( Q::TitleBarPanel ).height();

    if ( decorations & Q::Title )
    {
        const auto padding = subWindow->paddingHint( Q::TitleBarPanel );
        const QFontMetricsF fm( subWindow->effectiveFont( Q::TitleBarText ) );

        const qreal h = fm.height() + padding.top() + padding.bottom();
        if ( h > height )
            height = h;
    }

    return height;
}

QRectF QskSubWindowSkinlet::symbolRect(
    const QskSubWindow* subWindow, const QRectF& contentsRect ) const
{
    using Q = QskSubWindow;

    if ( !subWindow->hasDecoration( Q::Symbol ) )
        return QRectF();

    auto rect = subWindow->subControlContentsRect( contentsRect, Q::TitleBarPanel );

    if ( !rect.isEmpty() )
    {
        const auto symbol = subWindow->windowIcon();

        int w = 0;
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

    if ( !subWindow->hasDecoration( Q::Title ) )
        return QRectF();

    auto rect = subWindow->subControlContentsRect( contentsRect, Q::TitleBarPanel );

    if ( !rect.isEmpty() )
    {
        const auto symbolRect = subControlRect( subWindow, rect, Q::TitleBarSymbol );
        if ( !symbolRect.isEmpty() )
        {
            const auto spacing = subWindow->spacingHint( Q::TitleBarPanel );
            rect.setX( symbolRect.right() + spacing );
        }

#if 0
        const QFontMetricsF fm( subWindow->effectiveFont( Q::TitleBarText ) );
        rect.setHeight( fm.height() ); // TitleBarText | Alignment
#endif
    }

    return rect;
}

#include "moc_QskSubWindowSkinlet.cpp"
