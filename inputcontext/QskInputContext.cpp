#include "QskInputContext.h"

#include "QskInputCompositionModel.h"
#include "QskPinyinCompositionModel.h"

#include <QskInputPanel.h>
#include <QskDialog.h>
#include <QskWindow.h>
#include <QskSetup.h>

#include <private/qguiapplication_p.h>
#include <qpa/qplatformintegration.h>

#include <QGuiApplication>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickWindow>
#include <QRectF>

QskInputContext::QskInputContext():
    Inherited()
{
    connect( qskSetup, &QskSetup::inputPanelChanged,
        this, &QskInputContext::setInputPanel );
    setInputPanel( qskSetup->inputPanel() );
}

QskInputContext::~QskInputContext()
{
    if ( m_inputPanel )
        delete m_inputPanel;
}

bool QskInputContext::isValid() const
{
    return true;
}

void QskInputContext::update( Qt::InputMethodQueries queries )
{
    if ( !m_focusObject )
        return;

    QInputMethodQueryEvent queryEvent( queries );
    if ( !QCoreApplication::sendEvent( m_focusObject, &queryEvent ) )
        return;

    // Qt::ImCursorRectangle
    // Qt::ImFont
    // Qt::ImCursorPosition
    // Qt::ImSurroundingText // important for chinese input
    // Qt::ImCurrentSelection // important for prediction
    // Qt::ImMaximumTextLength // should be monitored
    // Qt::ImAnchorPosition

    if ( queries & Qt::ImHints )
    {
        const Qt::InputMethodHints hints =
            static_cast< Qt::InputMethodHints >( queryEvent.value( Qt::ImHints ).toInt() );
        Q_UNUSED(hints);
        //ImhHiddenText = 0x1,          // might need to disable certain checks
        //ImhSensitiveData = 0x2,       // shouldn't change anything
        //ImhNoAutoUppercase = 0x4,     // if we support auto uppercase, disable it
        //ImhPreferNumbers = 0x8,       // default to number keyboard
        //ImhPreferUppercase = 0x10,    // start with shift on
        //ImhPreferLowercase = 0x20,    // start with shift off
        //ImhNoPredictiveText = 0x40,   // ignored for now

        //ImhDate = 0x80,               // ignored for now (no date keyboard)
        //ImhTime = 0x100,              // ignored for know (no time keyboard)

        //ImhPreferLatin = 0x200,       // can be used to launch chinese kb in english mode

        //ImhMultiLine = 0x400,         // not useful?

        //ImhDigitsOnly                 // default to number keyboard, disable other keys
        //ImhFormattedNumbersOnly       // hard to say
        //ImhUppercaseOnly              // caps-lock, disable shift
        //ImhLowercaseOnly              // disable shift
        //ImhDialableCharactersOnly     // dial pad (calculator?)
        //ImhEmailCharactersOnly        // disable certain symbols (email-only kb?)
        //ImhUrlCharactersOnly          // disable certain symbols (url-only kb?)
        //ImhLatinOnly                  // disable chinese input
    }

    if ( queries & Qt::ImPreferredLanguage )
    {
        const auto locale = queryEvent.value( Qt::ImPreferredLanguage ).toLocale();
        if ( m_inputPanel )
            m_inputPanel->setLocale( locale );

        auto modelChanged = false;
        const bool currentModelIsPinyin =
            dynamic_cast< QskPinyinCompositionModel* >( m_inputCompositionModel.get() );
        const bool localeIsChinese = locale.language() == QLocale::Chinese;
        if ( localeIsChinese && !currentModelIsPinyin )
        {
            if ( m_inputPanel )
                m_inputPanel->disconnect( m_inputCompositionModel.get() );

            m_inputCompositionModel.reset( new QskPinyinCompositionModel );
            modelChanged = true;
        }
        else if ( !m_inputCompositionModel || ( !localeIsChinese && currentModelIsPinyin ) )  // Install default
        {
            if ( m_inputPanel )
                m_inputPanel->disconnect( m_inputCompositionModel.get() );

            m_inputCompositionModel.reset( new QskInputCompositionModel );
            modelChanged = true;
        }

        if ( modelChanged && m_inputPanel )
        {
            QObject::connect(
                m_inputCompositionModel.get(), &QskInputCompositionModel::groupsChanged,
                m_inputPanel.data(), &QskInputPanel::setPreeditGroups );
            QObject::connect(
                m_inputCompositionModel.get(), &QskInputCompositionModel::candidatesChanged,
                this, &QskInputContext::handleCandidatesChanged );
        }
    }

    // Qt::ImAbsolutePosition
    // Qt::ImTextBeforeCursor // important for chinese
    // Qt::ImTextAfterCursor  // important for chinese
    // Qt::ImPlatformData     // hard to say...
}

QRectF QskInputContext::keyboardRect() const
{
    if ( m_inputPanel
         && QskDialog::instance()->policy() != QskDialog::TopLevelWindow )
    {
        return m_inputPanel->keyboardRect();
    }

    return Inherited::keyboardRect();
}

bool QskInputContext::isAnimating() const
{
    return false;
}

void QskInputContext::showInputPanel()
{
    if ( !m_inputPanel )
    {
        setInputPanel( new QskInputPanel );

        if ( QskDialog::instance()->policy() == QskDialog::TopLevelWindow )
        {
            auto window = new QskWindow;
            window->setFlags( Qt::Tool | Qt::WindowDoesNotAcceptFocus );
            window->resize( 800, 240 ); // ### what size?
            m_inputPanel->setParentItem( window->contentItem() );
            connect( window, &QskWindow::visibleChanged,
                this, &QskInputContext::emitInputPanelVisibleChanged );
        }
        else
        {
            auto window = qobject_cast< QQuickWindow* >( QGuiApplication::focusWindow() );
            if ( window )
            {
                m_inputPanel->setParentItem( window->contentItem() );
                m_inputPanel->setSize( window->size() );
            }
        }
    }

    auto window = m_inputPanel->window();
    if ( window && window != QGuiApplication::focusWindow() )
        window->show();
    else
        m_inputPanel->setVisible( true );
}

void QskInputContext::hideInputPanel()
{
    if ( !m_inputPanel )
        return;

    auto window = m_inputPanel->window();
    if ( window && window != QGuiApplication::focusWindow() )
        window->hide();
    else
        m_inputPanel->setVisible( false );
}

bool QskInputContext::isInputPanelVisible() const
{
    return m_inputPanel && m_inputPanel->isVisible()
           && m_inputPanel->window() && m_inputPanel->window()->isVisible();
}

QLocale QskInputContext::locale() const
{
    return m_inputPanel ? m_inputPanel->locale() : QLocale();
}

void QskInputContext::setFocusObject( QObject* focusObject )
{
    if ( m_focusObject )
        m_focusObject->removeEventFilter( this );

    m_focusObject = focusObject;
    if ( !m_focusObject )
        return;

    QInputMethodQueryEvent queryEvent( Qt::ImEnabled );
    if ( !QCoreApplication::sendEvent( m_focusObject, &queryEvent ) )
        return;

    if ( !queryEvent.value( Qt::ImEnabled ).toBool() )
    {
        hideInputPanel();
        return;
    }

    m_focusObject->installEventFilter( this );
    update( Qt::InputMethodQuery( Qt::ImQueryAll & ~Qt::ImEnabled ) );
}

bool QskInputContext::eventFilter( QObject* object, QEvent* event )
{
    if ( m_inputPanel && event->type() == QEvent::KeyPress )
    {
        auto keyEvent = static_cast< QKeyEvent* >( event );
        const auto key = keyEvent->key();
        switch ( key )
        {
            case Qt::Key_Tab:
            case Qt::Key_Backtab:
                if ( m_inputPanel->advanceFocus( key == Qt::Key_Tab ) )
                {
                    keyEvent->accept();
                    return true;
                }
                break;
            case Qt::Key_Select:
            case Qt::Key_Space:
                // if there is a focused key, treat the key like a push button
                if ( m_inputPanel->activateFocusKey() )
                {
                    keyEvent->accept();
                    return true;
                }
                break;
        }
    }

    if ( m_inputPanel && event->type() == QEvent::KeyRelease )
    {
        auto keyEvent = static_cast< QKeyEvent* >( event );
        const auto key = keyEvent->key();
        switch ( key )
        {
            case Qt::Key_Select:
            case Qt::Key_Space:
                if ( m_inputPanel->deactivateFocusKey() )
                {
                    keyEvent->accept();
                    return true;
                }
                break;
        }
    }

    return Inherited::eventFilter( object, event );
}

void QskInputContext::invokeAction( QInputMethod::Action action, int cursorPosition )
{
    Q_UNUSED(cursorPosition);

    if ( !m_inputPanel )
        return;

    switch ( static_cast< QskInputPanel::Action >( action ) )
    {
        case QskInputPanel::Compose:
            m_inputCompositionModel->composeKey( static_cast< Qt::Key >( cursorPosition ) );
            break;
        case QskInputPanel::SelectGroup:
            m_inputCompositionModel->setGroupIndex( cursorPosition );
            break;
        case QskInputPanel::SelectCandidate:
            m_inputCompositionModel->commitCandidate( cursorPosition );
            break;
    }
}

void QskInputContext::emitAnimatingChanged()
{
    QPlatformInputContext::emitAnimatingChanged();
}

void QskInputContext::handleCandidatesChanged()
{
    QVector< Qt::Key > candidates( m_inputCompositionModel->candidateCount() );
    for ( int i = 0; i < candidates.length(); ++i )
    {
        candidates[i] = static_cast< Qt::Key >( m_inputCompositionModel->candidate( i ) );
    }

    m_inputPanel->setPreeditCandidates( candidates );
}

void QskInputContext::setInputPanel( QskInputPanel* inputPanel )
{
    if ( m_inputPanel == inputPanel )
        return;

    if ( m_inputPanel )
    {
        QObject::disconnect( m_inputPanel.data(), &QskInputPanel::visibleChanged,
            this, &QskInputContext::emitInputPanelVisibleChanged );
        QObject::disconnect( m_inputPanel.data(), &QskInputPanel::keyboardRectChanged,
            this, &QskInputContext::emitKeyboardRectChanged );
        QObject::disconnect( m_inputPanel.data(), &QskInputPanel::localeChanged,
            this, &QskInputContext::emitLocaleChanged );
        if ( m_inputCompositionModel )
            m_inputPanel->disconnect( m_inputCompositionModel.get() );
    }

    m_inputPanel = inputPanel;
    if ( !m_inputPanel )
        return;

    QObject::connect( m_inputPanel.data(), &QskInputPanel::visibleChanged,
        this, &QskInputContext::emitInputPanelVisibleChanged );
    QObject::connect( m_inputPanel.data(), &QskInputPanel::keyboardRectChanged,
        this, &QskInputContext::emitKeyboardRectChanged );
    QObject::connect( m_inputPanel.data(), &QskInputPanel::localeChanged,
        this, &QskInputContext::emitLocaleChanged );
    if ( m_inputCompositionModel )
    {
        QObject::connect(
            m_inputCompositionModel.get(), &QskInputCompositionModel::groupsChanged,
            m_inputPanel.data(), &QskInputPanel::setPreeditGroups );
    }
}

#include "moc_QskInputContext.cpp"
