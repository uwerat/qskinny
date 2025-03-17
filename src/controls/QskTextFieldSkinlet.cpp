/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskTextFieldSkinlet.h"
#include "QskTextField.h"
#include "QskFunctions.h"

using Q = QskTextField;

QskTextFieldSkinlet::QskTextFieldSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( { TextPanelRole, IconRole, ButtonPanelRole, ButtonRole,
        PlaceholderRole, HeaderRole, FooterRole } );
}

QskTextFieldSkinlet::~QskTextFieldSkinlet()
{
}

QRectF QskTextFieldSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    const auto textField = static_cast< const QskTextField* >( skinnable );

    if ( subControl == Q::Panel )
    {
        return contentsRect;
    }

    if ( subControl == Q::Header )
    {
        if ( auto h = effectiveHeaderHeight( textField ) )
        {
            const auto m = textField->marginHint( Q::Header );
            const auto r = subControlRect( skinnable, contentsRect, Q::TextPanel );

            return QRectF( r.left() + m.left(),
                r.top() - m.bottom() - h, r.width() - m.left() - m.right(), h );
        }

        return QRectF();
    }

    if ( subControl == Q::Footer )
    {
        if ( const auto h = effectiveFooterHeight( textField ) )
        {
            const auto m = textField->marginHint( Q::Footer );
            const auto r = subControlRect( skinnable, contentsRect, Q::TextPanel );

            return QRectF( r.left() + m.left(),
                r.bottom() + m.top(), r.width() - m.left() - m.right(), h );
        }

        return QRectF();
    }

    if ( subControl == Q::TextPanel )
    {
        const auto rect = textField->subControlContentsRect( contentsRect, Q::Panel );
        return inputPanelRect( textField, rect );
    }

    if ( subControl == Q::Text )
    {
        auto rect = textField->subControlContentsRect( contentsRect, Q::TextPanel );

        const auto iconRect = subControlRect( skinnable, contentsRect, Q::Icon );
        if ( !iconRect.isEmpty() )
            rect.setLeft( iconRect.right() );

        const auto buttonRect = subControlRect( skinnable, contentsRect, Q::Button );
        if( !buttonRect.isEmpty() )
            rect.setRight( buttonRect.left() );

        const auto h = skinnable->effectiveFontHeight( Q::Text );
        rect.setTop( rect.center().y() - 0.5 * h );
        rect.setHeight( h );

        return rect;
    }

    if ( subControl == Q::Placeholder )
    {
        if( textField->text().isEmpty() )
            return subControlRect( skinnable, contentsRect, Q::Text );

        return QRectF();
    }

    if ( subControl == Q::Icon )
    {
        if( !skinnable->symbolHint( subControl ).isEmpty() )
        {
            const auto rect = textField->subControlContentsRect( contentsRect, Q::TextPanel );

            return qskAlignedRectF( rect,
                skinnable->strutSizeHint( Q::Icon ), Qt::AlignLeft | Qt::AlignVCenter );
        }

        return QRectF();
    }

    if ( subControl == Q::ButtonPanel )
    {
        if ( textField->buttonStates() & Q::Hovered )
        {
            const auto sz = skinnable->strutSizeHint( Q::ButtonPanel );
            if ( !sz.isEmpty() )
            {
                const auto r = subControlRect( skinnable, contentsRect, Q::Button );

                QRectF rect( QPointF(), sz );
                rect.moveCenter( r.center() );

                return rect;
            }
        }

        return QRectF();
    }

    if ( subControl == Q::Button )
    {
        if( !skinnable->symbolHint( Q::Button ).isEmpty() )
        {
            const auto rect = textField->subControlContentsRect( contentsRect, Q::TextPanel );

            return qskAlignedRectF( rect,
                skinnable->strutSizeHint( Q::Icon ), Qt::AlignRight | Qt::AlignVCenter );
        }

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
        case TextPanelRole:
        {
            return updateBoxNode( skinnable, node, Q::TextPanel );
        }
        case PlaceholderRole:
        {
            const auto subControl = Q::Placeholder;

            const auto text = effectiveText( textField, subControl );
            if ( text.isEmpty() )
                return nullptr;

            QskSkinHintStatus status;

            auto options = skinnable->textOptionsHint( subControl, &status );
            if ( !status.isValid() )
                options.setElideMode( Qt::ElideRight );

            return updateTextNode( skinnable, node,
                textField->subControlRect( subControl ),
                textField->alignmentHint( subControl, Qt::AlignLeft ),
                options, text, subControl );
        }

        case HeaderRole:
        {
            return updateTextNode( skinnable, node,
                effectiveText( textField, Q::Header ), Q::Header );
        }

        case FooterRole:
        {
            return updateTextNode( skinnable, node,
                effectiveText( textField, Q::Footer ), Q::Footer );
        }

        case IconRole:
            return updateSymbolNode( skinnable, node, Q::Icon );

        case ButtonPanelRole:
            return updateBoxNode( skinnable, node, Q::ButtonPanel );

        case ButtonRole:
            return updateSymbolNode( skinnable, node, Q::Button );
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QRectF QskTextFieldSkinlet::inputPanelRect(
    const QskTextField* textField, const QRectF& rect ) const
{
    qreal h = textField->effectiveFontHeight( Q::Text );
    h = textField->outerBoxSize( Q::TextPanel, QSizeF( rect.width(), h ) ).height();
    h = qMax( h, textField->strutSizeHint( Q::TextPanel ).height() );

    /*
        when having textfields in horizontal layouts you usually want
        the text panels being vertically aligned - regardless of having
        Q::Header/Q::Footer being available.
     */

    auto top = textHeight( textField, Q::Header );
    auto bottom = textHeight( textField, Q::Footer );

    if ( rect.height() < top + h + bottom )
    {
        if ( effectiveText( textField, Q::Footer ).isEmpty() )
            bottom = 0.0;
    }

    if ( rect.height() < top + h + bottom )
    {
        if ( effectiveText( textField, Q::Header ).isEmpty() )
            top = 0.0;
    }

    return QRectF( rect.left(), rect.top() + top, rect.width(), h );
}

QSizeF QskTextFieldSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& constraint ) const
{
    Q_UNUSED( constraint ); // TODO ...

    if ( which != Qt::PreferredSize )
        return QSizeF();

    const auto textField = static_cast< const QskTextField* >( skinnable );

    auto hint = textField->unwrappedTextSize();

    if( !skinnable->symbolHint( Q::Button ).isEmpty() )
    {
        const auto sz = skinnable->strutSizeHint( Q::Button );
        if ( sz.width() > 0.0 )
            hint.rwidth() += sz.width();
    }

    hint = skinnable->outerBoxSize( Q::TextPanel, hint );
    hint = hint.expandedTo( skinnable->strutSizeHint( Q::TextPanel ) );

    hint.rheight() += effectiveHeaderHeight( textField );
    hint.rheight() += effectiveFooterHeight( textField );

    hint = skinnable->outerBoxSize( Q::Panel, hint );
    hint = hint.expandedTo( skinnable->strutSizeHint( Q::Panel ) );

    return hint;
}

qreal QskTextFieldSkinlet::textHeight( const QskTextField* textField,
    QskAspect::Subcontrol subControl ) const
{
    auto h = textField->effectiveFontHeight( subControl );

    const auto margins = textField->marginHint( subControl );
    h += margins.top() + margins.bottom();

    const auto sz = textField->strutSizeHint( subControl );

    return qMax( h, sz.height() );
}

QString QskTextFieldSkinlet::effectiveText(
    const QskTextField* textField, QskAspect::Subcontrol subcontrol ) const
{
    if ( subcontrol == Q::Text )
        return textField->text();

    if ( subcontrol == Q::Placeholder )
    {
        if ( textField->text().isEmpty() &&
            !( textField->isReadOnly() || textField->isEditing() ) )
        {
            return textField->placeholderText();
        }

        return QString();
    }

    if ( subcontrol == Q::Header )
        return textField->headerText();

    if ( subcontrol == Q::Footer )
        return textField->footerText();

    return QString();
}

qreal QskTextFieldSkinlet::effectiveHeaderHeight( const QskTextField* textField ) const
{
    const auto text = effectiveText( textField, Q::Header );
    return text.isEmpty() ? 0.0 : textHeight( textField, Q::Header );
}

qreal QskTextFieldSkinlet::effectiveFooterHeight( const QskTextField* textField ) const
{
    const auto text = effectiveText( textField, Q::Footer );
    return text.isEmpty() ? 0.0 : textHeight( textField, Q::Footer );
}

#include "moc_QskTextFieldSkinlet.cpp"
