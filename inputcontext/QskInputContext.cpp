/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskInputContext.h"

#include "QskInputCompositionModel.h"
#include "QskPinyinCompositionModel.h"
#include "QskHunspellCompositionModel.h"

#include "QskInputPanel.h"
#include <QskDialog.h>
#include <QskPopup.h>
#include <QskWindow.h>
#include <QskControl.h>
#include <QskSetup.h>
#include <QskEvent.h>

#include <QGuiApplication>
#include <QHash>
#include <QPointer>

static void qskSetLocale( QQuickItem* inputPanel, const QLocale& locale )
{
    if ( auto control = qobject_cast< QskControl* >( inputPanel ) )
    {
        control->setLocale( locale );
    }
    else
    {
        const auto mo = inputPanel->metaObject();

        const auto property = mo->property( mo->indexOfProperty( "locale" ) );
        if ( property.isWritable() )
            property.write( inputPanel, locale );
    }
}

static QLocale qskLocale( const QQuickItem* inputPanel )
{
    if ( inputPanel == nullptr )
        return QLocale();

    if ( auto control = qobject_cast< const QskControl* >( inputPanel ) )
        return control->locale();

    return inputPanel->property( "locale" ).toLocale();
}

static void qskSetCandidatesEnabled( QQuickItem* inputPanel, bool on )
{
    if ( inputPanel == nullptr )
        return;

    if ( auto panel = qobject_cast< QskInputPanel* >( inputPanel ) )
    {
        panel->setCandidatesEnabled( on );
    }
    else
    {
        QMetaObject::invokeMethod( inputPanel, "setCandidatesEnabled",
            Qt::DirectConnection, Q_ARG( bool, on ) );
    }
}

static void qskSetCandidates( QQuickItem* inputPanel,
    const QVector< QString >& candidates )
{
    if ( inputPanel == nullptr )
        return;

    if ( auto panel = qobject_cast< QskInputPanel* >( inputPanel ) )
    {
        panel->setCandidates( candidates );
    }
    else
    {
        QMetaObject::invokeMethod( inputPanel, "setCandidates",
            Qt::DirectConnection, Q_ARG( QVector< QString >, candidates ) );
    }
}

class QskInputContext::PrivateData
{
public:
    // item receiving the input
    QPointer< QQuickItem > inputItem;

    // item, wher the user enters texts/keys
    QPointer< QQuickItem > inputPanel; 

    // popup or window embedding the inputPanel
    QPointer< QskPopup > inputPopup;
    QPointer< QskWindow > inputWindow;

    QskInputCompositionModel* compositionModel;
    QHash< QLocale, QskInputCompositionModel* > compositionModels;

    // the input panel is embedded in a window
    bool ownsInputPanelWindow : 1;
};

QskInputContext::QskInputContext():
    m_data( new PrivateData() )
{
    setObjectName( "InputContext" );

#if 1
    m_data->compositionModel = new QskInputCompositionModel( this );
#else
    m_data->compositionModel = new QskHunspellCompositionModel( this );
#endif

    connect( m_data->compositionModel, &QskInputCompositionModel::candidatesChanged,
        this, &QskInputContext::handleCandidatesChanged );

    connect( qskSetup, &QskSetup::inputPanelChanged,
        this, &QskInputContext::setInputPanel );

#if 1
    setCompositionModel( QLocale::Chinese, new QskPinyinCompositionModel( this ) );
#endif

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

    m_data->inputItem = item;

    if ( item )
        update( Qt::ImQueryAll );
    else
        hideInputPanel();
}

void QskInputContext::update( Qt::InputMethodQueries queries )
{
    if ( m_data->inputItem == nullptr || m_data->inputPanel == nullptr )
        return;

    const auto queryEvent = queryInputMethod( queries );

    if ( queryEvent.queries() & Qt::ImEnabled )
    {
        if ( !queryEvent.value( Qt::ImEnabled ).toBool() )
        {
            hideInputPanel();
            return;
        }
    }

    if ( queryEvent.queries() & Qt::ImHints )
    {
        /*
            ImhHiddenText = 0x1,          // might need to disable certain checks
            ImhSensitiveData = 0x2,       // shouldn't change anything
            ImhNoAutoUppercase = 0x4,     // if we support auto uppercase, disable it
            ImhPreferNumbers = 0x8,       // default to number keyboard
            ImhPreferUppercase = 0x10,    // start with shift on
            ImhPreferLowercase = 0x20,    // start with shift off
            ImhNoPredictiveText = 0x40,   // ignored for now

            ImhDate = 0x80,               // ignored for now (no date keyboard)
            ImhTime = 0x100,              // ignored for know (no time keyboard)

            ImhPreferLatin = 0x200,       // can be used to launch chinese kb in english mode

            ImhMultiLine = 0x400,         // not useful?

            ImhDigitsOnly                 // default to number keyboard, disable other keys
            ImhFormattedNumbersOnly       // hard to say
            ImhUppercaseOnly              // caps-lock, disable shift
            ImhLowercaseOnly              // disable shift
            ImhDialableCharactersOnly     // dial pad (calculator?)
            ImhEmailCharactersOnly        // disable certain symbols (email-only kb?)
            ImhUrlCharactersOnly          // disable certain symbols (url-only kb?)
            ImhLatinOnly                  // disable chinese input
         */

#if 0
        const auto hints = static_cast< Qt::InputMethodHints >(
            queryEvent.value( Qt::ImHints ).toInt() );

#endif
    }

    if ( queryEvent.queries() & Qt::ImPreferredLanguage )
    {
        const auto locale = queryEvent.value( Qt::ImPreferredLanguage ).toLocale();

        auto oldModel = compositionModel();

        if( m_data->inputPanel )
            qskSetLocale( m_data->inputPanel, locale );

        auto newModel = compositionModel();

        if( oldModel != newModel )
        {
            connect( newModel, &QskInputCompositionModel::candidatesChanged,
                this, &QskInputContext::handleCandidatesChanged );

            qskSetCandidatesEnabled( m_data->inputPanel,
                newModel->supportsSuggestions() );
        }
    }

    /*
        Qt::ImMicroFocus
        Qt::ImCursorRectangle
        Qt::ImFont
        Qt::ImCursorPosition
        Qt::ImSurroundingText // important for chinese input
        Qt::ImCurrentSelection // important for prediction
        Qt::ImMaximumTextLength // should be monitored
        Qt::ImAnchorPosition

        Qt::ImAbsolutePosition
        Qt::ImTextBeforeCursor // important for chinese
        Qt::ImTextAfterCursor  // important for chinese
        Qt::ImPlatformData     // hard to say...
        Qt::ImEnterKeyType
        Qt::ImAnchorRectangle
        Qt::ImInputItemClipRectangle // could be used for the geometry of the panel
     */
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

            inputWindow->resize( 800, 240 ); // ### what size?
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
                popup->setModal( true );

                inputPanel->setParentItem( popup );
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

        inputPopup->setGeometry( qskItemGeometry( inputPopup->parentItem() ) );
        inputPopup->setVisible( true );
    }

    inputPanel->setVisible( true );

    connect( inputPanel->window(), &QskWindow::visibleChanged,
        this, &QskInputContext::emitInputPanelVisibleChanged );
}

void QskInputContext::hideInputPanel()
{
    if ( m_data->inputPanel )
    {
        // to get rid of the scene graph nodes
        m_data->inputPanel->setVisible( false );
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
            auto popup = m_data->inputPopup.data();
            m_data->inputPopup = nullptr;

            popup->deleteLater();
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
}

bool QskInputContext::isInputPanelVisible() const
{
    auto panel = m_data->inputPanel;

    return panel && panel->isVisible()
        && panel->window() && panel->window()->isVisible();
}

QLocale QskInputContext::locale() const
{
    return qskLocale( m_data->inputPanel );
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
        }

        if ( isAccepted )
            setInputItem( focusItem );
    }

    if ( m_data->inputPanel && m_data->inputPanel->isVisible() )
    {
        if ( m_data->inputItem && focusItem != m_data->inputItem )
            qGuiApp->installEventFilter( this );
        else
            qGuiApp->removeEventFilter( this );
    }
}

void QskInputContext::setCompositionModel(
    const QLocale& locale, QskInputCompositionModel* model )
{
    auto& models = m_data->compositionModels;

    if ( model )
    {
        const auto it = models.find( locale );
        if ( it != models.end() )
        {
            if ( it.value() == model )
                return;

            delete it.value();
            *it = model;
        }
        else
        {
            models.insert( locale, model );
        }

        connect( model, &QskInputCompositionModel::candidatesChanged,
            this, &QskInputContext::handleCandidatesChanged );
    }
    else
    {
        const auto it = models.find( locale );
        if ( it != models.end() )
        {
            delete it.value();
            models.erase( it );
        }
    }
}

QskInputCompositionModel* QskInputContext::compositionModel() const
{
    return m_data->compositionModels.value( locale(), m_data->compositionModel );
}

void QskInputContext::invokeAction( QInputMethod::Action action, int value )
{
    auto model = compositionModel();

    switch ( static_cast< int >( action ) )
    {
        case QskInputPanel::Compose:
        {
            model->composeKey( static_cast< Qt::Key >( value ) );
            break;
        }
        case QskInputPanel::SelectCandidate:
        {
            model->commitCandidate( value );
            break;
        }
        case QInputMethod::Click:
        case QInputMethod::ContextMenu:
        {
            break;
        }
    }
}

void QskInputContext::handleCandidatesChanged()
{
    const auto model = compositionModel();
    if ( model == nullptr || m_data->inputPanel == nullptr )
        return;

    const auto count = model->candidateCount();

    QVector< QString > candidates;
    candidates.reserve( count );

    for( int i = 0; i < count; i++ )
        candidates += model->candidate( i );

    qskSetCandidates( m_data->inputPanel, candidates );
}

void QskInputContext::setInputPanel( QQuickItem* inputPanel )
{
    if ( m_data->inputPanel == inputPanel )
        return;

    auto model = compositionModel();

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

        if ( model )
            model->disconnect( m_data->inputPanel );
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

        if ( model )
        {
            qskSetCandidatesEnabled( m_data->inputPanel,
                model->supportsSuggestions() );
        }
    }
}

void QskInputContext::reset()
{
}

void QskInputContext::commit()
{
}

bool QskInputContext::eventFilter( QObject* object, QEvent* event )
{
    switch( static_cast< int >( event->type() ) )
    {
        case QEvent::Move:
        case QEvent::Resize:
        {
            if ( m_data->inputPanel && object == m_data->inputPanel->window() )
                emitKeyboardRectChanged();

            break;
        }
        case QskEvent::GeometryChange:
        {
            if ( object == m_data->inputPopup )
                emitKeyboardRectChanged();

            break;
        }
        case QEvent::InputMethodQuery:
        {
            /*
                Qt/Quick expects that the item associated with the input context
                holds the focus. But this does not work, when a virtual
                keyboard is used, where you can navigate and select inside.
                So we have to fix the receiver.
             */

            if ( ( object != m_data->inputItem )
                && qskIsAncestorOf( m_data->inputPanel, m_data->inputItem ) )
            {
                sendEventToInputItem( event );
                return true;
            }

            break;
        }
    }

    return Inherited::eventFilter( object, event );
}

bool QskInputContext::filterEvent( const QEvent* event )
{
    // called from QXcbKeyboard, but what about other platforms
    Q_UNUSED( event )
    return false;
}

QInputMethodQueryEvent QskInputContext::queryInputMethod(
    Qt::InputMethodQueries queries ) const
{
    QInputMethodQueryEvent event( queries );
    sendEventToInputItem( &event );

    return event;
}

void QskInputContext::sendEventToInputItem( QEvent* event ) const
{
    if ( m_data->inputItem && event )
        QCoreApplication::sendEvent( m_data->inputItem, event );
}

#include "moc_QskInputContext.cpp"
