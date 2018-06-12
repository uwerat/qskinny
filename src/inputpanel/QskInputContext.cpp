/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskInputContext.h"
#include "QskInputPanel.h"
#include "QskInputPanelBox.h"

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

namespace
{
    class Panel final : public QskInputPanel
    {
    public:
        Panel( QQuickItem* parent = nullptr ):
            QskInputPanel( parent )
        {
            setAutoLayoutChildren( true );

            m_box = new QskInputPanelBox( this );

            connect( m_box, &QskInputPanelBox::keySelected,
                this, &QskInputPanel::keySelected );

            connect( m_box, &QskInputPanelBox::predictiveTextSelected,
                this, &QskInputPanel::predictiveTextSelected );
        }

        virtual void attachItem( QQuickItem* item ) override
        {
            m_box->attachInputItem( item );
        }

        virtual QQuickItem* inputProxy() const override
        {
            return m_box->inputProxy();
        }

        virtual void setPrompt( const QString& prompt ) override
        {
            m_box->setInputPrompt( prompt );
        }

        virtual void setPredictionEnabled( bool on ) override
        {
            m_box->setPanelHint( QskInputPanelBox::Prediction, on );
        }

        virtual void setPrediction( const QStringList& prediction ) override
        {
            m_box->setPrediction( prediction );
        }

    private:
        QskInputPanelBox* m_box;
    };
}

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
    // item receiving the input
    QPointer< QQuickItem > inputItem;
    QPointer< QskInputPanel > panel;

    // popup or window embedding the panel
    QskPopup* inputPopup = nullptr;
    QskWindow* inputWindow = nullptr;

    QPointer< QskInputContextFactory > factory;
};

QskInputContext::QskInputContext():
    m_data( new PrivateData() )
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

QskTextPredictor* QskInputContext::textPredictor( const QLocale& locale )
{
    if ( m_data->factory )
        return m_data->factory->createPredictor( locale );

    return nullptr;
}

QQuickItem* QskInputContext::inputItem() const
{
    return m_data->inputItem;
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

    if ( m_data->panel )
        m_data->panel->updateInputPanel( queries );
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

    const auto alignment = panel->alignment() & Qt::AlignVertical_Mask;
    popup->setOverlay( alignment == Qt::AlignVCenter );

    switch( alignment )
    {
        case Qt::AlignTop:
        {
            box->setExtraSpacingAt( Qt::BottomEdge | Qt::LeftEdge | Qt::RightEdge );
            break;
        }
        case Qt::AlignVCenter:
        {
            box->setMargins( QMarginsF( 5, 5, 5, 5 ) );
            break;
        }

        case Qt::AlignBottom:
        default:
        {
            box->setExtraSpacingAt( Qt::TopEdge | Qt::LeftEdge | Qt::RightEdge );
        }
    }

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

void QskInputContext::ensurePanel()
{
    if ( m_data->panel )
        return;

    QskInputPanel* panel = nullptr;

    if ( m_data->factory )
        panel = m_data->factory->createPanel();

    if ( panel == nullptr )
        panel = new Panel();

    panel->setParent( const_cast< QskInputContext* >( this ) );

    connect( panel, &QskInputPanel::visibleChanged,
        this, &QskInputContext::activeChanged,
        Qt::UniqueConnection );

    connect( panel, &QskInputPanel::localeChanged,
        this, [] { qskSendToPlatformContext( QEvent::LocaleChange ); },
        Qt::UniqueConnection );

    m_data->panel = panel;
}

void QskInputContext::showPanel()
{
    auto focusItem = qobject_cast< QQuickItem* >( qGuiApp->focusObject() );
    if ( focusItem == nullptr )
        return;

    ensurePanel();

    if ( ( focusItem == m_data->panel )
        || qskIsAncestorOf( m_data->panel, focusItem ) )
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
            auto window = createEmbeddingWindow( m_data->panel );

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
            auto popup = createEmbeddingPopup( m_data->panel );

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

    m_data->panel->attachInputItem( m_data->inputItem );
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

    if ( m_data->panel )
    {
        m_data->panel->setParentItem( nullptr );
        m_data->panel->disconnect( this );

        m_data->panel->attachInputItem( nullptr );
    }

    if ( m_data->inputPopup )
        m_data->inputPopup->deleteLater();

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
    const QQuickWindow* window = m_data->inputWindow;
    if ( window == nullptr && m_data->inputPopup )
        window = m_data->inputPopup->window();

    return window && window->isVisible();
}

QLocale QskInputContext::locale() const
{
    if ( m_data->panel )
        return m_data->panel->locale();

    return QLocale();
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

void QskInputContext::processClickAt( int cursorPosition )
{
    Q_UNUSED( cursorPosition );
}

void QskInputContext::commitPrediction( bool )
{
    /*
        called, when the input item loses the focus.
        As it it should be possible to navigate inside of the
        panel what should we do here ?
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
                if ( m_data->panel )
                    m_data->panel->setSize( m_data->inputWindow->size() );

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

QskInputContextFactory::QskInputContextFactory( QObject* parent ):
    QObject( parent )
{
}

QskInputContextFactory::~QskInputContextFactory()
{
}

QskTextPredictor* QskInputContextFactory::createPredictor( const QLocale& ) const
{
    return nullptr;
}

QskInputPanel* QskInputContextFactory::createPanel() const
{
    return new Panel();
}

#include "moc_QskInputContext.cpp"
