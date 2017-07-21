/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskTabButtonSkinlet.h"
#include "QskTabButton.h"
#include "QskTextRenderer.h"
#include "QskTabBar.h"
#include "QskTabButton.h"
#include "QskTextOptions.h"

QskTabButtonSkinlet::QskTabButtonSkinlet( QskSkin* skin ):
    Inherited( skin )
{
    setNodeRoles( { ButtonRole, TextRole } );
}

QskTabButtonSkinlet::~QskTabButtonSkinlet() = default;

QRectF QskTabButtonSkinlet::subControlRect(
    const QskSkinnable* skinnable, QskAspect::Subcontrol subControl ) const
{
    auto tabButton = static_cast< const QskTabButton* >( skinnable );

    if ( subControl == QskTabButton::Text )
    {
        return textRect( tabButton );
    }
    else if ( subControl == QskTabButton::Panel )
    {
        return panelRect( tabButton );
    }

    return Inherited::subControlRect( skinnable, subControl );
}

QRectF QskTabButtonSkinlet::textRect( const QskTabButton* tabButton ) const
{
    QRectF rect = subControlRect( tabButton, QskTabButton::Panel );

#if 1
    // the margins might depend on the state ???
    rect = tabButton->innerBox( QskTabButton::Panel, rect );
#endif

    return rect;
}

QRectF QskTabButtonSkinlet::panelRect( const QskTabButton* tabButton ) const
{
    return tabButton->contentsRect();
}   

QSGNode* QskTabButtonSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto tabButton = static_cast< const QskTabButton* >( skinnable );

    switch( nodeRole )
    {
        case ButtonRole:
            return updateButtonNode( tabButton, node );

        case TextRole:
            return updateTextNode( tabButton, node );

        default:
            return nullptr;
    }
}

QSGNode* QskTabButtonSkinlet::updateButtonNode(
    const QskTabButton* tabButton, QSGNode* node ) const
{
    const auto tabBar = tabButton->tabBar();
    const Qt::Orientation o = tabBar ? tabBar->orientation() : Qt::Horizontal;

    return updateBoxNode( tabButton, node, tabButton->contentsRect(),
        QskTabButton::Panel, ( o == Qt::Vertical ) ? 1 : 0 );
}

QSGNode* QskTabButtonSkinlet::updateTextNode(
    const QskTabButton* tabButton, QSGNode* node ) const
{
    return QskSkinlet::updateTextNode( tabButton, node, tabButton->text(),
        tabButton->textOptions(), QskTabButton::Text );
}

#include "moc_QskTabButtonSkinlet.cpp"
