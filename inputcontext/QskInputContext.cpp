/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskInputContext.h"

#include "QskInputCompositionModel.h"
#include "QskPinyinCompositionModel.h"
#include "QskHunspellCompositionModel.h"

#include "QskInputPanel.h"
#include "QskLinearBox.h"
#include <QskDialog.h>
#include <QskPopup.h>
#include <QskWindow.h>
#include <QskControl.h>
#include <QskSetup.h>
#include <QskEvent.h>

#include <QTextCharFormat>
#include <QHash>
#include <QPointer>
#include <QGuiApplication>

static inline QString qskKeyString( int keyCode )
{
    // Special case entry codes here, else default to the symbol
    switch ( keyCode )
    {
        case Qt::Key_Shift:
        case Qt::Key_CapsLock:
        case Qt::Key_Mode_switch:
        case Qt::Key_Backspace:
        case Qt::Key_Muhenkan:
            return QString();

        case Qt::Key_Return:
        case Qt::Key_Kanji:
            return QChar( QChar::CarriageReturn );

        case Qt::Key_Space:
            return QChar( QChar::Space );

        default:
            break;
    }

    return QChar( keyCode );
}

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

static inline uint qskHashLocale( const QLocale& locale )
{
    return uint( locale.language() + uint( locale.country() ) << 16 );
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

    QHash< uint, QskInputCompositionModel* > compositionModels;

    QString preedit;

    // the input panel is embedded in a window
    bool ownsInputPanelWindow : 1;
};

QskInputContext::QskInputContext():
    m_data( new PrivateData() )
{
    setObjectName( "InputContext" );

#if 1
    setCompositionModel( locale(), new QskHunspellCompositionModel( this ) );
#endif
#if 0
    setCompositionModel( QLocale::Chinese, new QskPinyinCompositionModel( this ) );
#endif

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
            ImhNoPredictiveText = 0x40,   // not use predictive text 

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

        const auto oldModel = compositionModel();

        if( m_data->inputPanel )
            qskSetLocale( m_data->inputPanel, locale );

        auto newModel = compositionModel();

        if( newModel && ( oldModel != newModel ) )
        {
            connect( newModel, &QskInputCompositionModel::candidatesChanged,
                this, &QskInputContext::handleCandidatesChanged );
        }

        qskSetCandidatesEnabled( m_data->inputPanel, newModel != nullptr );
    }

#if 0
    if ( queryEvent.queries() & Qt::ImTextBeforeCursor 
        && queryEvent.queries() & Qt::ImTextAfterCursor )
    {
        const auto text1 = queryEvent.value( Qt::ImTextBeforeCursor ).toString();
        const auto text2 = queryEvent.value( Qt::ImTextAfterCursor ).toString();

        qDebug() << text1 << text2;
    }
#endif

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

    m_data->preedit.clear();
    if ( auto model = compositionModel() )
        model->resetCandidates();
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
            else
            {
                isAccepted = true;
            }
        }

        if ( isAccepted )
            setInputItem( focusItem );
    }
}

void QskInputContext::setCompositionModel(
    const QLocale& locale, QskInputCompositionModel* model )
{
    auto& models = m_data->compositionModels;

    const auto key = qskHashLocale( locale );

    if ( model )
    {
        const auto it = models.find( key );
        if ( it != models.end() )
        {
            if ( it.value() == model )
                return;

            delete it.value();
            *it = model;
        }
        else
        {
            models.insert( key, model );
        }

        connect( model, &QskInputCompositionModel::candidatesChanged,
            this, &QskInputContext::handleCandidatesChanged );
    }
    else
    {
        const auto it = models.find( key );
        if ( it != models.end() )
        {
            delete it.value();
            models.erase( it );
        }
    }
}

QskInputCompositionModel* QskInputContext::compositionModel() const
{
    const auto key = qskHashLocale( locale() );
    return m_data->compositionModels.value( key, nullptr );
}

void QskInputContext::invokeAction( QInputMethod::Action action, int value )
{
    switch ( static_cast< int >( action ) )
    {
        case QskInputPanel::Compose:
        {
            processKey( value );
            break;
        }
        case QskInputPanel::SelectCandidate:
        {
            if ( auto model = compositionModel() )
            {
                auto text = model->candidate( value );

                if ( model->attributes() & QskInputCompositionModel::Words )
                    text += " ";

                sendText( text, true );

                m_data->preedit.clear();
                model->resetCandidates();
            }

            break;
        }
        case QInputMethod::Click:
        case QInputMethod::ContextMenu:
        {
            break;
        }
    }
}

int QskInputContext::keysLeft() const
{
    const auto event = queryInputMethod(
        Qt::ImSurroundingText | Qt::ImMaximumTextLength | Qt::ImHints );

    const auto hints = static_cast< Qt::InputMethodHints >(
            event.value( Qt::ImHints ).toInt() );

    if ( !( hints & Qt::ImhMultiLine ) )
    {
        const int max = event.value( Qt::ImMaximumTextLength ).toInt();

        if ( max > 0 )
        {
            const auto text = event.value( Qt::ImSurroundingText ).toString();
            return max - text.length();
        }
    }

    return -1; // unlimited
}

Qt::InputMethodHints QskInputContext::inputHints() const
{
    const auto e = queryInputMethod( Qt::ImHints );
    return static_cast< Qt::InputMethodHints >( e.value( Qt::ImHints ).toInt() );
}

void QskInputContext::processKey( int key )
{
    const auto hints = inputHints();

    auto spaceLeft = keysLeft();

    QskInputCompositionModel* model = nullptr;

    if ( !( hints & Qt::ImhHiddenText ) )
        model = compositionModel();

    auto& preedit = m_data->preedit;
    /*
        First we have to handle the control keys
     */
    switch ( key )
    {
        case Qt::Key_Backspace:
        case Qt::Key_Muhenkan:
        {
            if ( model )
            {
                if ( !preedit.isEmpty() )
                {
                    preedit.chop( 1 );
                    sendText( preedit, false );

                    model->requestCandidates( preedit );
                    return;
                }
            }

            sendKey( Qt::Key_Backspace );
            return;
        }
        case Qt::Key_Return:
        {
            if ( model )
            {
                if ( !preedit.isEmpty() )
                {
                    if ( spaceLeft )
                        sendText( preedit.left( spaceLeft ), true );

                    preedit.clear();
                    model->resetCandidates();

                    return;
                }
            }

            if( !( hints & Qt::ImhMultiLine ) )
            {
                sendKey( Qt::Key_Return );
                return;
            }

            break;
        }
        case Qt::Key_Space:
        {
            if ( model )
            {
                if ( !preedit.isEmpty() && spaceLeft)
                {
                    preedit = preedit.left( spaceLeft );
                    sendText( preedit, true );
                    spaceLeft -= preedit.length();

                    preedit.clear();
                    model->resetCandidates();
                }
            }

            break;
        }
        case Qt::Key_Left:
        case Qt::Key_Right:
        case Qt::Key_Escape:
        {
            sendKey( key );
            return;
        }
    }

    const QString text = qskKeyString( key );

    if ( model )
    {
        preedit += text;

        model->requestCandidates( preedit );

        if ( model->candidateCount() > 0 )
        {
            sendText( preedit, false );
        }
        else
        {
            sendText( preedit.left( spaceLeft ), true );
            preedit.clear();
        }
    }
    else
    {
        sendText( text, true );
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

        qskSetCandidatesEnabled( inputPanel, model != nullptr );
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
                QQuickItem* panel = m_data->inputPanel;

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
                if ( event->type() == QskEvent::GeometryChange )
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

QInputMethodQueryEvent QskInputContext::queryInputMethod(
    Qt::InputMethodQueries queries ) const
{
    QInputMethodQueryEvent event( queries );

    if ( m_data->inputItem )
        QCoreApplication::sendEvent( m_data->inputItem, &event );

    return event;
}

void QskInputContext::sendText(
    const QString& text, bool isFinal ) const
{
    if ( m_data->inputItem == nullptr )
        return;

    if ( isFinal )
    {
        QInputMethodEvent event;
        event.setCommitString( text );

        QCoreApplication::sendEvent( m_data->inputItem, &event );
    }
    else
    {
        QTextCharFormat format;
        format.setFontUnderline( true );

        const QInputMethodEvent::Attribute attribute(
            QInputMethodEvent::TextFormat, 0, text.length(), format );

        QInputMethodEvent event( text, { attribute } );

        QCoreApplication::sendEvent( m_data->inputItem, &event );
    }
}


void QskInputContext::sendKey( int key ) const
{
    if ( m_data->inputItem == nullptr )
        return;

    QKeyEvent keyPress( QEvent::KeyPress, key, Qt::NoModifier );
    QCoreApplication::sendEvent( m_data->inputItem, &keyPress );

    QKeyEvent keyRelease( QEvent::KeyRelease, key, Qt::NoModifier );
    QCoreApplication::sendEvent( m_data->inputItem, &keyRelease );
}

#include "moc_QskInputContext.cpp"
