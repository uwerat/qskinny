/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskTextAreaSkinlet.h"
#include "QskTextArea.h"

using Q = QskTextArea;

QskTextAreaSkinlet::QskTextAreaSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( { PanelRole, TextPanelRole, PlaceholderRole } );
}

QskTextAreaSkinlet::~QskTextAreaSkinlet()
{
}

QRectF QskTextAreaSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    if ( subControl == Q::Panel )
        return contentsRect;

    if ( subControl == Q::TextPanel )
        return skinnable->subControlContentsRect( contentsRect, Q::Panel );

    if ( subControl == Q::Text )
        return skinnable->subControlContentsRect( contentsRect, Q::TextPanel );

    if ( subControl == Q::Placeholder )
    {
        const auto textArea = static_cast< const QskTextArea* >( skinnable );
        if( textArea->text().isEmpty() )
            return subControlRect( skinnable, contentsRect, Q::Text );

        return QRectF();
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSGNode* QskTextAreaSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto textArea = static_cast< const QskTextArea* >( skinnable );

    switch ( nodeRole )
    {
        case PanelRole:
        {
            return updateBoxNode( skinnable, node, Q::Panel );
        }
        case TextPanelRole:
        {
            return updateBoxNode( skinnable, node, Q::TextPanel );
        }
        case PlaceholderRole:
        {
            const auto text = effectivePlaceholderText( textArea );
            if ( text.isEmpty() )
                return nullptr;

            const auto subControl = Q::Placeholder;

            QskSkinHintStatus status;

            auto options = skinnable->textOptionsHint( subControl, &status );
            if ( !status.isValid() )
                options.setElideMode( Qt::ElideRight );

            return updateTextNode( skinnable, node,
                textArea->subControlRect( subControl ),
                textArea->alignmentHint( subControl, Qt::AlignLeft ),
                options, text, subControl );
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QSizeF QskTextAreaSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& constraint ) const
{
    Q_UNUSED( constraint ); // TODO ...

    if ( which != Qt::PreferredSize )
        return QSizeF();

    const auto input = static_cast< const QskAbstractTextInput* >( skinnable );

    auto hint = input->unwrappedTextSize();
    hint = hint.grownBy( skinnable->marginHint( Q::Text ) );

    hint = input->outerBoxSize( Q::TextPanel, hint );
    hint = hint.expandedTo( input->strutSizeHint( Q::TextPanel ) );

    hint = skinnable->outerBoxSize( Q::Panel, hint );
    hint = hint.expandedTo( skinnable->strutSizeHint( Q::Panel ) );

    return hint;
}

QString QskTextAreaSkinlet::effectivePlaceholderText(
    const QskTextArea* textArea ) const
{
    if ( textArea->text().isEmpty() &&
        !( textArea->isReadOnly() || textArea->isEditing() ) )
    {
        return textArea->placeholderText();
    }

    return QString();
}

#include "moc_QskTextAreaSkinlet.cpp"
