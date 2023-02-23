/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSpinBox.h"
#include "QskEvent.h"

#include <qbasictimer.h>

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
}

class QskSpinBox::PrivateData
{
  public:
    PrivateData()
        : buttons( true )
        , tracking( true )
        , wrapping( false )
        , accelerating( false )
    {
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

    int autoRepeatDelay = 300;
    int autoRepeatInterval = 100;

    qreal autoRepeatIncrement = 0.0;

    QBasicTimer repeatTimer;

    int key = Qt::Key_unknown;

    bool buttons : 1;
    bool tracking : 1;
    bool wrapping : 1;
    bool accelerating : 1; // not yet implemented: TODO ...
};

QskSpinBox::QskSpinBox( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData )
{
    initSizePolicy( QskSizePolicy::Minimum, QskSizePolicy::Fixed );

    setBoundaries( 0.0, 99.99 ); // this is what QDoubleSpinBox does

    setAcceptedMouseButtons( Qt::LeftButton );
    setFocusPolicy( Qt::StrongFocus );
}

QskSpinBox::~QskSpinBox()
{
}

void QskSpinBox::setButtons( bool on )
{
    if ( on != m_data->buttons )
    {
        m_data->buttons = on;
        Q_EMIT buttonsChanged( on );
    }
}

bool QskSpinBox::hasButtons() const
{
    return m_data->buttons;
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

void QskSpinBox::mousePressEvent( QMouseEvent* event )
{
    if ( !isReadOnly() )
    {
        if ( auto subcontrol = buttonAt( this, qskMousePosition( event ) ) )
        {
            if ( !m_data->repeatTimer.isActive() )
            {
                auto increment = ( event->modifiers() == Qt::ControlModifier )
                    ? stepSize() : pageSize();

                if ( subcontrol == QskSpinBox::DownPanel )
                    increment = -increment;

                m_data->setAutoRepeat( this, increment );
            }
            return;
        }
    }

    Inherited::mousePressEvent( event );
}

void QskSpinBox::mouseReleaseEvent( QMouseEvent* )
{
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

                m_data->setAutoRepeat( this, offset );
                m_data->key = event->key();
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
            m_data->repeatTimer.start( m_data->autoRepeatInterval, this );
        }

        return;
    }

    Inherited::timerEvent( event );
}

#include "moc_QskSpinBox.cpp"
