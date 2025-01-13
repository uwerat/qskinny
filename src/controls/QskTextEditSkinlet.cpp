/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskTextEditSkinlet.h"
#include "QskTextEdit.h"

#include <qfontmetrics.h>

using Q = QskTextEdit;

QSK_SYSTEM_STATE( QskTextEditSkinlet, Selected, QskAspect::FirstSystemState << 3 )

QskTextEditSkinlet::QskTextEditSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( { TextPanelRole } );
}

QskTextEditSkinlet::~QskTextEditSkinlet()
{
}

QRectF QskTextEditSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    if ( subControl == Q::TextPanel )
    {
        return contentsRect;
    }
    else if ( subControl == Q::Text )
    {
        auto rect = skinnable->subControlContentsRect( contentsRect, Q::TextPanel );
        rect = rect.marginsAdded( skinnable->marginHint( Q::Text ) );

        return rect;
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSGNode* QskTextEditSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    switch ( nodeRole )
    {
        case TextPanelRole:
        {
            return updateBoxNode( skinnable, node, Q::TextPanel );
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QSizeF QskTextEditSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& ) const
{
    if ( which != Qt::PreferredSize )
        return QSizeF();

    const auto textEdit = static_cast< const QskTextEdit* >( skinnable );

    const QFontMetricsF fm( textEdit->effectiveFont( Q::Text ) );

    int flags = Qt::TextExpandTabs;

    const auto wm = textEdit->wrapMode();

    if( wm & QskTextOptions::WordWrap )
    {
        flags |= Qt::TextWordWrap;
    }
    else if( wm & QskTextOptions::WrapAnywhere )
    {
        flags |= Qt::TextWrapAnywhere;
    }
    else if( wm & QskTextOptions::Wrap )
    {
        flags |= Qt::TextWordWrap | Qt::TextWrapAnywhere;
    }

    auto hint = fm.boundingRect( textEdit->subControlRect( Q::Text ), flags, textEdit->text() ).size();

    hint = textEdit->outerBoxSize( Q::TextPanel, hint );
    hint = hint.expandedTo( textEdit->strutSizeHint( Q::TextPanel ) );

    return hint;
}

#include "moc_QskTextEditSkinlet.cpp"
