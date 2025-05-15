/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskTextFieldSkinlet.h"
#include "QskTextField.h"
#include "QskFunctions.h"
#include "QskBoxHints.h"

#include <qfontmetrics.h>

using Q = QskTextField;

static QString qskEffectiveCharacterCountText( const QskTextField* textField )
{
    auto policy = textField->flagHint< Qsk::Policy >(
        Q::CharacterCount | QskAspect::Option, Qsk::Never );

    const auto maxLength = textField->maxLength();
    const bool isLimited = maxLength < 32767; // magic number hardcoded in qquicktextinput.cpp

    if ( policy == Qsk::Always || ( policy == Qsk::Maybe && isLimited ) )
    {
        auto s = QString::number( textField->text().length() );
        if ( isLimited )
            s += " / " + QString::number( maxLength );

        return s;
    }

    return QString();
}

QskTextFieldSkinlet::QskTextFieldSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( { TextPanelRole, IconRole, ButtonPanelRole, ButtonRole,
        PlaceholderRole, HeaderRole, FooterRole, CharacterCountRole } );
}

QskTextFieldSkinlet::~QskTextFieldSkinlet()
{
}

void QskTextFieldSkinlet::setRenderHints( RenderHints hints )
{
    m_renderHints = hints;
}

QskTextFieldSkinlet::RenderHints QskTextFieldSkinlet::renderHints() const
{
    return m_renderHints;
}

QRectF QskTextFieldSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    const auto textField = static_cast< const QskTextField* >( skinnable );

    if ( subControl == Q::Panel )
        return contentsRect;

    if ( subControl == Q::Header )
        return headerRect( skinnable, contentsRect );

    if ( subControl == Q::Footer )
        return alignedLabelRect( skinnable, contentsRect, subControl, Qt::AlignBottom );

    if ( subControl == Q::CharacterCount )
        return alignedLabelRect( skinnable, contentsRect, subControl, Qt::AlignBottom );

    if ( subControl == Q::TextPanel )
        return inputPanelRect( skinnable, contentsRect );

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
        if ( hasText( skinnable, Q::Placeholder ) )
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
    switch ( nodeRole )
    {
        case TextPanelRole:
            return updateInputPanelNode( skinnable, node );

        case PlaceholderRole:
            return updateLabelNode( skinnable, node, Q::Placeholder );

        case HeaderRole:
            return updateLabelNode( skinnable, node, Q::Header );

        case FooterRole:
            return updateLabelNode( skinnable, node, Q::Footer );

        case CharacterCountRole:
            return updateLabelNode( skinnable, node, Q::CharacterCount );

        case IconRole:
            return updateSymbolNode( skinnable, node, Q::Icon );

        case ButtonPanelRole:
            return updateBoxNode( skinnable, node, Q::ButtonPanel );

        case ButtonRole:
            return updateSymbolNode( skinnable, node, Q::Button );
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QSGNode* QskTextFieldSkinlet::updateInputPanelNode(
    const QskSkinnable* skinnable, QSGNode* node ) const
{
    const auto control = static_cast< const QskControl* >( skinnable );

    const auto rect = control->subControlRect( Q::TextPanel );

    auto hints = skinnable->boxHints( Q::TextPanel );

    if ( panelMode( skinnable ) == 2 )
    {
        const auto clipRect = control->subControlRect( Q::Header );
        if ( !clipRect.isEmpty() )
        {
            // "cutting a hole" in the upper gradient for the header
            const auto margin = 6; // ->skin
            auto s1 = ( clipRect.left() - margin - rect.left() ) / rect.width();
            auto s2 = ( clipRect.right() - rect.left() ) / rect.width();

            auto gradient = hints.borderColors.top();
            gradient.setStops( qskClippedGradientStops( gradient.stops(), s1, s2 ) );
            hints.borderColors.setTop( gradient );
        }
    }

    return updateBoxNode( skinnable, node, rect, hints );
}

QSGNode* QskTextFieldSkinlet::updateLabelNode( const QskSkinnable* skinnable,
    QSGNode* node, QskAspect::Subcontrol subControl  ) const
{
    const auto text = effectiveText( skinnable, subControl );
    if ( text.isEmpty() )
        return nullptr;

    QskSkinHintStatus status;

    auto options = skinnable->textOptionsHint( subControl, &status );
    if ( !status.isValid() )
        options.setElideMode( Qt::ElideRight );

    auto rect = skinnable->controlCast()->contentsRect();
    rect = subControlRect( skinnable, rect, subControl );

    return updateTextNode( skinnable, node, rect,
        skinnable->alignmentHint( subControl, Qt::AlignLeft ),
        options, text, subControl );
}

QRectF QskTextFieldSkinlet::headerRect(
    const QskSkinnable* skinnable, const QRectF& contentsRect ) const
{
    switch( panelMode( skinnable ) )
    {
        case 1:
        {
            const auto sz = effectiveTextSize( skinnable, Q::Header );

            const auto r = subControlRect( skinnable, contentsRect, Q::Text );
            return QRectF( r.x(), r.top() - sz.height(), sz.width(), sz.height() );
        }
        case 2:
        {
            const auto sz = effectiveTextSize( skinnable, Q::Header );

            const auto r = subControlRect( skinnable, contentsRect, Q::TextPanel );

            const auto x = r.left() + skinnable->paddingHint( Q::TextPanel ).left();
            const auto y = r.top() - 0.5 * sz.height();

            return QRectF( x, y, sz.width(), sz.height() );
        }
        default:
            return alignedLabelRect( skinnable, contentsRect, Q::Header, Qt::AlignTop );
    }
}

QRectF QskTextFieldSkinlet::inputPanelRect(
    const QskSkinnable* skinnable, const QRectF& contentsRect ) const
{
    auto rect = skinnable->subControlContentsRect( contentsRect, Q::Panel );

    const auto mode = panelMode( skinnable );

    if ( mode == 0 )
    {
        qreal h = skinnable->effectiveFontHeight( Q::Text );
        h = skinnable->outerBoxSize( Q::TextPanel, QSizeF( rect.width(), h ) ).height();
        h = qMax( h, skinnable->strutSizeHint( Q::TextPanel ).height() );

        /*
            when having textfields in horizontal layouts you usually want
            the text panels being vertically aligned - regardless of having
            Q::Header/Q::Footer being available.
         */

        auto top = textHeight( skinnable, Q::Header );
        auto bottom = textHeight( skinnable, Q::Footer );

        if ( rect.height() < top + h + bottom )
        {
            if ( effectiveText( skinnable, Q::Footer ).isEmpty() )
                bottom = 0.0;
        }

        if ( rect.height() < top + h + bottom )
        {
            if ( effectiveText( skinnable, Q::Header ).isEmpty() )
                top = 0.0;
        }

        return QRectF( rect.left(), rect.top() + top, rect.width(), h );
    }
    else
    {
        if ( mode == 2 )
        {
            qreal h = 0.0;
#if 0
            h = effectiveTextHeight( skinnable, Q::Header );
#else
            if ( !text( skinnable, Q::Header ).isEmpty() )
                h = skinnable->effectiveFontHeight( Q::Header );
#endif
            rect.setTop( rect.top() + 0.5 * h );
        }

        const auto h = skinnable->strutSizeHint( Q::TextPanel ).height();
        rect.setHeight( h );

        return rect;
    }
}

QRectF QskTextFieldSkinlet::alignedLabelRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl,
    Qt::Alignment alignment ) const
{
    const auto h = effectiveTextHeight( skinnable, subControl );
    if ( h <= 0.0 )
        return QRectF();

    const auto m = skinnable->marginHint( subControl );

    auto r = subControlRect( skinnable, contentsRect, Q::TextPanel );

    const auto y = ( alignment & Qt::AlignTop )
        ? r.top() - m.bottom() - h : r.bottom() + m.top();

    return QRectF( r.left() + m.left(), y,
        r.width() - m.left() - m.right(), h );
}

QSizeF QskTextFieldSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& constraint ) const
{
    Q_UNUSED( constraint ); // TODO ...

    if ( which != Qt::PreferredSize )
        return QSizeF();

    const auto textField = static_cast< const QskTextField* >( skinnable );

    auto hint = textField->unwrappedTextSize();
    hint = hint.expandedTo( skinnable->strutSizeHint( Q::Text ) );

    if( !skinnable->symbolHint( Q::Icon ).isEmpty() )
    {
        const auto sz = skinnable->strutSizeHint( Q::Icon );
        if ( sz.width() > 0.0 )
            hint.rwidth() += sz.width();

        hint.rheight() = qMax( hint.height(), sz.height() );
    }

    if( !skinnable->symbolHint( Q::Button ).isEmpty() )
    {
        const auto sz = skinnable->strutSizeHint( Q::Button );
        if ( sz.width() > 0.0 )
            hint.rwidth() += sz.width();

        hint.rheight() = qMax( hint.height(), sz.height() );
    }

    switch( panelMode( skinnable ) )
    {
        case 0:
        {
            hint = skinnable->outerBoxSize( Q::TextPanel, hint );
            hint = hint.expandedTo( skinnable->strutSizeHint( Q::TextPanel ) );

            hint.rheight() += effectiveTextHeight( skinnable, Q::Header );
            break;
        }
        case 1:
        {
            hint = hint.expandedTo( skinnable->strutSizeHint( Q::TextPanel ) );
            break;
        }
        case 2:
        {
            hint = hint.expandedTo( skinnable->strutSizeHint( Q::TextPanel ) );
            hint.rheight() += 0.5 * skinnable->effectiveFontHeight( Q::Header );
            break;
        }
    }

    hint.rheight() += qMax(
        effectiveTextHeight( skinnable, Q::Footer ),
        effectiveTextHeight( skinnable, Q::CharacterCount )
    );

    hint = skinnable->outerBoxSize( Q::Panel, hint );
    return hint.expandedTo( skinnable->strutSizeHint( Q::Panel ) );
}

qreal QskTextFieldSkinlet::textHeight( const QskSkinnable* skinnable,
    QskAspect::Subcontrol subControl ) const
{
    auto h = skinnable->effectiveFontHeight( subControl );

    const auto margins = skinnable->marginHint( subControl );
    h += margins.top() + margins.bottom();

    const auto sz = skinnable->strutSizeHint( subControl );

    return qMax( h, sz.height() );
}

QSizeF QskTextFieldSkinlet::effectiveTextSize( const QskSkinnable* skinnable,
    QskAspect::Subcontrol subControl ) const
{
    const auto text = effectiveText( skinnable, subControl );

    const QFontMetricsF fm( skinnable->effectiveFont( subControl ) );

    auto w = qskHorizontalAdvance( fm, effectiveText( skinnable, subControl ) );
    auto h = fm.height();

    const auto margins = skinnable->marginHint( subControl );
    w += margins.left() + margins.right();
    h += margins.top() + margins.bottom();

    QSizeF sz( w, h );
    sz = sz.expandedTo( skinnable->strutSizeHint( subControl ) );

    return sz;
}

QString QskTextFieldSkinlet::text(
    const QskSkinnable* skinnable, QskAspect::Subcontrol subControl ) const
{
    const auto textField = static_cast< const QskTextField* >( skinnable );

    if ( subControl == Q::Text )
        return textField->text();

    if ( subControl == Q::Placeholder )
        return textField->placeholderText();

    if ( subControl == Q::Header )
        return textField->headerText();

    if ( subControl == Q::Footer )
        return textField->footerText();

    if ( subControl == Q::CharacterCount )
        return qskEffectiveCharacterCountText( textField );

    return QString();
}

bool QskTextFieldSkinlet::isPlaceholderVisible(
    const QskSkinnable* skinnable ) const
{
    return !skinnable->hasSkinState( Q::Editing )
        && text( skinnable, Q::Text ).isEmpty();
}

bool QskTextFieldSkinlet::hasText(
    const QskSkinnable* skinnable, QskAspect::Subcontrol subControl ) const
{
    return !effectiveText( skinnable, subControl ).isEmpty();
}

QString QskTextFieldSkinlet::effectiveText(
    const QskSkinnable* skinnable, QskAspect::Subcontrol subControl ) const
{
    if ( subControl == Q::Placeholder )
    {
        if ( !isPlaceholderVisible( skinnable ) )
            return QString();

        auto txt = text( skinnable, Q::Placeholder );
        if ( txt.isEmpty() && ( renderHints() & UseHeaderAsPlaceholder ) )
            txt = text( skinnable, Q::Header );

        return txt;
    }

    if ( subControl == Q::Header )
    {
        if ( isPlaceholderVisible( skinnable )
            && ( renderHints() & UseHeaderAsPlaceholder ) )
        {
            return QString();
        }
     
        return text( skinnable, Q::Header );
    }

    return text( skinnable, subControl );
}

qreal QskTextFieldSkinlet::effectiveTextHeight(
    const QskSkinnable* skinnable, QskAspect::Subcontrol subcontrol ) const
{
    const auto text = effectiveText( skinnable, subcontrol );
    return text.isEmpty() ? 0.0 : textHeight( skinnable, subcontrol );
}

#include "moc_QskTextFieldSkinlet.cpp"
