#include "FocusIndicator.h"

#include <QskEvent.h>
#include <QskAnimationHint.h>

#include <qquickwindow.h>
#include <qbasictimer.h>

static inline bool qskIsEnablingKey( const QKeyEvent* event )
{
    // what keys do we want have here ???
    return qskIsButtonPressKey( event ) || qskFocusChainIncrement( event );
}

class FocusIndicator::PrivateData
{
  public:
    inline bool isAutoDisabling() const { return duration > 0; }
    inline bool isAutoEnabling() const { return false; }

    int duration = 0;
    QBasicTimer timer;

    bool blockAutoRepeatKeyEvents = false;
};

FocusIndicator::FocusIndicator( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData() )
{
    setDuration( 4500 );
}

FocusIndicator::~FocusIndicator()
{
}

void FocusIndicator::setDuration( int ms )
{
    ms = std::max( ms, 0 );
    if ( ms == m_data->duration )
        return;

    m_data->duration = ms;

    if ( m_data->isAutoDisabling() )
    {
        if ( auto w = window() )
            w->installEventFilter( this );

        if ( isEnabled() )
        {
            if ( isInitiallyPainted() )
                m_data->timer.start( m_data->duration, this );
            else
                setEnabled( false );
        }

        connect( this, &QQuickItem::enabledChanged,
            this, &FocusIndicator::resetTimer );
    }
    else
    {
        if ( auto w = window() )
            w->removeEventFilter( this );

        setEnabled( true );

        disconnect( this, &QQuickItem::enabledChanged,
            this, &FocusIndicator::resetTimer );
    }

    Q_EMIT durationChanged( ms );
}

int FocusIndicator::duration() const
{
    return m_data->duration;
}

void FocusIndicator::maybeEnable( bool on  )
{
    if ( !m_data->isAutoEnabling() )
        return;

    if ( on )
    {
        if ( auto w = window() )
        {
            if ( w->isExposed() && w->isActive() )
                setEnabled( true );
        }
    }
    else
    {
        setEnabled( false );
    }
}

void FocusIndicator::resetTimer()
{
    if ( m_data->isAutoDisabling() )
    {
        if ( isEnabled() )
        {
            const auto hint = animationHint( Panel | QskAspect::Color );
            m_data->timer.start( m_data->duration + hint.duration, this );
        }
        else
        {
            m_data->timer.stop();
        }
    }
}

bool FocusIndicator::eventFilter( QObject* object, QEvent* event )
{
    if( ( object != window() ) || !m_data->isAutoDisabling() )
        return Inherited::eventFilter( object, event );

    switch( static_cast< int >( event->type() ) )
    {
        case QEvent::KeyPress:
        case QEvent::KeyRelease:
        case QEvent::ShortcutOverride:
        {
            if ( m_data->timer.isActive() )
            {
                // renew the exposed period
                m_data->timer.start( m_data->duration, this );
            }
            break;
        }
    }

    switch( static_cast< int >( event->type() ) )
    {
        case QEvent::KeyPress:
        {
            const auto keyEvent = static_cast< QKeyEvent* >( event );

            if( keyEvent->isAutoRepeat() && m_data->blockAutoRepeatKeyEvents )
            {
                /*
                    We swallow all auto repeated events to avoid running along
                    the tab chain by accident.
                 */
                return true;
            }

            if ( !isEnabled() && qskIsEnablingKey( keyEvent ) )
            {
                setEnabled( true );
                m_data->blockAutoRepeatKeyEvents = true;
                return true;
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

        case QEvent::Expose:
        case QEvent::FocusIn:
        case QEvent::FocusOut:
        {
            maybeEnable( event->type() != QEvent::FocusOut );
            break;
        }
    }

    return Inherited::eventFilter( object, event );
}

void FocusIndicator::windowChangeEvent( QskWindowChangeEvent* event )
{
    Inherited::windowChangeEvent( event );

    if ( m_data->isAutoDisabling() )
    {
        if ( auto w = event->oldWindow() )
            w->removeEventFilter( this );

        if( auto w = event->window() )
        {
            w->installEventFilter( this );
            maybeEnable( true );
        }
    }
}

void FocusIndicator::timerEvent( QTimerEvent* event )
{
    if ( m_data->isAutoDisabling() )
    {
        if( event->timerId() == m_data->timer.timerId() )
        {
            setEnabled( false );
            return;
        }
    }

    Inherited::timerEvent( event );
}

#include "moc_FocusIndicator.cpp"
