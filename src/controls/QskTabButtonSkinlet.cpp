/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskTabButtonSkinlet.h"
#include "QskTabButton.h"

#include "QskTextOptions.h"

QskTabButtonSkinlet::QskTabButtonSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( { PanelRole, TextRole } );
}

QskTabButtonSkinlet::~QskTabButtonSkinlet() = default;

QRectF QskTabButtonSkinlet::subControlRect(
    const QskSkinnable* skinnable, QskAspect::Subcontrol subControl ) const
{
    auto tabButton = static_cast< const QskTabButton* >( skinnable );

    if ( subControl == QskTabButton::Text )
    {
        QRectF rect = subControlRect( tabButton, QskTabButton::Panel );
        rect = tabButton->innerBox( QskTabButton::Panel, rect );

        return rect;
    }
    else if ( subControl == QskTabButton::Panel )
    {
        return tabButton->contentsRect();
    }

    return Inherited::subControlRect( skinnable, subControl );
}

QSGNode* QskTabButtonSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto tabButton = static_cast< const QskTabButton* >( skinnable );

    switch ( nodeRole )
    {
        case PanelRole:
        {
            return updateBoxNode( tabButton, node, QskTabButton::Panel );
        }

        case TextRole:
        {
            return updateTextNode( tabButton, node, tabButton->text(),
                tabButton->textOptions(), QskTabButton::Text );
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

#include "moc_QskTabButtonSkinlet.cpp"
