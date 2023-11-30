#include "FocusIndicator.h"

#include <QskEvent.h>
#include <QskAnimationHint.h>
#include <QskBoxBorderMetrics.h>
#include <QskBoxShapeMetrics.h>
#include <QskBoxBorderColors.h>
#include <QskRgbValue.h>

#include <QQuickWindow>

QSK_STATE( FocusIndicator, Concealed, QskAspect::FirstUserState )

class FocusIndicator::PrivateData
{
  public:
    void restartTimer( FocusIndicator* indicator, int ms )
    {
        if( timerId > 0 )
        {
            indicator->killTimer( timerId );
            timerId = 0;
        }

        if ( ms > 0 )
            timerId = indicator->startTimer( ms );
    }

    const int timeout = 4500;
    int timerId = 0;

    bool blockAutoRepeatKeyEvents = false;
};

FocusIndicator::FocusIndicator( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData() )
{
    if( window() )
        window()->installEventFilter( this );
#if 1
    auto colors = boxBorderColorsHint( Panel );

    setBoxBorderColorsHint( Panel | Concealed, QskRgb::Transparent );

    setAnimationHint( Panel | QskAspect::Color, 200 );
    setAnimationHint( Panel | QskAspect::Color | Concealed, 500 );
#endif

    m_data->restartTimer( this, m_data->timeout );
}

FocusIndicator::~FocusIndicator()
{
}

void FocusIndicator::setConcealed( bool on )
{
    if ( on == isConcealed() )
        return;

    setSkinStateFlag( Concealed, on );

    int timeout = 0;
    if ( !on )
        timeout = m_data->timeout + animationHint( Panel | QskAspect::Color ).duration;

    m_data->restartTimer( this, timeout );

    Q_EMIT concealedChanged( on );
}

bool FocusIndicator::eventFilter( QObject* object, QEvent* event )
{
    if( object != window() )
        return Inherited::eventFilter( object, event );

    switch( static_cast< int >( event->type() ) )
    {
        case QEvent::KeyPress:
        {
            const auto keyEvent = static_cast< QKeyEvent* >( event );

            if( keyEvent->isAutoRepeat() && m_data->blockAutoRepeatKeyEvents )
                return true;

            if ( qskIsButtonPressKey( keyEvent ) )
            {
                if ( isConcealed() )
                {
                    setConcealed( false );
                    return true;
                }

                return false;
            }

            if( qskFocusChainIncrement( keyEvent ) != 0 )
            {
                if ( isConcealed() )
                {
                    setConcealed( false );
                    m_data->blockAutoRepeatKeyEvents = true;
                    return true;
                }
                else
                {
                    // extending the timer
                    m_data->restartTimer( this, m_data->timeout );
                    return false;
                }
            }

            m_data->blockAutoRepeatKeyEvents = false;

            break;
        }

        case QEvent::KeyRelease:
        {
            if( m_data->blockAutoRepeatKeyEvents )
            {
                if( !static_cast< QKeyEvent* >( event )->isAutoRepeat() )
                    m_data->blockAutoRepeatKeyEvents = false;

                return true;
            }

            break;
        }

        case QEvent::FocusOut:
        {
            setConcealed( true );
            break;
        }

        case QEvent::FocusIn:
        {
            setConcealed( false );
            break;
        }
    }

    return Inherited::eventFilter( object, event );
}

void FocusIndicator::windowChangeEvent( QskWindowChangeEvent* event )
{
    Inherited::windowChangeEvent( event );

    if( event->oldWindow() )
        event->oldWindow()->removeEventFilter( this );

    if( event->window() )
        event->window()->installEventFilter( this );
}

void FocusIndicator::timerEvent( QTimerEvent* event )
{
    if( event->timerId() == m_data->timerId )
    {
        m_data->restartTimer( this, 0 );

        if ( !isConcealed() )
        {
            setSkinStateFlag( Concealed, true );
            Q_EMIT concealedChanged( true );
        }

        return;
    }

    Inherited::timerEvent( event );
}

bool FocusIndicator::isConcealed() const
{
    return hasSkinState( Concealed );
}

#include "moc_FocusIndicator.cpp"
