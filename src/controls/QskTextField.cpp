/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskTextField.h"
#include "QskEvent.h"
#include "QskFontRole.h"
#include "QskQuick.h"

QSK_SUBCONTROL( QskTextField, Panel )

QSK_SUBCONTROL( QskTextField, Header )
QSK_SUBCONTROL( QskTextField, Footer )

QSK_SUBCONTROL( QskTextField, Icon )
QSK_SUBCONTROL( QskTextField, ButtonPanel )
QSK_SUBCONTROL( QskTextField, Button )
QSK_SUBCONTROL( QskTextField, Placeholder )

QSK_SUBCONTROL( QskTextField, CharacterCount )

QSK_SYSTEM_STATE( QskTextField, Pressed, QskAspect::LastUserState << 1 )

class QskTextField::PrivateData
{
  public:
    QString headerText;
    QString footerText;
    QString placeholderText;

    Style style = PlainStyle;
    QskAspect::States buttonStates;
};

QskTextField::QskTextField( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData() )
{
#if 1
    // character count might have changed
    connect( this, &QskTextInput::textChanged, this, &QQuickItem::update );
#endif
}

QskTextField::QskTextField( const QString& text, QQuickItem* parent )
    : QskTextField( parent )
{
    setText( text );
}

QskTextField::~QskTextField()
{
}

void QskTextField::setStyle( Style style )
{
    if ( style != m_data->style )
    {
        m_data->style = style;

        resetImplicitSize();
        update();

        Q_EMIT styleChanged( style );
    }
}

QskTextField::Style QskTextField::style() const
{
    return m_data->style;
}

QString QskTextField::headerText() const
{
    return m_data->headerText;
}

void QskTextField::setHeaderText( const QString& text )
{
    if ( m_data->headerText != text )
    {
        m_data->headerText = text;

        update();
        resetImplicitSize();

        Q_EMIT headerTextChanged( text );
    }
}

QString QskTextField::footerText() const
{
    return m_data->footerText;
}

void QskTextField::setFooterText( const QString& text )
{
    if ( m_data->footerText != text )
    {
        m_data->footerText = text;

        update();
        resetImplicitSize();

        Q_EMIT footerTextChanged( text );
    }
}

QskGraphic QskTextField::icon() const
{
    return symbolHint( Icon );
}

void QskTextField::setIcon( const QskGraphic& icon )
{
    if ( setSymbolHint( Icon, icon ) )
    {
        update();
        resetImplicitSize();
    }
}

void QskTextField::setPlaceholderText( const QString& text )
{
    if ( m_data->placeholderText != text )
    {
        m_data->placeholderText = text;
        Q_EMIT placeholderTextChanged( text );
    }
}

QString QskTextField::placeholderText() const
{
    return m_data->placeholderText;
}

QskAspect::Variation QskTextField::effectiveVariation() const
{
    return static_cast< QskAspect::Variation >( m_data->style );
}

void QskTextField::handleButtonClick()
{
    clear();
    setEditing( true );
}

void QskTextField::mousePressEvent( QMouseEvent* event )
{
    if( !isReadOnly() )
    {
        const auto r = subControlRect( Button );
        if ( r.contains( qskMousePosition( event ) ) )
        {
            setButtonState( Pressed, true );
            return;
        }
    }

    Inherited::mousePressEvent( event );
}

void QskTextField::mouseMoveEvent( QMouseEvent* event )
{
    if ( m_data->buttonStates & Pressed )
    {
        const auto r = subControlRect( Button );
        setButtonState( Pressed, r.contains( qskMousePosition( event ) ) );
        return;
    }

    Inherited::mouseMoveEvent( event );
}

void QskTextField::mouseReleaseEvent( QMouseEvent* event )
{
    if ( m_data->buttonStates & Pressed )
    {
        setButtonState( Pressed, false );
        handleButtonClick();

        return;
    }

    Inherited::mouseReleaseEvent( event );
}

void QskTextField::mouseUngrabEvent()
{
    setButtonState( Pressed, false );
    Inherited::mouseUngrabEvent();
}

void QskTextField::hoverEnterEvent( QHoverEvent* event )
{
    Inherited::hoverEnterEvent( event );

    const auto r = subControlRect( Button );
    setButtonState( Hovered, r.contains( qskHoverPosition( event ) ) );
}

void QskTextField::hoverMoveEvent( QHoverEvent* event )
{
    const auto r = subControlRect( Button );
    setButtonState( Hovered, r.contains( qskHoverPosition( event ) ) );

    Inherited::hoverMoveEvent( event );
}

void QskTextField::hoverLeaveEvent( QHoverEvent* event )
{
    setButtonState( Hovered, false );
    Inherited::hoverLeaveEvent( event );
}

QskAspect::States QskTextField::buttonStates() const
{
    auto states = skinStates() | m_data->buttonStates;

    if ( !( m_data->buttonStates & Hovered ) )
        states &= ~Hovered;

    return states;
}

void QskTextField::setButtonState( QskAspect::State state, bool on )
{
    const auto oldStates = m_data->buttonStates;

    if ( on )
        m_data->buttonStates |= state;
    else
        m_data->buttonStates &= ~state;

    if ( oldStates != m_data->buttonStates )
        update();
}

#include "moc_QskTextField.cpp"
