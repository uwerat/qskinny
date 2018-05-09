/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskInputContext.h"
#include "QskInputPanel.h"
#include "QskInputPanel.h"
#include "QskInputEngine.h"

#include "QskLinearBox.h"
#include "QskDialog.h"
#include "QskPopup.h"
#include "QskWindow.h"
#include "QskEvent.h"
#include "QskQuick.h"

#include <QPointer>
#include <QGuiApplication>

QSK_QT_PRIVATE_BEGIN
#include <private/qguiapplication_p.h>
QSK_QT_PRIVATE_END

#include <qpa/qplatformintegration.h>
#include <qpa/qplatforminputcontext.h>

static QPointer< QskInputContext > qskInputContext = nullptr;

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
    qAddPostRoutine( []{ delete qskInputContext; } );
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
    // item receiving the input
    QPointer< QQuickItem > inputItem;
    QPointer< QskInputPanel > inputPanel;

    // popup or window embedding the panel
    QskPopup* inputPopup = nullptr;
    QskWindow* inputWindow = nullptr;

    QskInputEngine* engine = nullptr;

    bool isPredictorDirty = true;
};

QskInputContext::QskInputContext():
    m_data( new PrivateData() )
{
    setObjectName( "InputContext" );
    m_data->engine = new QskInputEngine( this );
}

QskInputContext::~QskInputContext()
{
}

QQuickItem* QskInputContext::inputItem() const
{
    return m_data->inputItem;
}

QskInputPanel* QskInputContext::inputPanel() const
{
    if ( m_data->inputPanel == nullptr )
    {
        auto that = const_cast< QskInputContext* >( this );

        auto panel = new QskInputPanel();
        panel->setParent( that );

        connect( panel, &QQuickItem::visibleChanged,
            this, &QskInputContext::activeChanged );
        
        connect( panel, &QskControl::localeChanged,
            this, &QskInputContext::updateLocale );

        m_data->inputPanel = panel;
    }

    return m_data->inputPanel;
}

void QskInputContext::update( Qt::InputMethodQueries queries )
{
    if ( queries & Qt::ImEnabled )
    {
        QInputMethodQueryEvent event( Qt::ImEnabled );
        QCoreApplication::sendEvent( m_data->inputItem, &event );

        if ( !event.value( Qt::ImEnabled ).toBool() )
        {
            hidePanel();
            return;
        }
    }

    if ( auto panel = inputPanel() )
        panel->processInputMethodQueries( queries );
}

QRectF QskInputContext::panelRect() const
{
    if ( m_data->inputPopup )
        return m_data->inputPopup->geometry();

    return QRectF();
}

QskPopup* QskInputContext::createEmbeddingPopup( QskInputPanel* panel )
{
    auto popup = new QskPopup();

    popup->setAutoLayoutChildren( true );
    popup->setTransparentForPositioner( false );
    popup->setModal( true );

    auto box = new QskLinearBox( popup );
    box->addItem( panel );

    /*
        When the panel has an input proxy ( usually a local text input )
        we don't need to see the input item and display the overlay
        and align in the center of the window.
     */
    const bool hasInputProxy = panel->hasInputProxy();

    popup->setOverlay( hasInputProxy );

    if ( hasInputProxy )
        box->setMargins( QMarginsF( 5, 5, 5, 5 ) );
    else
        box->setExtraSpacingAt( Qt::TopEdge | Qt::LeftEdge | Qt::RightEdge );

    return popup;
}

QskWindow* QskInputContext::createEmbeddingWindow( QskInputPanel* panel )
{
    auto window = new QskWindow();

    window->setFlags( window->flags() & Qt::Dialog );
    //window->setModality( Qt::ApplicationModal );
    window->setAutoLayoutChildren( true );
#if 0
    window->setFlags( Qt::Tool | Qt::WindowDoesNotAcceptFocus );
#endif

    panel->setParentItem( window->contentItem() );

    return window;
}

void QskInputContext::showPanel()
{
    auto focusItem = qobject_cast< QQuickItem* >( qGuiApp->focusObject() );
    if ( focusItem == nullptr )
        return;

    auto panel = inputPanel();
    if ( panel == nullptr )
        return;

    if ( ( focusItem == panel )
        || qskIsAncestorOf( panel, focusItem ) )
    {
        // ignore: usually the input proxy of the panel
        return;
    }

    m_data->inputItem = focusItem;

    if ( QskDialog::instance()->policy() == QskDialog::TopLevelWindow )
    {
        // The input panel is embedded in a top level window

        delete m_data->inputPopup;

        if ( m_data->inputWindow == nullptr )
        {
            auto window = createEmbeddingWindow( panel );

            if ( window )
            {
                QSize size = window->effectivePreferredSize();
                if ( size.isEmpty() )
                {
                    // no idea, may be something based on the screen size
                    size = QSize( 800, 240 );
                }

                window->resize( size );
                window->show();

                window->setDeleteOnClose( true );
                window->installEventFilter( this );
            }

            m_data->inputWindow = window;
        }
    }
    else
    {
        // The input panel is embedded in a popup

        delete m_data->inputWindow;

        if ( m_data->inputPopup == nullptr )
        {
            auto popup = createEmbeddingPopup( panel );

            if ( popup )
            {
                popup->setParentItem( m_data->inputItem->window()->contentItem() );
                if ( popup->parent() == nullptr )
                    popup->setParent( this );

                popup->setVisible( true );
                popup->installEventFilter( this );
            }
    
            m_data->inputPopup = popup;
        }
    }

    updatePredictor();

    panel->setLocale( locale() );
    panel->attachInputItem( m_data->inputItem );
    panel->setEngine( m_data->engine );
}

void QskInputContext::hidePanel()
{
    if ( m_data->inputPopup )
    {
#if 1
        if ( auto focusItem = m_data->inputPopup->scopedFocusItem() )
        {
            /*
                Qt bug: QQuickItem::hasFocus() is not cleared
                when the corresponding focusScope gets deleted.
                Usually no problem, but here the focusItem is no
                child and will be reused with a different parent
                later.
             */
            focusItem->setFocus( false );
        }
#endif
    }

    if ( auto panel = inputPanel() )
    {
        panel->setParentItem( nullptr );
        panel->attachInputItem( nullptr );
        panel->setEngine( nullptr );
    }

    if ( m_data->inputPopup )
    {
        m_data->inputPopup->deleteLater();
    }

    if ( m_data->inputWindow )
    {
        QskWindow* window = m_data->inputWindow;
        m_data->inputWindow = nullptr;

        window->removeEventFilter( this );
        window->close(); // deleteOnClose is set
    }

    m_data->inputItem = nullptr;
}

void QskInputContext::setActive( bool on )
{
    if ( on )
        showPanel();
    else
        hidePanel();
}

bool QskInputContext::isActive() const
{
    if ( auto panel = inputPanel() )
    {
        return panel && panel->isVisible()
            && panel->window() && panel->window()->isVisible();
    }

    return false;
}

QLocale QskInputContext::locale() const
{
    if ( m_data->inputItem )
    {
        QInputMethodQueryEvent event( Qt::ImPreferredLanguage );
        QCoreApplication::sendEvent( m_data->inputItem, &event );

        return event.value( Qt::ImPreferredLanguage ).toLocale();
    }

    return QLocale();
}

void QskInputContext::updateLocale()
{
    m_data->isPredictorDirty = true;

    if ( isActive() )
        updatePredictor();

    qskSendToPlatformContext( QEvent::LocaleChange );
}

void QskInputContext::updatePredictor()
{
    if ( m_data->isPredictorDirty )
    {
        if ( m_data->engine )
        {
            m_data->engine->setPredictor( textPredictor( locale() ) );
            m_data->isPredictorDirty = false;
        }
    }
}

void QskInputContext::setFocusObject( QObject* focusObject )
{
    if ( m_data->inputItem == nullptr || m_data->inputItem == focusObject )
    {
        // we don't care
        return;
    }

    const auto w = m_data->inputItem->window();
    if ( w == nullptr )
        return;

    if ( m_data->inputWindow )
    {
        if ( focusObject == nullptr )
        {
            if ( m_data->inputItem->hasFocus() )
            {
                /*
                    As long as the focus is nowhere and
                    the local focus stay on the input item
                    we don't care
                 */

                return;
            }
        }
        else
        {
            const auto focusItem = qobject_cast< QQuickItem* >( focusObject );
            if ( focusItem && focusItem->window() == m_data->inputWindow )
                return;
        }
    }
    else if ( m_data->inputPopup )
    {
        if ( w->contentItem()->scopedFocusItem() == m_data->inputPopup )
        {
            /*
                As long as the focus stays inside the inputPopup
                we don't care
             */
            return;
        }
    }

    hidePanel();
    m_data->inputItem = nullptr;
}

QskTextPredictor* QskInputContext::textPredictor( const QLocale& ) const
{
    return nullptr;
}

void QskInputContext::processClickAt( int cursorPosition )
{
    Q_UNUSED( cursorPosition );
}

void QskInputContext::commitPrediction( bool )
{
    /*
        called, when the input item loses the focus.
        As it it should be possible to navigate inside of the
        inputPanel what should we do here ?
     */
}

bool QskInputContext::eventFilter( QObject* object, QEvent* event )
{
    if ( object == m_data->inputWindow )
    {
        switch( event->type() )
        {
            case QEvent::Move:
            {
                Q_EMIT panelRectChanged();
                break;
            }
            case QEvent::Resize:
            {
                if ( auto panel = inputPanel() )
                    panel->setSize( m_data->inputWindow->size() );

                break;
            }
            case QEvent::DeferredDelete:
            {
                m_data->inputWindow = nullptr;
                break;
            }
            default:
                break;
        }
    }
    else if ( object == m_data->inputPopup )
    {
        switch( static_cast< int >( event->type() ) )
        {
            case QskEvent::GeometryChange:
            {
                Q_EMIT panelRectChanged();
                break;
            }
            case QEvent::DeferredDelete:
            {
                m_data->inputPopup = nullptr;
                break;
            }
        }
    }

    return Inherited::eventFilter( object, event );
}

#include "moc_QskInputContext.cpp"
