#include "FocusIndicator.h"

#include <QskEvent.h>
#include <QskAnimationHint.h>
#include <QskBoxBorderMetrics.h>
#include <QskBoxShapeMetrics.h>
#include <QskBoxBorderColors.h>
#include <QskRgbValue.h>

#include <qquickwindow.h>
#include <qbasictimer.h>


QSK_STATE( FocusIndicator, Concealed, QskAspect::FirstUserState )

static inline bool qskIsExposingKeyPress( const QKeyEvent* event )
{
    // what keys do we want have here ???
    return qskIsButtonPressKey( event ) || qskFocusChainIncrement( event );
}

static void qskMaybeExpose( FocusIndicator* indicator, bool on  )
{
    Q_UNUSED( indicator );
    Q_UNUSED( on );

#if 0
    if ( on )
    {
        if ( auto w = indicator->window() )
        {
            if ( w->isExposed() && w->isActive() )
                indicator->setExposed( true );
        }
    }
    else
    {
        indicator->setExposed( false );
    }
#endif
}

class FocusIndicator::PrivateData
{
  public:
    inline bool isAutoConcealing() const { return timeout > 0; }

    int timeout = 0;
    QBasicTimer concealTimer;

    bool blockAutoRepeatKeyEvents = false;
};

FocusIndicator::FocusIndicator( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData() )
{
#if 1
    auto colors = boxBorderColorsHint( Panel );

    setBoxBorderColorsHint( Panel | Concealed, QskRgb::Transparent );

    setAnimationHint( Panel | QskAspect::Color, 200 );
    setAnimationHint( Panel | QskAspect::Color | Concealed, 500 );
#endif

    setExposedTimeout( 4500 );
}

FocusIndicator::~FocusIndicator()
{
}

void FocusIndicator::setExposedTimeout( int ms )
{
    ms = std::max( ms, 0 );
    if ( ms == m_data->timeout )
        return;

    m_data->timeout = ms;

    if ( m_data->isAutoConcealing() )
    {
        if ( auto w = window() )
            w->installEventFilter( this );

        if ( isExposed() )
        {
            if ( isInitiallyPainted() )
                m_data->concealTimer.start( m_data->timeout, this );
            else
                setExposed( false );
        }
    }
    else
    {
        if ( auto w = window() )
            w->removeEventFilter( this );

        setExposed( true );
    }

    Q_EMIT exposedTimeoutChanged( ms );
}

int FocusIndicator::exposedTimeout() const
{
    return m_data->timeout;
}

void FocusIndicator::setExposed( bool on )
{
    if ( on == isExposed() )
        return;

    setSkinStateFlag( Concealed, !on );

    if ( m_data->isAutoConcealing() )
    {
        if ( on )
        {
            const auto hint = animationHint( Panel | QskAspect::Color );
            m_data->concealTimer.start( m_data->timeout + hint.duration, this );
        }
        else
        {
            m_data->concealTimer.stop();
        }
    }

    Q_EMIT exposedChanged( on );
}

bool FocusIndicator::eventFilter( QObject* object, QEvent* event )
{
    if( ( object != window() ) || !m_data->isAutoConcealing() )
        return Inherited::eventFilter( object, event );

    switch( static_cast< int >( event->type() ) )
    {
        case QEvent::KeyPress:
        case QEvent::KeyRelease:
        case QEvent::ShortcutOverride:
            if ( m_data->concealTimer.isActive() )
            {
                // renew the exposed period
                m_data->concealTimer.start( m_data->timeout, this );
            }
            break;
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

            if ( !isExposed() && qskIsExposingKeyPress( keyEvent ) )
            {
                setExposed( true );
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
            qskMaybeExpose( this, event->type() != QEvent::FocusOut );
            break;
        }
    }

    return Inherited::eventFilter( object, event );
}

void FocusIndicator::windowChangeEvent( QskWindowChangeEvent* event )
{
    Inherited::windowChangeEvent( event );

    if ( m_data->isAutoConcealing() )
    {
        if ( auto w = event->oldWindow() )
            w->removeEventFilter( this );

        if( auto w = event->window() )
        {
            w->installEventFilter( this );
            qskMaybeExpose( this, true );
        }
    }
}

void FocusIndicator::timerEvent( QTimerEvent* event )
{
    if ( m_data->isAutoConcealing() )
    {
        if( event->timerId() == m_data->concealTimer.timerId() )
        {
            setExposed( false );
            return;
        }
    }

    Inherited::timerEvent( event );
}

bool FocusIndicator::isExposed() const
{
    return !hasSkinState( Concealed );
}

#include "moc_FocusIndicator.cpp"
