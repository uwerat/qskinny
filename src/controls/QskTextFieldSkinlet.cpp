/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskTextFieldSkinlet.h"
#include "QskTextField.h"

#include <qfontmetrics.h>

using Q = QskTextField;

QSK_SYSTEM_STATE( QskTextFieldSkinlet, Selected, QskAspect::FirstUserState >> 1 )

QskTextFieldSkinlet::QskTextFieldSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( { PanelRole, PlaceholderTextRole, } );
}

QskTextFieldSkinlet::~QskTextFieldSkinlet()
{
}

QRectF QskTextFieldSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    if ( subControl == Q::Panel )
    {
        return contentsRect;
    }
    else if ( subControl == Q::Text )
    {
        return skinnable->subControlContentsRect( contentsRect, Q::Panel );
    }
    else if ( subControl == Q::PlaceholderText )
    {
        const auto textField = static_cast< const QskTextField* >( skinnable );
        if( textField->text().isEmpty() )
            return subControlRect( skinnable, contentsRect, Q::Text );

        return QRectF();
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSGNode* QskTextFieldSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto textField = static_cast< const QskTextField* >( skinnable );

    switch ( nodeRole )
    {
        case PanelRole:
        {
            if ( !textField->hasPanel() )
                return nullptr;

            return updateBoxNode( skinnable, node, Q::Panel );
        }
        case PlaceholderTextRole:
        {
            if ( textField->text().isEmpty()
                && !textField->placeholderText().isEmpty() )
            {
                const auto subControl = Q::PlaceholderText;

                QskSkinHintStatus status;

                auto options = skinnable->textOptionsHint( subControl, &status );
                if ( !status.isValid() )
                    options.setElideMode( Qt::ElideRight );

                return updateTextNode( skinnable, node,
                    textField->subControlRect( subControl ),
                    textField->alignmentHint( subControl, Qt::AlignLeft ),
                    options, textField->placeholderText(), subControl );
            }

            return nullptr;
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QSizeF QskTextFieldSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& ) const
{
    if ( which != Qt::PreferredSize )
        return QSizeF();

    const auto textField = static_cast< const QskTextField* >( skinnable );

    const QFontMetricsF fm( textField->effectiveFont( Q::Text ) );

    auto hint = fm.size( Qt::TextSingleLine | Qt::TextExpandTabs, textField->text() );

    if ( textField->hasPanel() )
    {
        hint = textField->outerBoxSize( Q::Panel, hint );
        hint = hint.expandedTo( textField->strutSizeHint( Q::Panel ) );
    }

    return hint;
}

#include "moc_QskTextFieldSkinlet.cpp"
