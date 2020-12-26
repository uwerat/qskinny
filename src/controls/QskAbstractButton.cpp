/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskAbstractButton.h"
#include "QskAspect.h"
#include "QskEvent.h"
#include "QskQuick.h"

#include <qbasictimer.h>

// Flat/Checkable are no states - we need to get rid of them. TODO ...
QSK_SYSTEM_STATE( QskAbstractButton, Flat, QskAspect::FirstSystemState << 1 )
#if 1
// Wrong: we are overlapping with the user states, TODO ...
QSK_STATE( QskAbstractButton, Checked, QskAspect::LastUserState )
#endif
QSK_SYSTEM_STATE( QskAbstractButton, Pressed, QskAspect::LastSystemState >> 3 )
QSK_SYSTEM_STATE( QskAbstractButton, Checkable, QskAspect::LastSystemState >> 2 )

static QskAbstractButton* qskCheckedSibling( const QskAbstractButton* button )
{
    const auto parentItem = button->parentItem();
    if ( parentItem == nullptr )
        return nullptr;

    const auto siblings = parentItem->childItems();
    for ( auto sibling : siblings )
    {
        if ( auto btn = qobject_cast< QskAbstractButton* >( sibling ) )
        {
            if ( btn != button && btn->exclusive() && btn->isChecked() )
                return btn;
        }
    }

    return nullptr;
}

class QskAbstractButton::PrivateData
{
  public:
    PrivateData()
        : autoRepeatDelay( 300 )
        , autoRepeatInterval( 100 )
        , exclusive( false )
        , autoRepeat( false )
    {
    }

    QBasicTimer repeatTimer;

    int autoRepeatDelay;    // milliseconds
    int autoRepeatInterval; // milliseconds

    bool exclusive : 1;
    bool autoRepeat : 1;
};

QskAbstractButton::QskAbstractButton( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData() )
{
    setFocusPolicy( Qt::StrongFocus );
    setAcceptedMouseButtons( Qt::LeftButton );
    setAcceptHoverEvents( true );
}

QskAbstractButton::~QskAbstractButton()
{
}

void QskAbstractButton::click()
{
    setPressed( true );
    releaseButton();
}

void QskAbstractButton::releaseButton()
{
    if ( !isPressed() )
        return;

    if ( skinState() & Checkable )
    {
        // we will have toggled before released,
        // maybe there is more work to have the signals coming
        // in a logical order. TODO ...

        setCheckedState( !( skinState() & Checked ) );
    }

    setPressed( false );
    Q_EMIT clicked();
}

void QskAbstractButton::setCheckedState( bool on )
{
    setChecked( on );
}

void QskAbstractButton::toggle()
{
    setChecked( !( skinState() & Checked ) );
}

bool QskAbstractButton::isPressed() const
{
    return skinState() & Pressed;
}

void QskAbstractButton::setPressed( bool on )
{
    if ( on == isPressed() )
        return;

    setSkinStateFlag( Pressed, on );
    Q_EMIT pressedChanged( on );

    if ( on )
        Q_EMIT pressed();
    else
        Q_EMIT released();

    if ( m_data->autoRepeat )
    {
        if ( on )
        {
            m_data->repeatTimer.start( m_data->autoRepeatDelay, this );
        }
        else
        {
            m_data->repeatTimer.stop();
        }
    }
}

void QskAbstractButton::setCheckable( bool on )
{
    if ( on == isCheckable() )
        return;

    setSkinStateFlag( Checkable, on );
    Q_EMIT checkableChanged( on );
}

bool QskAbstractButton::isCheckable() const
{
    return skinState() & Checkable;
}

void QskAbstractButton::setChecked( bool on )
{
    if ( on == isChecked() )
        return;

    if ( m_data->exclusive && !on )
    {
        // an exclusive button can't be unchecked
        return;
    }

    auto checkedButton = qskCheckedSibling( this );
    if ( checkedButton )
    {
        // we enter temporary state, where no buttons are selected,
        // better delay the notifications

        checkedButton->setSkinStateFlag( Checked, false );
    }

    setSkinStateFlag( Checked, on );
    Q_EMIT checkedChanged( on );
    Q_EMIT toggled( on );

    if ( checkedButton )
    {
        Q_EMIT checkedButton->checkedChanged( false );
        Q_EMIT checkedButton->toggled( false );
    }
}

bool QskAbstractButton::isChecked() const
{
    return skinState() & Checked;
}

void QskAbstractButton::setAutoRepeat( bool on )
{
    if ( on != m_data->autoRepeat )
    {
        m_data->autoRepeat = on;

        if ( m_data->autoRepeat && isPressed() )
            m_data->repeatTimer.start( m_data->autoRepeatDelay, this );
        else
            m_data->repeatTimer.stop();

        Q_EMIT autoRepeatChanged( on );
    }
}

bool QskAbstractButton::autoRepeat() const
{
    return m_data->autoRepeat;
}

void QskAbstractButton::setAutoRepeatDelay( int ms )
{
    if ( ms != m_data->autoRepeatDelay )
    {
        m_data->autoRepeatDelay = ms;
        Q_EMIT autoRepeatDelayChanged();
    }
}

int QskAbstractButton::autoRepeatDelay() const
{
    return m_data->autoRepeatDelay;
}

void QskAbstractButton::setAutoRepeatInterval( int ms )
{
    if ( ms != m_data->autoRepeatInterval )
    {
        m_data->autoRepeatInterval = ms;
        Q_EMIT autoRepeatIntervalChanged();
    }
}

int QskAbstractButton::autoRepeatInterval() const
{
    return m_data->autoRepeatInterval;
}

void QskAbstractButton::setExclusive( bool on )
{
    if ( on != m_data->exclusive )
    {
        m_data->exclusive = on;
        Q_EMIT exclusiveChanged( on );
    }
}

bool QskAbstractButton::exclusive() const
{
    return m_data->exclusive;
}

bool QskAbstractButton::event( QEvent* event )
{
    const auto eventType = static_cast< int >( event->type() );
    switch ( eventType )
    {
        case QEvent::Shortcut:
        {
            // TODO
            // setFocus( true, Qt::ShortcutFocusReason );
            break;
        }
        case QskEvent::WindowChange:
        {
            if ( qskIsItemComplete( this ) && isPressed() )
            {
                /*
                    When the window change happens on pressed() we won't get
                    the corrsponding release.
                 */
                setPressed( false );
            }

            break;
        }
    }

    return Inherited::event( event );
}

void QskAbstractButton::keyPressEvent( QKeyEvent* event )
{
    switch ( event->key() )
    {
        case Qt::Key_Select:
        case Qt::Key_Space:
        {
            if ( !event->isAutoRepeat() )
                setPressed( true );

            // always accepting
            return;
        }
    }

    Inherited::keyPressEvent( event );
}

void QskAbstractButton::keyReleaseEvent( QKeyEvent* event )
{
    if ( !event->isAutoRepeat() )
    {
        switch ( event->key() )
        {
            case Qt::Key_Select:
            case Qt::Key_Space:
            {
                releaseButton();
                return;
            }
            default:
                break;
        }
    }

    Inherited::keyReleaseEvent( event );
}

void QskAbstractButton::mouseMoveEvent( QMouseEvent* event )
{
    if ( isPressed() )
    {
        if ( !contains( event->pos() ) )
        {
            setPressed( false );
            Q_EMIT canceled();
        }
    }

    event->accept();
}

void QskAbstractButton::mousePressEvent( QMouseEvent* )
{
    // QGuiApplication::styleHints()->mousePressAndHoldInterval() ???
    setPressed( true );
}

void QskAbstractButton::mouseReleaseEvent( QMouseEvent* )
{
    releaseButton();
}

void QskAbstractButton::mouseUngrabEvent()
{
    if ( isPressed() )
    {
        setPressed( false );
        Q_EMIT canceled();
    }
}

void QskAbstractButton::focusInEvent( QFocusEvent* event )
{
    Inherited::focusInEvent( event );
}

void QskAbstractButton::focusOutEvent( QFocusEvent* event )
{
    if ( isPressed() )
    {
        setPressed( false );
        Q_EMIT canceled();
    }

    Inherited::focusOutEvent( event );
}

void QskAbstractButton::timerEvent( QTimerEvent* event )
{
    if ( event->timerId() == m_data->repeatTimer.timerId() )
    {
        if ( isPressed() )
        {
            Q_EMIT released();
            Q_EMIT clicked();
            Q_EMIT pressed();

            m_data->repeatTimer.start( m_data->autoRepeatInterval, this );
        }
        else
        {
            m_data->repeatTimer.stop();
        }

        return;
    }

    Inherited::timerEvent( event );
}

#include "moc_QskAbstractButton.cpp"
