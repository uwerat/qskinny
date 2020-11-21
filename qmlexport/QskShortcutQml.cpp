/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskShortcutQml.h"
#include <QskShortcutMap.h>

#include <qquickitem.h>
#include <qquickwindow.h>

QSK_QT_PRIVATE_BEGIN
#include <QtGui/private/qguiapplication_p.h>
QSK_QT_PRIVATE_END

static inline QShortcutMap& qskShortcutMap()
{
    return QGuiApplicationPrivate::instance()->shortcutMap;
}

static bool qskContextMatcher( QObject* object, Qt::ShortcutContext context )
{
    if ( context == Qt::ApplicationShortcut )
        return true;

    if ( context == Qt::WindowShortcut )
    {
        if ( const auto shortcut = qobject_cast< const QskShortcutQml* >( object ) )
            return shortcut->isFocusInScope();
    }

    return false;
}

class QskShortcutQml::PrivateData
{
  public:
    PrivateData()
        : id( 0 )
        , autoRepeat( true )
        , enabled( true )
        , isWindowContext( true )
        , isComplete( true )
    {
    }

    ~PrivateData()
    {
        if ( id != 0 )
            qskShortcutMap().removeShortcut( id, nullptr );
    }

    void resetShortcut( QskShortcutQml* shortcut )
    {
        if ( !isComplete )
            return;

        auto& map = qskShortcutMap();

        const int oldId = id;

        if ( id != 0 )
        {
            map.removeShortcut( id, nullptr );
            id = 0;
        }

        if ( !sequence.isEmpty() )
        {
            id = map.addShortcut( shortcut, sequence,
                shortcut->context(), qskContextMatcher );

            if ( !autoRepeat )
                map.setShortcutAutoRepeat( false, id, shortcut );

            if ( !enabled )
                map.setShortcutEnabled( false, id, shortcut );
        }

        if ( oldId != id )
            Q_EMIT shortcut->shortcutIdChanged( id );
    }

  public:
    QKeySequence sequence;

    int id;

    bool autoRepeat : 1;
    bool enabled : 1;
    bool isWindowContext : 1;
    bool isComplete : 1;
};

QskShortcutQml::QskShortcutQml( QObject* parent )
    : Inherited( parent )
    , m_data( new PrivateData )
{
}

QskShortcutQml::QskShortcutQml( const QKeySequence& sequence, QObject* parent )
    : QskShortcutQml( sequence, Qt::WindowShortcut, parent )
{
}

QskShortcutQml::QskShortcutQml( const QKeySequence& sequence,
        Qt::ShortcutContext context, QObject* parent )
    : Inherited( parent )
    , m_data( new PrivateData )
{
    m_data->sequence = sequence;
    m_data->isWindowContext = ( context == Qt::WindowShortcut );
    m_data->resetShortcut( this );
}

QskShortcutQml::~QskShortcutQml()
{
}

int QskShortcutQml::shortcutId() const
{
    return m_data->id;
}

Qt::ShortcutContext QskShortcutQml::context() const
{
    return m_data->isWindowContext
        ? Qt::WindowShortcut : Qt::ApplicationShortcut;
}

void QskShortcutQml::setContext( Qt::ShortcutContext context )
{
    if ( context == Qt::ApplicationShortcut ||
        context == Qt::WindowShortcut )
    {
        const bool isWindowContext = ( context == Qt::WindowShortcut );

        if ( isWindowContext != m_data->isWindowContext )
        {
            m_data->isWindowContext = isWindowContext;
            m_data->resetShortcut( this );

            Q_EMIT contextChanged();
        }
    }
}

void QskShortcutQml::setSequence( const QKeySequence& sequence )
{
    if ( sequence != m_data->sequence )
    {
        m_data->sequence = sequence;
        m_data->resetShortcut( this );

        Q_EMIT sequenceChanged();
    }
}

QKeySequence QskShortcutQml::sequence() const
{
    return m_data->sequence;
}

void QskShortcutQml::setSequenceVariant( const QVariant& sequence )
{
    if ( sequence.userType() == QVariant::Int )
        setSequence( static_cast< QKeySequence::StandardKey >( sequence.toInt() ) );
    else
        setSequence( QKeySequence::fromString( sequence.toString() ) );
}

QVariant QskShortcutQml::sequenceVariant() const
{
    return m_data->sequence.toString();
}

void QskShortcutQml::setEnabled( bool on )
{
    if ( on != m_data->enabled )
    {
        m_data->enabled = on;

        if ( m_data->id != 0 )
            qskShortcutMap().setShortcutEnabled( on, m_data->id, this );

        Q_EMIT enabledChanged();
    }
}

bool QskShortcutQml::isEnabled() const
{
    return m_data->enabled;
}

void QskShortcutQml::setAutoRepeat( bool on )
{
    if ( on != m_data->autoRepeat )
    {
        m_data->autoRepeat = on;

        if ( m_data->id != 0 )
            qskShortcutMap().setShortcutEnabled( on, m_data->id, this );

        Q_EMIT autoRepeatChanged();
    }
}

bool QskShortcutQml::autoRepeat() const
{
    return m_data->autoRepeat;
}

bool QskShortcutQml::event( QEvent* event )
{
    if ( event->type() == QEvent::Shortcut )
    {
        auto* shortcutEvent = static_cast< QShortcutEvent* >( event );

        if ( shortcutEvent->shortcutId() == m_data->id )
        {
            if ( shortcutEvent->isAmbiguous() )
                Q_EMIT activatedAmbiguously();
            else
                Q_EMIT activated();

            return true;
        }
    }

    return false;
}

bool QskShortcutQml::isFocusInScope() const
{
    if ( !m_data->isWindowContext )
        return true;

    const QQuickItem* contextItem = nullptr;

    if ( parent()->isWindowType() )
    {
        if ( auto window = qobject_cast< const QQuickWindow* >( parent() ) )
            contextItem = window->contentItem();
    }
    else
    {
        contextItem = qobject_cast< const QQuickItem* >( parent() );
    }

    if ( contextItem )
    {
        return QskShortcutMap::contextMatcher( contextItem, Qt::WindowShortcut );
    }
    else
    {
        qWarning( "QskShortcut has no valid parent for Qt::WindowShortcut" );
        return false;
    }
}

void QskShortcutQml::classBegin()
{
    m_data->isComplete = false;
}

void QskShortcutQml::componentComplete()
{
    if ( m_data->isComplete == false )
    {
        m_data->isComplete = true;
        m_data->resetShortcut( this );
    }
}

#include "moc_QskShortcutQml.cpp"
