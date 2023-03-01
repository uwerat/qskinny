/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSpinBox.h"
#include "QskEvent.h"
#include "QskFunctions.h"

#include <qbasictimer.h>
#include <qlocale.h>

#include <cfloat>

QSK_SUBCONTROL( QskSpinBox, Panel )

QSK_SUBCONTROL( QskSpinBox, TextPanel )
QSK_SUBCONTROL( QskSpinBox, Text )

QSK_SUBCONTROL( QskSpinBox, UpPanel )
QSK_SUBCONTROL( QskSpinBox, UpIndicator )

QSK_SUBCONTROL( QskSpinBox, DownPanel )
QSK_SUBCONTROL( QskSpinBox, DownIndicator )

QSK_SYSTEM_STATE( QskSpinBox, Increasing, ( QskAspect::FirstSystemState << 2 ) )
QSK_SYSTEM_STATE( QskSpinBox, Decreasing, ( QskAspect::FirstSystemState << 3 ) )

namespace
{
    inline QskAspect::Subcontrol buttonAt( const QskSpinBox* spinBox, const QPointF& pos )
    {
        if ( spinBox->subControlRect( QskSpinBox::UpPanel ).contains( pos ) )
            return QskSpinBox::UpPanel;

        if ( spinBox->subControlRect( QskSpinBox::DownPanel ).contains( pos ) )
            return QskSpinBox::DownPanel;

        return QskAspect::NoSubcontrol;
    }

    inline QskAspect aspectDecoration()
    {
        return QskSpinBox::Panel | QskAspect::Flag | QskAspect::Style;
    }

    inline QskAspect aspectTextAlignment()
    {
        return QskSpinBox::TextPanel | QskAspect::Flag | QskAspect::Alignment;
    }
}

class QskSpinBox::PrivateData
{
  public:
    PrivateData()
        : tracking( true )
        , wrapping( false )
        , accelerating( false )
    {
    }

    inline bool isActivatedByMouse() const
    {
        return this->repeatTimer.isActive() && ( this->key == Qt::Key_unknown );
    }

    inline bool isTimerBlocked( QskSpinBox* spinBox, qreal offset ) const
    {
        if ( !this->wrapping )
        {
            if ( offset >= 0.0 )
                return spinBox->value() >= spinBox->maximum();
            else
                return spinBox->value() <= spinBox->minimum();
        }

        return false;
    }

    inline void setAutoRepeat( QskSpinBox* spinBox, qreal offset )
    {
        this->autoRepeatIncrement = offset;
        this->key = Qt::Key_unknown;

        if( offset < 0.0 )
        {
            if ( spinBox->hasSkinState( QskSpinBox::Increasing ) )
                spinBox->setSkinStateFlag( QskSpinBox::Increasing, false );

            if ( !spinBox->hasSkinState( QskSpinBox::Decreasing ) )
            {
                spinBox->setSkinStateFlag( QskSpinBox::Decreasing, true );
                this->repeatTimer.start( autoRepeatDelay, spinBox );
            }

            return;
        }

        if ( offset > 0.0 )
        {
            if ( spinBox->hasSkinState( QskSpinBox::Decreasing ) )
                spinBox->setSkinStateFlag( QskSpinBox::Decreasing, false );

            if ( !spinBox->hasSkinState( QskSpinBox::Increasing ) )
            {
                spinBox->setSkinStateFlag( QskSpinBox::Increasing, true );
                this->repeatTimer.start( autoRepeatDelay, spinBox );
            }

            return;
        }

        spinBox->setSkinStateFlag( QskSpinBox::Decreasing, false );
        spinBox->setSkinStateFlag( QskSpinBox::Increasing, false );

        this->repeatTimer.stop();
    }

    int decimals = 2;

    int autoRepeatDelay = 300;
    int autoRepeatInterval = 100;

    qreal autoRepeatIncrement = 0.0;

    QBasicTimer repeatTimer;

    int key = Qt::Key_unknown;

    bool tracking     : 1;
    bool wrapping     : 1;
    bool accelerating : 1; // not yet implemented: TODO ...
};

QskSpinBox::QskSpinBox( qreal min, qreal max, qreal stepSize, QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData )
{
    initSizePolicy( QskSizePolicy::Minimum, QskSizePolicy::Fixed );

    setBoundaries( min, max );
    setStepSize( stepSize );

    setAcceptedMouseButtons( Qt::LeftButton );
    setFocusPolicy( Qt::StrongFocus );

    connect( this, &QskSpinBox::valueChanged, this, &QskSpinBox::textChanged );
}

QskSpinBox::QskSpinBox( QQuickItem* parent )
    : QskSpinBox( 0.0, 99.99, 0.1, parent )
{
}

QskSpinBox::~QskSpinBox()
{
}

void QskSpinBox::setDecoration( Decoration decoration )
{
    if ( setFlagHint( aspectDecoration(), decoration ) )
        Q_EMIT decorationChanged( decoration );
}

void QskSpinBox::resetDecoration()
{
    if ( resetFlagHint( aspectDecoration() ) )
        Q_EMIT decorationChanged( decoration() );
}

QskSpinBox::Decoration QskSpinBox::decoration() const
{
    return flagHint< QskSpinBox::Decoration >( aspectDecoration(), Buttons );
}

void QskSpinBox::setTextAlignment( Qt::Alignment alignment )
{
    alignment &= Qt::AlignHorizontal_Mask;

    if ( setFlagHint( aspectTextAlignment(), alignment ) )
        Q_EMIT textAlignmentChanged( alignment );
}

void QskSpinBox::resetTextAlignment()
{
    if ( resetFlagHint( aspectTextAlignment() ) )
        Q_EMIT textAlignmentChanged( textAlignment() );
}

Qt::Alignment QskSpinBox::textAlignment() const
{
    return flagHint< Qt::Alignment >(
        aspectTextAlignment(), Qt::AlignLeft ) & Qt::AlignHorizontal_Mask;
}

void QskSpinBox::setTracking( bool on )
{
    if ( on != m_data->tracking )
    {
        m_data->tracking = on;
        Q_EMIT trackingChanged( on );
    }
}

bool QskSpinBox::isTracking() const
{
    return m_data->tracking;
}

void QskSpinBox::setWrapping( bool on )
{
    if ( on != m_data->wrapping )
    {
        m_data->wrapping = on;
        Q_EMIT wrappingChanged( on );
    }
}

bool QskSpinBox::isWrapping() const
{
    return m_data->wrapping;
}

void QskSpinBox::setAccelerating( bool on )
{
    if ( on != m_data->accelerating )
    {
        m_data->accelerating = on;
        Q_EMIT acceleratingChanged( on );
    }
}

bool QskSpinBox::isAccelerating() const
{
    return m_data->accelerating;
}

void QskSpinBox::setDecimals( int decimals )
{
    decimals = qBound( 0, decimals, DBL_MAX_10_EXP + DBL_DIG );
    if ( decimals != m_data->decimals )
    {
        m_data->decimals = decimals;

        update();
        resetImplicitSize();
    }
}

int QskSpinBox::decimals() const
{
    return m_data->decimals;
}

QString QskSpinBox::text() const
{
    return textFromValue( value() );
}

QString QskSpinBox::textFromValue( qreal value ) const
{
    return locale().toString( value, 'f', m_data->decimals );
}

void QskSpinBox::increment( qreal offset )
{
    if ( m_data->wrapping )
    {
        const auto v = value();

        if ( offset > 0.0 && qskFuzzyCompare( v, maximum() ) )
        {
            setValue( minimum() );
            return;
        }

        if ( offset < 0.0 && qskFuzzyCompare( v, minimum() ) )
        {
            setValue( maximum() );
            return;
        }
    }

    Inherited::increment( offset );
}

void QskSpinBox::mousePressEvent( QMouseEvent* event )
{
    if ( !isReadOnly() )
    {
        if ( auto subcontrol = buttonAt( this, qskMousePosition( event ) ) )
        {
            bool acceptPress = !m_data->repeatTimer.isActive();

            if ( acceptPress && !m_data->wrapping )
            {
                if ( subcontrol == QskSpinBox::DownPanel )
                    acceptPress = value() > minimum();
                else
                    acceptPress = value() < maximum();
            }

            if ( acceptPress )
            {
                auto offset = stepSize();
                if ( event->modifiers() & ( Qt::ControlModifier | Qt::ShiftModifier ) )
                    offset *= pageSize();

                if ( subcontrol == QskSpinBox::DownPanel )
                    offset = -offset;

                increment( offset );

                if ( !m_data->isTimerBlocked( this, offset ) )
                    m_data->setAutoRepeat( this, offset );
            }
            return;
        }
    }

    Inherited::mousePressEvent( event );
}

void QskSpinBox::mouseReleaseEvent( QMouseEvent* )
{
    if ( m_data->isActivatedByMouse() )
        m_data->setAutoRepeat( this, 0.0 );
}

void QskSpinBox::mouseMoveEvent( QMouseEvent* event )
{
    if ( m_data->isActivatedByMouse() )
    {
        const auto subcontrol = buttonAt( this, qskMousePosition( event ) );

        if ( m_data->autoRepeatIncrement >= 0.0 )
        {
            if ( subcontrol != QskSpinBox::UpPanel )
                m_data->setAutoRepeat( this, 0.0 );
        }
        else
        {
            if ( subcontrol != QskSpinBox::DownPanel )
                m_data->setAutoRepeat( this, 0.0 );
        }
    }
}

void QskSpinBox::mouseUngrabEvent()
{
    if ( m_data->isActivatedByMouse() )
        m_data->setAutoRepeat( this, 0.0 );
}

void QskSpinBox::keyPressEvent( QKeyEvent* event )
{
    if ( !isReadOnly() && !m_data->repeatTimer.isActive() )
    {
        const auto offset = incrementForKey( event );
        if ( offset != 0.0 )
        {
            if ( !m_data->repeatTimer.isActive() )
            {
                increment( offset );

                if ( !m_data->isTimerBlocked( this, offset ) )
                {
                    m_data->setAutoRepeat( this, offset );
                    m_data->key = event->key();
                }
            }
            return;
        }
    }

    Inherited::keyPressEvent( event );
}

void QskSpinBox::keyReleaseEvent( QKeyEvent* event )
{
    if ( m_data->key == event->key() )
    {
        m_data->setAutoRepeat( this, 0.0 );
        return;
    }

    Inherited::keyReleaseEvent( event );
}

void QskSpinBox::timerEvent( QTimerEvent* event )
{
    if ( event->timerId() == m_data->repeatTimer.timerId() )
    {
        if ( skinStates() & ( QskSpinBox::Increasing | QskSpinBox::Decreasing ) )
        {
            increment( m_data->autoRepeatIncrement );

            if ( m_data->isTimerBlocked( this, m_data->autoRepeatIncrement ) )
                m_data->setAutoRepeat( this, 0.0 );
            else
                m_data->repeatTimer.start( m_data->autoRepeatInterval, this );
        }

        return;
    }

    Inherited::timerEvent( event );
}

#include "moc_QskSpinBox.cpp"
