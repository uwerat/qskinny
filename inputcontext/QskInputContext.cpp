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
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickWindow>
#include <QRectF>

QskInputContext::QskInputContext():
    Inherited(),
    m_inputCompositionModel( new QskInputCompositionModel )
{
    connect( qskSetup, &QskSetup::inputPanelChanged,
        this, &QskInputContext::setInputPanel );
    setInputPanel( qskSetup->inputPanel() );

    // We could connect candidatesChanged() here, but we don't emit
    // the signal in the normal composition model anyhow
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
                m_inputPanel.data(), &QskVirtualKeyboard::setPreeditGroups );
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
        m_inputCompositionModel->setInputItem( nullptr );
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
            m_inputCompositionModel->setInputItem( m_inputItem ); // ### use a signal/slot connection
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

void QskInputContext::invokeAction( QInputMethod::Action action, int cursorPosition )
{
    Q_UNUSED(cursorPosition);

    if ( !m_inputPanel )
        return;

    switch ( static_cast< QskVirtualKeyboard::Action >( action ) )
    {
        case QskVirtualKeyboard::Compose:
            m_inputCompositionModel->composeKey( static_cast< Qt::Key >( cursorPosition ) );
            break;
        case QskVirtualKeyboard::SelectGroup:
            m_inputCompositionModel->setGroupIndex( cursorPosition );
            break;
        case QskVirtualKeyboard::SelectCandidate:
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

void QskInputContext::setInputPanel( QskVirtualKeyboard* inputPanel )
{
    if ( m_inputPanel == inputPanel )
        return;

    if ( m_inputPanel )
    {
        QObject::disconnect( m_inputPanel, &QskVirtualKeyboard::visibleChanged,
            this, &QPlatformInputContext::emitInputPanelVisibleChanged );
        QObject::disconnect( m_inputPanel, &QskVirtualKeyboard::keyboardRectChanged,
            this, &QPlatformInputContext::emitKeyboardRectChanged );
        QObject::disconnect( m_inputPanel, &QskVirtualKeyboard::localeChanged,
            this, &QPlatformInputContext::emitLocaleChanged );
        if ( m_inputCompositionModel )
            m_inputPanel->disconnect( m_inputCompositionModel.get() );
    }

    m_inputPanel = inputPanel;
    if ( !m_inputPanel )
        return;

    QObject::connect( m_inputPanel, &QskVirtualKeyboard::visibleChanged,
        this, &QPlatformInputContext::emitInputPanelVisibleChanged );
    QObject::connect( m_inputPanel, &QskVirtualKeyboard::keyboardRectChanged,
        this, &QPlatformInputContext::emitKeyboardRectChanged );
    QObject::connect( m_inputPanel, &QskVirtualKeyboard::localeChanged,
        this, &QPlatformInputContext::emitLocaleChanged );
    if ( m_inputCompositionModel )
    {
        QObject::connect(
            m_inputCompositionModel.get(), &QskInputCompositionModel::groupsChanged,
            m_inputPanel.data(), &QskVirtualKeyboard::setPreeditGroups );
    }
}

#include "moc_QskInputContext.cpp"
