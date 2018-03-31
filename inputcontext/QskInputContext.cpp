/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskInputContext.h"

#include "QskInputCompositionModel.h"
#include "QskPinyinCompositionModel.h"

#include <QskVirtualKeyboard.h>
#include <QskDialog.h>
#include <QskWindow.h>
#include <QskSetup.h>

#include <QGuiApplication>

QskInputContext::QskInputContext():
    Inherited(),
    m_defaultInputCompositionModel( new QskInputCompositionModel )
{
    connect( qskSetup, &QskSetup::inputPanelChanged,
        this, &QskInputContext::setInputPanel );
    setInputPanel( qskSetup->inputPanel() );

    QskPinyinCompositionModel* pinyinModel = new QskPinyinCompositionModel;
    // For input methods outside skinny, call QskInputPanel::registerCompositionModelForLocale()
    inputMethodRegistered( QLocale::Chinese, pinyinModel );

    // We could connect candidatesChanged() here, but we don't emit
    // the signal in the normal composition model anyhow
}

QskInputContext::~QskInputContext()
{
    if ( m_inputPanel )
        delete m_inputPanel;

    for( int a = 0; a < m_inputModels.values().count(); a++ )
    {
        delete m_inputModels.values()[a];
    }
}

bool QskInputContext::isValid() const
{
    return true;
}

void QskInputContext::update( Qt::InputMethodQueries queries )
{
    if ( !m_inputItem )
        return;

    QInputMethodQueryEvent queryEvent( queries );
    if ( !QCoreApplication::sendEvent( m_inputItem, &queryEvent ) )
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
        Q_UNUSED( hints );
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

        auto oldModel = currentInputCompositionModel();

        if( m_inputPanel )
            m_inputPanel->setLocale( locale );

        auto newModel = currentInputCompositionModel();

        bool modelChanged = ( oldModel != newModel );

        if( modelChanged )
        {
            if( m_inputPanel )
            {
                m_inputPanel->setCandidateBarVisible( newModel->supportsSuggestions() );
                m_inputPanel->disconnect( oldModel );
                QObject::connect(
                    newModel, &QskInputCompositionModel::groupsChanged,
                    m_inputPanel.data(), &QskVirtualKeyboard::setPreeditGroups );
                QObject::connect(
                    newModel, &QskInputCompositionModel::candidatesChanged,
                    this, &QskInputContext::handleCandidatesChanged );
            }
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
        return m_inputPanel->geometry();
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
        setInputPanel( new QskVirtualKeyboard );

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
    m_focusObject = focusObject;
    if ( !m_focusObject )
    {
        m_inputItem = nullptr;
        currentInputCompositionModel()->setInputItem( nullptr );
        return;
    }

    bool inputItemChanged = false;

    auto focusQuickItem = qobject_cast< QQuickItem* >( focusObject );
    if( focusQuickItem )
    {
        // Do not change the input item when panel buttons get the focus:
        if( qskNearestFocusScope( focusQuickItem ) != m_inputPanel )
        {
            m_inputItem = focusQuickItem;
            currentInputCompositionModel()->setInputItem( m_inputItem ); // ### use a signal/slot connection
            inputItemChanged = true;
        }
    }

    if( inputItemChanged )
    {
        QInputMethodQueryEvent queryEvent( Qt::ImEnabled );
        if ( !QCoreApplication::sendEvent( m_inputItem, &queryEvent ) )
            return;

        if ( !queryEvent.value( Qt::ImEnabled ).toBool() )
        {
            hideInputPanel();
            return;
        }
    }

    update( Qt::InputMethodQuery( Qt::ImQueryAll & ~Qt::ImEnabled ) );
}

void QskInputContext::inputMethodRegistered( const QLocale& locale, QskInputCompositionModel* model )
{
    auto oldModel = m_inputModels.value( locale, nullptr );

    if( oldModel != nullptr )
    {
        oldModel->deleteLater();
    }

    m_inputModels.insert( locale, model );
}

QskInputCompositionModel* QskInputContext::compositionModelForLocale( const QLocale& locale ) const
{
    return m_inputModels.value( locale, m_defaultInputCompositionModel );
}

QskInputCompositionModel* QskInputContext::currentInputCompositionModel() const
{
    return m_inputModels.value( locale(), m_defaultInputCompositionModel );
}

void QskInputContext::resetCandidates()
{
    m_inputPanel->setPreeditCandidates( {} );
}

void QskInputContext::invokeAction( QInputMethod::Action action, int cursorPosition )
{
    Q_UNUSED( cursorPosition );

    if ( !m_inputPanel )
        return;

    switch ( static_cast< QskVirtualKeyboard::Action >( action ) )
    {
        case QskVirtualKeyboard::Compose:
            currentInputCompositionModel()->composeKey( static_cast< Qt::Key >( cursorPosition ) );
            break;

        case QskVirtualKeyboard::SelectGroup:
            currentInputCompositionModel()->setGroupIndex( cursorPosition );
            break;

        case QskVirtualKeyboard::SelectCandidate:
            currentInputCompositionModel()->commitCandidate( cursorPosition );
            resetCandidates();
            break;
    }
}

void QskInputContext::emitAnimatingChanged()
{
    QPlatformInputContext::emitAnimatingChanged();
}

void QskInputContext::handleCandidatesChanged()
{
    QVector< QString > candidates( currentInputCompositionModel()->candidateCount() );

    for( int i = 0; i < candidates.length(); ++i )
    {
        candidates[i] = currentInputCompositionModel()->candidate( i );
    }

    m_inputPanel->setPreeditCandidates( candidates );
}

void QskInputContext::setInputPanel( QskVirtualKeyboard* inputPanel )
{
    if ( m_inputPanel == inputPanel )
        return;

    auto compositionModel = currentInputCompositionModel();

    if ( m_inputPanel )
    {
        m_inputPanel->disconnect( this );

        if ( compositionModel )
            compositionModel->disconnect( m_inputPanel );
    }

    m_inputPanel = inputPanel;

    if ( inputPanel )
    {
        connect( inputPanel, &QskVirtualKeyboard::visibleChanged,
            this, &QPlatformInputContext::emitInputPanelVisibleChanged );

        connect( inputPanel, &QskVirtualKeyboard::keyboardRectChanged,
            this, &QPlatformInputContext::emitKeyboardRectChanged );

        connect( inputPanel, &QskVirtualKeyboard::localeChanged,
            this, &QPlatformInputContext::emitLocaleChanged );

        if ( compositionModel )
        {
            inputPanel->setCandidateBarVisible(
                compositionModel->supportsSuggestions() );

            connect( compositionModel, &QskInputCompositionModel::groupsChanged,
                inputPanel, &QskVirtualKeyboard::setPreeditGroups );
        }
    }
}

#include "moc_QskInputContext.cpp"
