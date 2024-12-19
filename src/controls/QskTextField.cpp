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

static constexpr QskAspect qskAspectRipple()
{
    using Q = QskTextField;
    return Q::ButtonPanel | Q::Hovered | QskAspect::Metric | QskAspect::Position;
}

class QskTextField::PrivateData
{
  public:
    QString headerText;
    QString footerText;
    QString placeholderText;

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

void QskTextField::mousePressEvent( QMouseEvent* event )
{
    if( !isReadOnly() )
    {
        const auto r = subControlRect( Button );
        if ( r.contains( qskMousePosition( event ) ) )
        {
            setText( QString() );
            setEditing( true );

            return;
        }
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
