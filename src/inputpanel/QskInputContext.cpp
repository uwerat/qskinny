/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskInputContext.h"
#include "QskInputPanel.h"
#include "QskInputPanelBox.h"

#include "QskDialog.h"
#include "QskLinearBox.h"
#include "QskPopup.h"
#include "QskQuick.h"
#include "QskTextPredictor.h"
#include "QskWindow.h"

#include <qguiapplication.h>
#include <qmap.h>
#include <qpointer.h>

QSK_QT_PRIVATE_BEGIN
#include <private/qguiapplication_p.h>
QSK_QT_PRIVATE_END

#include <qpa/qplatforminputcontext.h>
#include <qpa/qplatformintegration.h>

#if HUNSPELL
#include "QskHunspellTextPredictor.h"
#endif

namespace
{
    class Panel final : public QskInputPanel
    {
      public:
        Panel( QQuickItem* parent = nullptr )
            : QskInputPanel( parent )
        {
            setAutoLayoutChildren( true );
            setLayoutAlignmentHint( Qt::AlignCenter );

            m_box = new QskInputPanelBox( this );

            connect( m_box, &QskInputPanelBox::keySelected,
                this, &QskInputPanel::keySelected );

            connect( m_box, &QskInputPanelBox::predictiveTextSelected,
                this, &QskInputPanel::predictiveTextSelected );
        }

        void attachItem( QQuickItem* item ) override
        {
            m_box->attachInputItem( item );
        }

        QQuickItem* inputProxy() const override
        {
            return m_box->inputProxy();
        }

        void setPrompt( const QString& prompt ) override
        {
            m_box->setInputPrompt( prompt );
        }

        void setPredictionEnabled( bool on ) override
        {
            m_box->setPanelHint( QskInputPanelBox::Prediction, on );
        }

        void setPrediction( const QStringList& prediction ) override
        {
            QskInputPanel::setPrediction( prediction );
            m_box->setPrediction( prediction );
        }

      private:
        QskInputPanelBox* m_box;
    };

    class Channel
    {
      public:
        // item receiving the input
        QPointer< QQuickItem > item;

        // panel for inserting the input
        QPointer< QskInputPanel > panel;

        // popup or window embedding the panel
        QPointer< QskPopup > popup;
        QPointer< QskWindow > window;
    };

    class ChannelTable
    {
      public:
        inline Channel* currentChannel() const
        {
            const auto object = QGuiApplication::focusObject();
            return channel( qobject_cast< const QQuickItem* >( object ) );
        }

        inline Channel* channel( const QQuickWindow* window ) const
        {
            if ( window )
            {
                auto it = m_map.constFind( window );
                if ( it != m_map.constEnd() )
                    return const_cast< Channel* >( &it.value() );
            }

            return nullptr;
        }

        inline Channel* channel( const QskInputPanel* panel ) const
        {
            if ( panel )
            {
                QMap< const QQuickWindow*, Channel >::const_iterator it;

                for( it = m_map.constBegin(); it != m_map.constEnd(); ++it )
                {
                    if( it.value().panel == panel )
                        return const_cast< Channel* >( &it.value() );
                }
            }

            return nullptr;
        }

        inline Channel* channel( const QQuickItem* item ) const
        {
            if ( item )
            {
                // item->window() might already been gone
                QMap< const QQuickWindow*, Channel >::const_iterator it;

                for( it = m_map.constBegin(); it != m_map.constEnd(); ++it )
                {
                    if( it.value().item == item )
                        return const_cast< Channel* >( &it.value() );
                }
            }

            return nullptr;
        }

        inline Channel* ancestorChannel( const QQuickItem* item ) const
        {
            for ( auto it = m_map.constBegin();
                it != m_map.constEnd(); ++it )
            {
                if ( const auto panel = it.value().panel )
                {
                    if ( ( item == panel ) || qskIsAncestorOf( panel, item ) )
                    {
                        return const_cast< Channel* >( &it.value() );
                    }
                }
            }

            return nullptr;
        }

        inline Channel* insert( const QQuickWindow* window )
        {
            return &m_map[ window ];
        }

        inline void remove( const QQuickWindow* window )
        {
            m_map.remove( window );
        }

      private:
        QMap< const QQuickWindow*, Channel > m_map;
    };
}

static QPointer< QskInputContext > qskInputContext;

static void qskSendToPlatformContext( QEvent::Type type )
{
    const auto platformInputContext =
        QGuiApplicationPrivate::platformIntegration()->inputContext();

    if ( platformInputContext )
    {
        QEvent event( type );
        QCoreApplication::sendEvent( platformInputContext, &event );
    }
}

static void qskInputContextHook()
{
    qAddPostRoutine( [] { delete qskInputContext; } );
}

Q_COREAPP_STARTUP_FUNCTION( qskInputContextHook )

void QskInputContext::setInstance( QskInputContext* inputContext )
{
    if ( inputContext != qskInputContext )
    {
        const auto oldContext = qskInputContext;
        qskInputContext = inputContext;

        if ( oldContext && oldContext->parent() == nullptr )
            delete oldContext;

        qskSendToPlatformContext( QEvent::PlatformPanel );
    }
}

QskInputContext* QskInputContext::instance()
{
    return qskInputContext;
}

class QskInputContext::PrivateData
{
  public:
    inline QskInputPanel* createPanel( QskInputContext* context ) const
    {
        QskInputPanel* panel = nullptr;

        if ( this->factory )
            panel = this->factory->createPanel();

        if ( panel == nullptr )
            panel = new Panel();

        connect( panel, &QskInputPanel::visibleChanged,
            context, &QskInputContext::activeChanged );

        connect( panel, &QskInputPanel::localeChanged,
            context, [] { qskSendToPlatformContext( QEvent::LocaleChange ); } );

        connect( panel, &QskInputPanel::inputItemDestroyed,
            context, [ context, panel ] { context->hideChannel( panel ); } );

        return panel;
    }

    inline QskPopup* createPopup( QskInputPanel* panel ) const
    {
        auto popup = new QskPopup();

        popup->setAutoLayoutChildren( true );
        popup->setTransparentForPositioner( false );
        popup->setMargins( 5 );
        popup->setModal( true );

        panel->setParentItem( popup );
        if ( panel->parent() == nullptr )
            panel->setParent( popup );

        return popup;
    }

    inline QskWindow* createWindow( QskInputPanel* panel ) const
    {
        auto window = new QskWindow();

        window->setFlags( window->flags() & Qt::Dialog );
        // window->setModality( Qt::ApplicationModal );
        window->setAutoLayoutChildren( true );
#if 0
        window->setFlags( Qt::Tool | Qt::WindowDoesNotAcceptFocus );
#endif

        panel->setParentItem( window->contentItem() );

        return window;
    }

    void closeChannel( Channel* channel )
    {
        if ( channel->popup )
            channel->popup->close(); // deleteOnClose is set

        if ( channel->window )
            channel->window->close(); // deleteOnClose is set
    }

    ChannelTable channels;
    QPointer< QskInputContextFactory > factory;
};

QskInputContext::QskInputContext()
    : m_data( new PrivateData() )
{
    setObjectName( "InputContext" );
}

QskInputContext::~QskInputContext()
{
}

void QskInputContext::setFactory( QskInputContextFactory* factory )
{
    if ( m_data->factory == factory )
        return;

    if ( m_data->factory && m_data->factory->parent() == this )
        delete m_data->factory;

    m_data->factory = factory;

    if ( factory && factory->parent() == nullptr )
        factory->setParent( this );
}

QskInputContextFactory* QskInputContext::factory() const
{
    return m_data->factory;
}

std::shared_ptr<QskTextPredictor> QskInputContext::textPredictor( const QLocale& locale )
{
    if ( m_data->factory )
        return m_data->factory->setupPredictor( locale );

    return nullptr;
}

void QskInputContext::update( const QQuickItem* item, Qt::InputMethodQueries queries )
{
    if ( item == nullptr )
    {
        // those are coming from QQuickWindow based on focus changes
        item = qobject_cast< QQuickItem* >( QGuiApplication::focusObject() );
    }

    auto channel = m_data->channels.channel( item );
    if ( channel == nullptr )
        return;

    if ( queries & Qt::ImEnabled )
    {
        QInputMethodQueryEvent event( Qt::ImEnabled );
        QCoreApplication::sendEvent( channel->item, &event );

        if ( !event.value( Qt::ImEnabled ).toBool() )
        {
            hidePanel( item );
            return;
        }
    }

    channel->panel->updateInputPanel( queries );
}

QRectF QskInputContext::panelRect() const
{
    /*
        As we can have more than panel at the same time we
        better don't return any geometry
     */

    return QRectF();
}

void QskInputContext::showPanel( const QQuickItem* item )
{
    if ( item == nullptr )
        return;

    if ( m_data->channels.ancestorChannel( item ) )
    {
        // We are inside of an existing panel
        return;
    }

    if ( auto channel = m_data->channels.channel( item->window() ) )
    {
        if ( channel->item == item )
            return;

        hidePanel( channel->item );
    }

    auto panel = m_data->createPanel( this );

    auto channel = m_data->channels.insert( item->window() );
    channel->item = const_cast< QQuickItem* >( item );
    channel->panel = panel;

    if ( QskDialog::instance()->policy() == QskDialog::TopLevelWindow )
    {
        // The input panel is embedded in a top level window

        auto window = m_data->createWindow( panel );

        QSize size = window->sizeConstraint();
        if ( size.isEmpty() )
        {
            // no idea, may be something based on the screen size
            size = QSize( 800, 240 );
        }

        window->resize( size );
        window->show();

        window->setDeleteOnClose( true );

        channel->window = window;
    }
    else
    {
        // The input panel is embedded in a popup

        auto popup = m_data->createPopup( panel );

        popup->setPopupFlag( QskPopup::DeleteOnClose, true );
        popup->setParentItem( item->window()->contentItem() );
        popup->setParent( this );

        channel->popup = popup;

        popup->open();
    }

    panel->attachInputItem( const_cast< QQuickItem* >( item ) );
}

void QskInputContext::hidePanel( const QQuickItem* item )
{
    if ( item == nullptr )
        return;

    if ( auto channel = m_data->channels.channel( item ) )
    {
        m_data->closeChannel( channel );
        m_data->channels.remove( item->window() );
    }
}

void QskInputContext::hideChannel( const QskInputPanel* panel )
{
    if ( auto channel = m_data->channels.channel( panel ) )
    {
        // channel->item is already dead here
        m_data->closeChannel( channel );
        m_data->channels.remove( panel->window() );
    }
}

void QskInputContext::setInputPanelVisible( const QQuickItem* item, bool on )
{
    // called from inside the controls

    if ( item == nullptr )
        item = qobject_cast< QQuickItem* >( QGuiApplication::focusObject() );

    if ( item )
    {
        if ( on )
            showPanel( item );
        else
            hidePanel( item );
    }
}

bool QskInputContext::isInputPanelVisible() const
{
    return m_data->channels.currentChannel() != nullptr;
}

QLocale QskInputContext::locale() const
{
    if ( auto channel = m_data->channels.currentChannel() )
    {
        if ( channel->panel )
            return channel->panel->locale();
    }

    return QLocale();
}

void QskInputContext::setFocusObject( QObject* )
{
}

void QskInputContext::invokeAction(
    QInputMethod::Action, int cursorPosition )
{
    // called from qquicktextinput/qquicktextedit
    Q_UNUSED( cursorPosition )
}

void QskInputContext::commitPrediction( bool )
{
    /*
        called, when the input item loses the focus.
        As it it should be possible to navigate inside of the
        panel what should we do here ?
     */
}

class QskInputContextFactory::PrivateData
{
  public:
    QThread* thread = nullptr;
    std::shared_ptr< QskTextPredictor > predictor;
    QLocale predictorLocale;
};

QskInputContextFactory::QskInputContextFactory( QObject* parent )
    : QObject( parent )
    , m_data( new PrivateData() )
{
}

QskInputContextFactory::~QskInputContextFactory()
{
    if( m_data->thread )
    {
        m_data->thread->quit();
        connect( m_data->thread, &QThread::finished, m_data->thread, &QObject::deleteLater );
    }
}

std::shared_ptr< QskTextPredictor > QskInputContextFactory::setupPredictor( const QLocale& locale )
{
    if( !m_data->predictor
        || m_data->predictorLocale.language() != locale.language()
        || m_data->predictorLocale.country() != locale.country() )
    {
        m_data->predictor = std::shared_ptr< QskTextPredictor >( createPredictor( locale ) );
        m_data->predictorLocale = QLocale( locale.language(), locale.country() );

        if( m_data->predictor )
        {
            if( !m_data->thread )
            {
                m_data->thread = new QThread();
                m_data->thread->start();
            }

            m_data->predictor->moveToThread( m_data->thread );
        }
    }

    return m_data->predictor;
}

QskTextPredictor* QskInputContextFactory::createPredictor( const QLocale& locale )
{
#if HUNSPELL
   return new QskHunspellTextPredictor( locale );
#else
    Q_UNUSED( locale );
#endif

   return nullptr;
}

QskInputPanel* QskInputContextFactory::createPanel() const
{
    return new Panel();
}

#include "moc_QskInputContext.cpp"
