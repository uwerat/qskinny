/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSpinBox.h"
#include "QskIntervalF.h"
#include "QskEvent.h"

#include <qguiapplication.h>
#include <qstylehints.h>
#include <qmath.h>

#include <array>

QSK_SUBCONTROL( QskSpinBox, IncrementPanel )
QSK_SUBCONTROL( QskSpinBox, DecrementPanel )
QSK_SUBCONTROL( QskSpinBox, IncrementText )
QSK_SUBCONTROL( QskSpinBox, DecrementText )
QSK_SUBCONTROL( QskSpinBox, Text )
QSK_SUBCONTROL( QskSpinBox, TextPanel )
QSK_SUBCONTROL( QskSpinBox, Layout )

QSK_SYSTEM_STATE( QskSpinBox, Pressed, ( QskAspect::QskAspect::FirstSystemState << 0 ) )

namespace aliased_enum
{
    constexpr auto D = QskSpinBox::Decrement;
    constexpr auto T = QskSpinBox::Textbox;
    constexpr auto I = QskSpinBox::Increment;
    constexpr auto N = QskSpinBox::None;
}

class QskSpinBox::PrivateData
{
  public:
    explicit PrivateData( QskSpinBox* const parent )
        : q( parent )
    {
    }

    FocusIndeces defaultFocusIndex() const
    {
        const auto layout = q->alignmentHint( QskSpinBox::Layout );

        if ( layout == Qt::AlignLeft )
        {
            return QskSpinBox::Textbox;
        }
        if ( layout == Qt::AlignRight )
        {
            return QskSpinBox::Decrement;
        }
        if ( layout == Qt::AlignHCenter )
        {
            return QskSpinBox::Decrement;
        }
        if ( layout == Qt::AlignTop )
        {
            return QskSpinBox::Textbox;
        }
        if ( layout == Qt::AlignBottom )
        {
            return QskSpinBox::Increment;
        }
        if ( layout == Qt::AlignVCenter )
        {
            return QskSpinBox::Increment;
        }
        if ( layout == ( Qt::AlignLeft | Qt::AlignVCenter ) )
        {
            return QskSpinBox::Textbox;
        }
        if ( layout == ( Qt::AlignRight | Qt::AlignVCenter ) )
        {
            return QskSpinBox::Increment;
        }
        if ( layout == ( Qt::AlignTop | Qt::AlignHCenter ) )
        {
            return QskSpinBox::Textbox;
        }
        if ( layout == ( Qt::AlignBottom | Qt::AlignHCenter ) )
        {
            return QskSpinBox::Decrement;
        }

        return None;
    }

    FocusIndeces nextFocusIndex() const
    {
        const auto layout = q->alignmentHint( QskSpinBox::Layout );
        using namespace aliased_enum;

        // [0][1][2][3] := index
        // [D][T][I][N] := control
        using LUT = std::array< QskSpinBox::FocusIndeces, 4 >;
        if ( layout == Qt::AlignLeft )
        {
            return LUT{ I, D, N, T }[ m_focusIndex ];
        }
        if ( layout == Qt::AlignRight )
        {
            return LUT{ I, N, T, D }[ m_focusIndex ];
        }
        if ( layout == Qt::AlignHCenter )
        {
            return LUT{ T, I, N, D }[ m_focusIndex ];
        }
        if ( layout == Qt::AlignTop )
        {
            return LUT{ N, I, D, T }[ m_focusIndex ];
        }
        if ( layout == Qt::AlignBottom )
        {
            return LUT{ T, N, D, I }[ m_focusIndex ];
        }
        if ( layout == Qt::AlignVCenter )
        {
            return LUT{ N, D, T, I }[ m_focusIndex ];
        }
        if ( layout == ( Qt::AlignLeft | Qt::AlignVCenter ) )
        {
            return LUT{ N, I, D, T }[ m_focusIndex ];
        }
        if ( layout == ( Qt::AlignRight | Qt::AlignVCenter ) )
        {
            return LUT{ T, N, D, I }[ m_focusIndex ];
        }
        if ( layout == ( Qt::AlignTop | Qt::AlignHCenter ) )
        {
            return LUT{ I, D, N, T }[ m_focusIndex ];
        }
        if ( layout == ( Qt::AlignBottom | Qt::AlignHCenter ) )
        {
            return LUT{ I, N, T, D }[ m_focusIndex ];
        }

        return None;
    }

    FocusIndeces previousFocusIndex() const
    {
        const auto layout = q->alignmentHint( QskSpinBox::Layout );
        using namespace aliased_enum;

        // [0][1][2][3] := index
        // [D][T][I][N] := control
        using LUT = std::array< FocusIndeces, 4 >;
        if ( layout == Qt::AlignLeft )
        {
            return LUT{ T, N, D, I }[ m_focusIndex ];
        }
        if ( layout == Qt::AlignRight )
        {
            return LUT{ N, I, D, T }[ m_focusIndex ];
        }
        if ( layout == Qt::AlignHCenter )
        {
            return LUT{ N, D, T, I }[ m_focusIndex ];
        }
        if ( layout == Qt::AlignTop )
        {
            return LUT{ I, N, T, D }[ m_focusIndex ];
        }
        if ( layout == Qt::AlignBottom )
        {
            return LUT{ I, D, N, T }[ m_focusIndex ];
        }
        if ( layout == Qt::AlignVCenter )
        {
            return LUT{ T, I, N, D }[ m_focusIndex ];
        }
        if ( layout == ( Qt::AlignLeft | Qt::AlignVCenter ) )
        {
            return LUT{ I, N, T, D }[ m_focusIndex ];
        }
        if ( layout == ( Qt::AlignRight | Qt::AlignVCenter ) )
        {
            return LUT{ I, D, N, T }[ m_focusIndex ];
        }
        if ( layout == ( Qt::AlignTop | Qt::AlignHCenter ) )
        {
            return LUT{ T, N, D, I }[ m_focusIndex ];
        }
        if ( layout == ( Qt::AlignBottom | Qt::AlignHCenter ) )
        {
            return LUT{ N, I, D, T }[ m_focusIndex ];
        }

        return None;
    }

    FocusIndeces focusIndex() const
    {
        return m_focusIndex;
    }

    void focusNext()
    {
        const auto index = nextFocusIndex();
        setFocus( index );
    }

    void focusPrevious()
    {
        const auto index = previousFocusIndex();
        setFocus( index );
    }

    void focusDefault()
    {
        const auto index = defaultFocusIndex();
        setFocus( index );
    }

    void setFocus( const FocusIndeces index )
    {
        using namespace aliased_enum;
        Q_ASSERT( index == D || index == T || index == I || index == N );
        if ( index == D || index == T || index == I || index == N )
        {
            m_focusIndex = index;
            Q_EMIT q->focusIndexChanged( m_focusIndex );
            q->update();
        }
    }

    QRectF focusIndicatorRect() const
    {
        if ( m_focusIndex == QskSpinBox::Decrement )
        {
            return q->subControlRect( QskSpinBox::DecrementPanel );
        }
        if ( m_focusIndex == QskSpinBox::Increment )
        {
            return q->subControlRect( QskSpinBox::IncrementPanel );
        }
        if ( m_focusIndex == QskSpinBox::Textbox )
        {
            return q->subControlRect( QskSpinBox::TextPanel );
        }
        return {};
    }

    void saveMousePosition( const QPointF& pos )
    {
        q->setSkinHint( QskSpinBox::Layout | QskAspect::Metric | QskAspect::Position, pos );
    }

    bool focusNow() const
    {
        const auto focusOnClick = ( q->focusPolicy() & Qt::ClickFocus ) == Qt::ClickFocus;
        const auto focusOnTouchRelease = QGuiApplication::styleHints()->setFocusOnTouchRelease();
        return focusOnClick && !focusOnTouchRelease;
    }

  private:
    QskSpinBox* const q;
    FocusIndeces m_focusIndex = FocusIndeces::None;
};

using S = QskSpinBox;

QskSpinBox::QskSpinBox( QQuickItem* const parent )
    : Inherited( parent )
    , m_data( std::make_unique< PrivateData >( this ) )
{
    setBoundaries( 0.0, 1.0 );
    setAcceptHoverEvents( true );
    setAcceptedMouseButtons( Qt::LeftButton );
    setFocusPolicy( Qt::StrongFocus );

    connect( this, &S::focusIndexChanged, this, &S::focusIndicatorRectChanged );
}

QskSpinBox::~QskSpinBox() = default;

void QskSpinBox::hoverEnterEvent( QHoverEvent* const event )
{
    m_data->saveMousePosition( qskHoverPosition( event ) );
}

void QskSpinBox::hoverLeaveEvent( QHoverEvent* /*const event */ )
{
    m_data->saveMousePosition( {} );
}

void QskSpinBox::hoverMoveEvent( QHoverEvent* const event )
{
    m_data->saveMousePosition( qskHoverPosition( event ) );
}

void QskSpinBox::mouseReleaseEvent( QMouseEvent* const event )
{
    m_data->saveMousePosition( qskMousePosition( event ) );

    const auto focus = m_data->focusNow();

    if ( subControlRect( QskSpinBox::IncrementPanel ).contains( event->pos() ) )
    {
        increment( +stepSize() );

        if ( focus )
        {
            m_data->setFocus( Increment );
        }

        return;
    }

    if ( subControlRect( QskSpinBox::DecrementPanel ).contains( event->pos() ) )
    {
        increment( -stepSize() );

        if ( focus )
        {
            m_data->setFocus( Decrement );
        }

        return;
    }

    if ( subControlRect( QskSpinBox::TextPanel ).contains( event->pos() ) )
    {
        if ( focus )
        {
            m_data->setFocus( Textbox );
        }

        return;
    }

    event->ignore();
}

void QskSpinBox::mousePressEvent( QMouseEvent* const event )
{
    m_data->saveMousePosition( -1 * qskMousePosition( event ) );

    const auto focus = m_data->focusNow();

    if ( subControlRect( QskSpinBox::IncrementPanel ).contains( event->pos() ) )
    {
        if ( focus )
        {
            m_data->setFocus( QskSpinBox::Increment );
        }
        return;
    }

    if ( subControlRect( QskSpinBox::DecrementPanel ).contains( event->pos() ) )
    {
        if ( focus )
        {
            m_data->setFocus( QskSpinBox::Decrement );
        }
        return;
    }

    event->ignore();
}

void QskSpinBox::keyPressEvent( QKeyEvent* const event )
{
    switch ( event->key() )
    {
        case Qt::Key_Plus:
        case Qt::Key_Up:
        case Qt::Key_Right:
            increment( +stepSize() );
            return;
        case Qt::Key_Minus:
        case Qt::Key_Down:
        case Qt::Key_Left:
            increment( -stepSize() );
            return;
        case Qt::Key_Select:
        case Qt::Key_Space:
            if ( focusIndex() == Increment )
            {
                increment( +stepSize() );
            }
            if ( focusIndex() == Decrement )
            {
                increment( -stepSize() );
            }
            return;
        default:
            break;
    }

    const int steps = qskFocusChainIncrement( event );

    if ( steps < 0 )
    {
        for ( int i = 0; i < qAbs( steps ); ++i )
        {
            m_data->focusPrevious();
        }
    }

    if ( steps > 0 )
    {
        for ( int i = 0; i < steps; ++i )
        {
            m_data->focusNext();
        }
    }

    if ( steps != 0 && m_data->focusIndex() != None )
    {
        return;
    }

    Inherited::keyPressEvent( event );
}

void QskSpinBox::keyReleaseEvent( QKeyEvent* const event )
{
    if ( event->key() == Qt::Key_Select || event->key() == Qt::Key_Space )
    {
        return;
    }

    Inherited::keyReleaseEvent( event );
}

void QskSpinBox::focusInEvent( QFocusEvent* const event )
{
    if ( event->reason() == Qt::TabFocusReason )
    {
        m_data->focusNext();
        return;
    }

    if ( event->reason() == Qt::BacktabFocusReason )
    {
        m_data->focusPrevious();
        return;
    }

    if ( m_data->focusIndex() == QskSpinBox::None )
    {
        m_data->focusDefault();
        return;
    }

    Inherited::focusInEvent( event );
}

QRectF QskSpinBox::focusIndicatorRect() const
{
    return m_data->focusIndicatorRect();
}

QskSpinBox::FocusIndeces QskSpinBox::focusIndex() const
{
    return m_data->focusIndex();
}
