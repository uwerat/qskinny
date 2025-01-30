/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskAbstractTextInputSkinlet.h"
#include "QskAbstractTextInput.h"

using Q = QskAbstractTextInput;

QskAbstractTextInputSkinlet::QskAbstractTextInputSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( { TextPanelRole } );
}

QskAbstractTextInputSkinlet::~QskAbstractTextInputSkinlet()
{
}

QRectF QskAbstractTextInputSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    if ( subControl == Q::TextPanel )
        return contentsRect;

    if ( subControl == Q::Text )
    {
        auto rect = skinnable->subControlContentsRect( contentsRect, Q::TextPanel );
        rect = rect.marginsAdded( skinnable->marginHint( Q::Text ) );

        return rect;
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSGNode* QskAbstractTextInputSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    switch ( nodeRole )
    {
        case TextPanelRole:
            return updateBoxNode( skinnable, node, Q::TextPanel );
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QSizeF QskAbstractTextInputSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& constraint ) const
{
    if ( which != Qt::PreferredSize )
        return QSizeF();

    Q_UNUSED( constraint ); // TODO ...

    const auto input = static_cast< const QskAbstractTextInput* >( skinnable );

    auto hint = input->unwrappedTextSize();
    hint = input->outerBoxSize( Q::TextPanel, hint );
    hint = hint.expandedTo( input->strutSizeHint( Q::TextPanel ) );

    return hint;
}

#include "moc_QskAbstractTextInputSkinlet.cpp"
