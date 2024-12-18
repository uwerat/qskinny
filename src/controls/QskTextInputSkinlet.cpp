/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskTextInputSkinlet.h"
#include "QskTextInput.h"

#include <qfontmetrics.h>

using Q = QskTextInput;

QSK_SYSTEM_STATE( QskTextInputSkinlet, Selected, QskAspect::FirstSystemState << 3 )

QskTextInputSkinlet::QskTextInputSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( { TextPanelRole } );
}

QskTextInputSkinlet::~QskTextInputSkinlet()
{
}

QRectF QskTextInputSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    if ( subControl == Q::TextPanel )
        return contentsRect;

    if ( subControl == Q::Text )
    {
        auto rect = skinnable->subControlContentsRect( contentsRect, Q::TextPanel );

        const auto h = skinnable->effectiveFontHeight( Q::Text );
        rect.setTop( rect.center().y() - 0.5 * h );
        rect.setHeight( h );
        rect = rect.marginsAdded( skinnable->marginHint( Q::Text ) );

        return rect;
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSGNode* QskTextInputSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    switch ( nodeRole )
    {
        case TextPanelRole:
            return updateBoxNode( skinnable, node, Q::TextPanel );
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QSizeF QskTextInputSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& ) const
{
    if ( which != Qt::PreferredSize )
        return QSizeF();

    const auto text = static_cast< const QskTextInput* >( skinnable )->text();

    const QFontMetricsF fm( skinnable->effectiveFont( Q::Text ) );

    auto hint = fm.size( Qt::TextSingleLine | Qt::TextExpandTabs, text );

    hint = skinnable->outerBoxSize( Q::TextPanel, hint );
    hint = hint.expandedTo( skinnable->strutSizeHint( Q::TextPanel ) );

    return hint;
}
