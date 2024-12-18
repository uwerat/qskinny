/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskTextField.h"
#include "QskTextFieldSkinlet.h"
#include "QskEvent.h"
#include "QskFontRole.h"
#include "QskQuick.h"

QSK_SUBCONTROL( QskTextField, Panel )

QSK_SUBCONTROL( QskTextField, LeadingIcon )
QSK_SUBCONTROL( QskTextField, LabelText )
QSK_SUBCONTROL( QskTextField, Ripple )
QSK_SUBCONTROL( QskTextField, TrailingIcon )
QSK_SUBCONTROL( QskTextField, PlaceholderText )
QSK_SUBCONTROL( QskTextField, SupportingText )
QSK_SUBCONTROL( QskTextField, CharacterCount )

static constexpr QskAspect qskAspectRipple()
{
    using Q = QskTextField;
    return Q::Ripple | Q::Hovered | QskAspect::Metric | QskAspect::Position;
}

class QskTextField::PrivateData
{
  public:
    QString labelText;
    QString placeholderText;
    QString supportingText;

    Style style = PlainStyle;
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

QString QskTextField::labelText() const
{
    return m_data->labelText;
}

void QskTextField::setLabelText( const QString& text )
{
    if ( m_data->labelText != text )
    {
        m_data->labelText = text;
        Q_EMIT labelTextChanged( text );
    }
}

QskGraphic QskTextField::leadingIcon() const
{
    return symbolHint( LeadingIcon );
}

void QskTextField::setLeadingIcon( const QskGraphic& icon )
{
    setSymbolHint( LeadingIcon, icon );
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

void QskTextField::setSupportingText( const QString& text )
{
    if ( m_data->supportingText != text )
    {
        m_data->supportingText = text;
        Q_EMIT supportingTextChanged( text );
    }
}

QString QskTextField::supportingText() const
{
    return m_data->supportingText;
}

QskAspect::Variation QskTextField::effectiveVariation() const
{
    return static_cast< QskAspect::Variation >( m_data->style );
}

void QskTextField::mousePressEvent( QMouseEvent* event )
{
    if( !isReadOnly() && subControlContentsRect( TrailingIcon ).contains( event->pos() ) )
    {
        setText( QString() );
        setEditing( true );

        return;
    }

    Inherited::mousePressEvent( event );
}

void QskTextField::hoverEnterEvent( QHoverEvent* event )
{
    setSkinHint( qskAspectRipple(), qskHoverPosition( event ) );
    Inherited::hoverEnterEvent( event );
}

void QskTextField::hoverMoveEvent( QHoverEvent* event )
{
    setSkinHint( qskAspectRipple(), qskHoverPosition( event ) );
    Inherited::hoverMoveEvent( event );
}

void QskTextField::hoverLeaveEvent( QHoverEvent* event )
{
    resetSkinHint( qskAspectRipple() );
    Inherited::hoverLeaveEvent( event );
}

#include "moc_QskTextField.cpp"
