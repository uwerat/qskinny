/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskTabButtonSkinlet.h"
#include "QskTabButton.h"

#include "QskTextOptions.h"
#include <qfontmetrics.h>

QskTabButtonSkinlet::QskTabButtonSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( { PanelRole, TextRole } );
}

QskTabButtonSkinlet::~QskTabButtonSkinlet() = default;

QRectF QskTabButtonSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    if ( subControl == QskTabButton::Text )
    {
        return skinnable->subControlContentsRect( contentsRect, QskTabButton::Panel );
    }
    else if ( subControl == QskTabButton::Panel )
    {
        return contentsRect;
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
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
            return updateTextNode( tabButton, node,
                tabButton->text(), QskTabButton::Text );
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QSizeF QskTabButtonSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& ) const
{
    using Q = QskTabButton;

    if ( which != Qt::PreferredSize )
        return QSizeF();

    const auto tabButton = static_cast< const QskTabButton* >( skinnable );

    QSizeF size;
    const auto text = tabButton->text();

    if ( !text.isEmpty() )
    {
        const QFontMetricsF fm( tabButton->effectiveFont( QskTabButton::Text ) );
        size = fm.size( Qt::TextShowMnemonic, text );
    }

    size = tabButton->outerBoxSize( Q::Panel, size );
    size = size.expandedTo( tabButton->strutSizeHint( Q::Panel ) );
    size = size.grownBy( skinnable->marginHint( Q::Panel ) );

    return size;
}

#include "moc_QskTabButtonSkinlet.cpp"
