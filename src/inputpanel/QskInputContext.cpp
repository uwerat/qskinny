/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskInputContext.h"
#include "QskTextPredictor.h"
#include "QskInputPanel.h"
#include "QskInputEngine.h"

#include "QskLinearBox.h"
#include <QskDialog.h>
#include <QskPopup.h>
#include <QskWindow.h>
#include <QskSetup.h>
#include <QskEvent.h>

#include <QHash>
#include <QPointer>
#include <QGuiApplication>

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

    // item, wher the user enters texts/keys
    QPointer< QQuickItem > inputPanel;

    // popup or window embedding the inputPanel
    QskPopup* inputPopup = nullptr;
    QskWindow* inputWindow = nullptr;

    PredictorTable predictorTable;

    QskInputEngine* engine = nullptr;

    // the input panel is embedded in a window
    bool ownsInputPanelWindow : 1;
};

QskInputContext::QskInputContext():
    m_data( new PrivateData() )
{
    setObjectName( "InputContext" );

    m_data->engine = new QskInputEngine( this );

    connect( qskSetup, &QskSetup::inputPanelChanged,
        this, &QskInputContext::setInputPanel );

    setInputPanel( qskSetup->inputPanel() );
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

void QskInputContext::setInputItem( QQuickItem* item )
{
    if ( m_data->inputItem == item )
        return;

    auto panel = qobject_cast< QskInputPanel* >( m_data->inputPanel );

    if ( isInputPanelVisible() )
    {
        if ( item == nullptr )
        {
            hideInputPanel();
        }
        else
        {
            if ( panel )
                panel->attachInputItem( item );

            update( Qt::ImQueryAll );
        }
    }
    else
    {
        // no need for updates
        if ( panel )
            panel->attachInputItem( nullptr );
    }

    m_data->inputItem = item;
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

    if ( auto panel = qobject_cast< QskInputPanel* >( m_data->inputPanel ) )
        panel->processInputMethodQueries( queries );
}

QRectF QskInputContext::keyboardRect() const
{
    if ( m_data->inputPanel
         && QskDialog::instance()->policy() != QskDialog::TopLevelWindow )
    {
        return qskItemGeometry( m_data->inputPanel );
    }

    return Inherited::keyboardRect();
}

bool QskInputContext::isAnimating() const
{
    return false;
}

void QskInputContext::showInputPanel()
{
    auto& inputPanel = m_data->inputPanel;
    auto& inputPopup = m_data->inputPopup;
    auto& inputWindow = m_data->inputWindow;

    if ( inputPanel == nullptr )
    {
        auto panel = new QskInputPanel();

        panel->setParent( this );
        panel->setInputProxy( true );

        setInputPanel( panel );
    }

    const bool isPopupPanel = qobject_cast< QskPopup* >( inputPanel );

    bool useWindow = false;
    if ( !isPopupPanel )
    {
        useWindow = ( QskDialog::instance()->policy() == QskDialog::TopLevelWindow );
    }

    if ( useWindow )
    {
        delete inputPopup;

        if ( inputWindow == nullptr )
        {
            inputWindow = new QskWindow();
            inputWindow->setDeleteOnClose( true );
            inputWindow->setFlags( Qt::Tool | Qt::WindowDoesNotAcceptFocus );

            inputPanel->setParentItem( inputWindow->contentItem() );

            QSizeF size;
            if ( auto control = qobject_cast< const QskControl* >( inputPanel ) )
                size = control->sizeHint();

            if ( size.isEmpty() )
                size = QSizeF( 800, 240 ); // ### what size?

            inputWindow->resize( size.toSize() );
            inputWindow->show();

            inputWindow->installEventFilter( this );
        }
    }
    else
    {
        delete inputWindow;

        if ( inputPopup == nullptr )
        {
            if ( isPopupPanel )
            {
                inputPopup = qobject_cast< QskPopup* >( inputPanel );
            }
            else
            {
                auto popup = new QskPopup( m_data->inputItem->window()->contentItem() );

                popup->setAutoLayoutChildren( true );
                popup->setTransparentForPositioner( false );
                popup->setOverlay( false );
                popup->setModal( true );

                auto box = new QskLinearBox( popup );
                box->addItem( inputPanel );

                if ( auto panel = qobject_cast< QskInputPanel* >( inputPanel ) )
                {
                    if ( panel->hasInputProxy() )
                    {
                        popup->setOverlay( true );
                    }
                }

                if ( !popup->hasOverlay() )
                {
                    box->setExtraSpacingAt( Qt::TopEdge | Qt::LeftEdge | Qt::RightEdge );
                }

                inputPopup = popup;
            }

            inputPopup->installEventFilter( this );
        }

        if ( inputPopup->window() == nullptr )
        {
            QQuickWindow* window = nullptr;
            if ( m_data->inputItem )
                window = m_data->inputItem->window();
            else
                window = qobject_cast< QQuickWindow* >( QGuiApplication::focusWindow() );

            if ( window )
            {
                inputPopup->setParentItem( window->contentItem() );
            }
        }

        inputPopup->setVisible( true );
    }

    update( Qt::ImQueryAll );

#if 1
    if ( auto panel = qobject_cast< QskInputPanel* >( m_data->inputPanel ) )
        panel->updateInputProxy( m_data->inputItem );
#endif

    inputPanel->setVisible( true );

#if 0
    if ( auto focusItem = inputPanel->nextItemInFocusChain( true ) )
        qskForceActiveFocus( focusItem, Qt::OtherFocusReason );
#endif

    connect( inputPanel->window(), &QskWindow::visibleChanged,
        this, &QskInputContext::emitInputPanelVisibleChanged );

    updateInputPanel( m_data->inputItem );

    m_data->engine->setPredictor(
        m_data->predictorTable.find( locale() ) );
}

void QskInputContext::hideInputPanel()
{
    if ( m_data->inputPanel )
    {
        // to get rid of the scene graph nodes
        m_data->inputPanel->setVisible( false );
        if ( auto panel = qobject_cast< QskInputPanel* >( m_data->inputPanel ) )
            panel->setEngine( nullptr );
    }

    if ( m_data->inputPopup == m_data->inputPanel )
    {
        m_data->inputPopup->removeEventFilter( this );
        m_data->inputPopup = nullptr;
    }
    else
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

            m_data->inputPopup->deleteLater();
        }
    }

    QskWindow* window = m_data->inputWindow;
    m_data->inputWindow = nullptr;

    if ( window )
    {
        window->removeEventFilter( this );
        window->close(); // deleteOnClose is set
    }

    qGuiApp->removeEventFilter( this );

    updateInputPanel( nullptr );
}

void QskInputContext::updateInputPanel( QQuickItem* inputItem )
{
    auto panel = qobject_cast< QskInputPanel* >( m_data->inputPanel );
    if ( panel == nullptr )
        return;

    panel->setLocale( locale() );
    panel->attachInputItem( inputItem );

    panel->setEngine( inputItem ? m_data->engine : nullptr );
}

bool QskInputContext::isInputPanelVisible() const
{
    auto panel = m_data->inputPanel;

    return panel && panel->isVisible()
        && panel->window() && panel->window()->isVisible();
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
    auto focusItem = qobject_cast< QQuickItem* >( focusObject );

    if ( focusItem == nullptr )
    {
        if ( m_data->inputItem )
        {
            if ( m_data->inputItem->window() == QGuiApplication::focusWindow() )
                setInputItem( nullptr );
        }
    }
    else
    {
        /*
            Do not change the input item when
            navigating to or inside the input popup/window
         */

        bool isAccepted = ( m_data->inputItem == nullptr );

        if ( !isAccepted )
        {
            if ( m_data->inputWindow )
            {
                if ( focusItem->window() != m_data->inputWindow )
                    isAccepted = true;
            }
            else if ( m_data->inputPopup )
            {
                if ( ( focusItem != m_data->inputPopup )
                    && !qskIsAncestorOf( m_data->inputPopup, focusItem ) )
                {
                    isAccepted = true;
                }
            }
            else
            {
                isAccepted = true;
            }
        }

        if ( isAccepted )
            setInputItem( focusItem );
    }
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

void QskInputContext::setInputPanel( QQuickItem* inputPanel )
{
    if ( m_data->inputPanel == inputPanel )
        return;

    if ( m_data->inputPanel )
    {
        m_data->inputPanel->disconnect( this );

        if ( m_data->inputPanel->parent() == this )
        {
            delete m_data->inputPanel;
        }
        else
        {
            m_data->inputPanel->setParentItem( nullptr );
        }
    }

    m_data->inputPanel = inputPanel;
    m_data->ownsInputPanelWindow = false;

    if ( inputPanel )
    {
        if ( inputPanel->parent() == nullptr )
            inputPanel->setParent( this );

        connect( inputPanel, &QQuickItem::visibleChanged,
            this, &QPlatformInputContext::emitInputPanelVisibleChanged );

        if ( auto control = qobject_cast< QskControl* >( inputPanel ) )
        {
            connect( control, &QskControl::localeChanged,
                this, &QPlatformInputContext::emitLocaleChanged );
        }
    }
}

void QskInputContext::reset()
{
}

void QskInputContext::commit()
{
    // called on focus changes
}

bool QskInputContext::eventFilter( QObject* object, QEvent* event )
{
    if ( object == m_data->inputWindow )
    {
        switch( event->type() )
        {
            case QEvent::Move:
            {
                if ( m_data->inputPanel )
                    emitKeyboardRectChanged();

                break;
            }
            case QEvent::Resize:
            {
                if ( m_data->inputPanel )
                    m_data->inputPanel->setSize( m_data->inputWindow->size() );

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
