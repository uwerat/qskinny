/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskInputContext.h"
#include "QskInputPanel.h"
#include "QskTextPredictor.h"
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

static QPointer< QskInputPanel > qskInputPanel = nullptr;

static void qskDeletePanel()
{
    delete qskInputPanel;
}

static void qskInputPanelHook()
{
    qAddPostRoutine( qskDeletePanel );
}

Q_COREAPP_STARTUP_FUNCTION( qskInputPanelHook )

static void qskSetInputPanel( QskInputPanel* inputPanel )
{
    if ( inputPanel == qskInputPanel )
        return;

    delete qskInputPanel;
    qskInputPanel = inputPanel;
}

void QskInputContext::setInputPanel( QskInputPanel* inputPanel )
{
    if ( inputPanel == qskInputPanel )
        return;

    qskSetInputPanel( inputPanel );

    const auto inputContext =
        QGuiApplicationPrivate::platformIntegration()->inputContext();

    if ( auto context = qobject_cast< QskInputContext* >( inputContext ) )
        context->hideInputPanel();
}

QskInputPanel* QskInputContext::inputPanel()
{
    return qskInputPanel;
}

static inline uint qskHashLocale( const QLocale& locale )
{
    return uint( locale.language() + ( uint( locale.country() ) << 16 ) );
}

namespace
{
    class PredictorTable
    {
    public:
        void replace( const QLocale& locale, QskTextPredictor* predictor )
        {
            const auto key = qskHashLocale( locale );

            if ( predictor )
            {
                const auto it = hashTab.find( key );
                if ( it != hashTab.end() )
                {
                    if ( it.value() == predictor )
                        return;

                    delete it.value();
                    *it = predictor;
                }
                else
                {
                    hashTab.insert( key, predictor );
                }
            }
            else
            {
                const auto it = hashTab.find( key );
                if ( it != hashTab.end() )
                {
                    delete it.value();
                    hashTab.erase( it );
                }
            }
        }

        QskTextPredictor* find( const QLocale& locale )
        {
            const auto key = qskHashLocale( locale );
            return hashTab.value( key, nullptr );
        }

    private:
        QHash< uint, QskTextPredictor* > hashTab;
    };
}

class QskInputContext::PrivateData
{
public:
    // item receiving the input
    QPointer< QQuickItem > inputItem;

    // popup or window embedding qskInputPanel
    QskPopup* inputPopup = nullptr;
    QskWindow* inputWindow = nullptr;

    PredictorTable predictorTable;

    QskInputEngine* engine = nullptr;
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

bool QskInputContext::isValid() const
{
    return true;
}

bool QskInputContext::hasCapability( Capability ) const
{
    // what is QPlatformInputContext::HiddenTextCapability ???
    return true;
}

QQuickItem* QskInputContext::inputItem()
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
            hideInputPanel();
            return;
        }
    }

    if ( qskInputPanel )
        qskInputPanel->processInputMethodQueries( queries );
}

QRectF QskInputContext::keyboardRect() const
{
    // is this correct and what is this good for ?
    if ( m_data->inputPopup )
        return m_data->inputPopup->geometry();

    return Inherited::keyboardRect();
}

bool QskInputContext::isAnimating() const
{
    // can be implemented once we have some sliding/fading effects
    return false;
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

void QskInputContext::showInputPanel()
{
    auto focusItem = qobject_cast< QQuickItem* >( qGuiApp->focusObject() );

    if ( focusItem == nullptr )
        return;

    if ( ( focusItem == qskInputPanel )
        || qskIsAncestorOf( qskInputPanel, focusItem ) )
    {
        // ignore: usually the input proxy of the panel
        return;
    }

    m_data->inputItem = focusItem;

    if ( qskInputPanel == nullptr )
        qskSetInputPanel( new QskInputPanel() );

    connect( qskInputPanel, &QQuickItem::visibleChanged,
        this, &QPlatformInputContext::emitInputPanelVisibleChanged,
        Qt::UniqueConnection );

    connect( qskInputPanel, &QskControl::localeChanged,
        this, &QPlatformInputContext::emitLocaleChanged,
        Qt::UniqueConnection );

    if ( QskDialog::instance()->policy() == QskDialog::TopLevelWindow )
    {
        // The input panel is embedded in a top level window

        delete m_data->inputPopup;

        if ( m_data->inputWindow == nullptr )
        {
            auto window = createEmbeddingWindow( qskInputPanel );

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
            auto popup = createEmbeddingPopup( qskInputPanel );

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

    m_data->engine->setPredictor(
        m_data->predictorTable.find( locale() ) );

    qskInputPanel->setLocale( locale() );
    qskInputPanel->attachInputItem( m_data->inputItem );
    qskInputPanel->setEngine( m_data->engine );
}

void QskInputContext::hideInputPanel()
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

    if ( qskInputPanel )
    {
        qskInputPanel->setParentItem( nullptr );
        qskInputPanel->attachInputItem( nullptr );
        qskInputPanel->setEngine( nullptr );
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

bool QskInputContext::isInputPanelVisible() const
{
    return qskInputPanel && qskInputPanel->isVisible()
        && qskInputPanel->window() && qskInputPanel->window()->isVisible();
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

Qt::LayoutDirection QskInputContext::inputDirection() const
{
    return Inherited::inputDirection();
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
                    As long as the focus is noewhere and
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

    hideInputPanel();
    m_data->inputItem = nullptr;
}

void QskInputContext::registerPredictor(
    const QLocale& locale, QskTextPredictor* predictor )
{
    auto oldPredictor = m_data->predictorTable.find( locale );
    if ( predictor == oldPredictor )
        return;

    if ( predictor )
        predictor->setParent( this );

    m_data->predictorTable.replace( locale, predictor );

    if ( oldPredictor )
        delete oldPredictor;

    if ( qskHashLocale( locale ) == qskHashLocale( this->locale() ) )
        m_data->engine->setPredictor( predictor );
}

QskTextPredictor* QskInputContext::registeredPredictor( const QLocale& locale )
{
    return m_data->predictorTable.find( locale );
}

void QskInputContext::invokeAction( QInputMethod::Action, int )
{
}

void QskInputContext::reset()
{
}

void QskInputContext::commit()
{
    /*
        commit is called, when the input item loses the focus.
        As it it should be possible to navigate inside of the
        inputPanel this is no valid reason to hide the panel.
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
                if ( qskInputPanel )
                    emitKeyboardRectChanged();

                break;
            }
            case QEvent::Resize:
            {
                if ( qskInputPanel )
                    qskInputPanel->setSize( m_data->inputWindow->size() );

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
                emitKeyboardRectChanged();
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

bool QskInputContext::filterEvent( const QEvent* )
{
    // called from QXcbKeyboard, but what about other platforms
    return false;
}

#include "moc_QskInputContext.cpp"
