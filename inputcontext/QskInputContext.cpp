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
#include <QHash>
#include <QPointer>

class QskInputContext::PrivateData
{
public:
    QPointer< QQuickItem > inputItem;
    QPointer< QskVirtualKeyboard > inputPanel;
    QskInputCompositionModel* compositionModel;
    QHash< QLocale, QskInputCompositionModel* > compositionModels;
};

QskInputContext::QskInputContext() :
    m_data( new PrivateData() )
{
    setObjectName( "InputContext" );

    m_data->compositionModel = new QskInputCompositionModel();

    connect( m_data->compositionModel, &QskInputCompositionModel::candidatesChanged,
        this, &QskInputContext::handleCandidatesChanged );

    connect( qskSetup, &QskSetup::inputPanelChanged,
        this, &QskInputContext::setInputPanel );

#if 1
    setCompositionModel( QLocale::Chinese, new QskPinyinCompositionModel() );
#endif

    setInputPanel( qskSetup->inputPanel() );
}

QskInputContext::~QskInputContext()
{
    if ( m_data->inputPanel )
        delete m_data->inputPanel;

    qDeleteAll( m_data->compositionModels );
    delete m_data->compositionModel;
}

bool QskInputContext::isValid() const
{
    return true;
}

void QskInputContext::update( Qt::InputMethodQueries queries )
{
    if ( !m_data->inputItem )
        return;

    QInputMethodQueryEvent queryEvent( queries );
    if ( !QCoreApplication::sendEvent( m_data->inputItem, &queryEvent ) )
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

        auto oldModel = compositionModel();

        if( m_data->inputPanel )
            m_data->inputPanel->setLocale( locale );

        auto newModel = compositionModel();

        if( oldModel != newModel )
        {
            if( m_data->inputPanel )
            {
                m_data->inputPanel->setCandidateBarVisible( newModel->supportsSuggestions() );
                m_data->inputPanel->disconnect( oldModel );

                connect( newModel, &QskInputCompositionModel::groupsChanged,
                    m_data->inputPanel.data(), &QskVirtualKeyboard::setPreeditGroups );

                connect( newModel, &QskInputCompositionModel::candidatesChanged,
                    this, &QskInputContext::handleCandidatesChanged );
            }
        }
    }

    // Qt::ImAbsolutePosition
    // Qt::ImTextBeforeCursor // important for chinese
    // Qt::ImTextAfterCursor  // important for chinese
    // Qt::ImPlatformData     // hard to say...
}

QQuickItem* QskInputContext::inputItem()
{
    return m_data->inputItem;
}

QRectF QskInputContext::keyboardRect() const
{
    if ( m_data->inputPanel
         && QskDialog::instance()->policy() != QskDialog::TopLevelWindow )
    {
        return m_data->inputPanel->geometry();
    }

    return Inherited::keyboardRect();
}

bool QskInputContext::isAnimating() const
{
    return false;
}

void QskInputContext::showInputPanel()
{
    if ( !m_data->inputPanel )
    {
        setInputPanel( new QskVirtualKeyboard() );

        if ( QskDialog::instance()->policy() == QskDialog::TopLevelWindow )
        {
            auto window = new QskWindow;
            window->setFlags( Qt::Tool | Qt::WindowDoesNotAcceptFocus );
            window->resize( 800, 240 ); // ### what size?
            m_data->inputPanel->setParentItem( window->contentItem() );
            connect( window, &QskWindow::visibleChanged,
                this, &QskInputContext::emitInputPanelVisibleChanged );
        }
        else
        {
            auto window = qobject_cast< QQuickWindow* >( QGuiApplication::focusWindow() );
            if ( window )
            {
                m_data->inputPanel->setParentItem( window->contentItem() );
                m_data->inputPanel->setSize( window->size() );
            }
        }
    }

    auto window = m_data->inputPanel->window();
    if ( window && window != QGuiApplication::focusWindow() )
        window->show();
    else
        m_data->inputPanel->setVisible( true );
}

void QskInputContext::hideInputPanel()
{
    if ( m_data->inputPanel == nullptr )
        return;

    auto window = m_data->inputPanel->window();
    if ( window && window != QGuiApplication::focusWindow() )
        window->hide();
    else
        m_data->inputPanel->setVisible( false );
}

bool QskInputContext::isInputPanelVisible() const
{
    auto panel = m_data->inputPanel;

    return panel && panel->isVisible()
        && panel->window() && panel->window()->isVisible();
}

QLocale QskInputContext::locale() const
{
    return m_data->inputPanel ? m_data->inputPanel->locale() : QLocale();
}

void QskInputContext::setFocusObject( QObject* focusObject )
{
    if ( focusObject == nullptr )
    {
        m_data->inputItem = nullptr;
        compositionModel()->setInputItem( nullptr );
        return;
    }

    bool inputItemChanged = false;

    auto focusItem = qobject_cast< QQuickItem* >( focusObject );
    if( focusItem )
    {
        // Do not change the input item when panel buttons get the focus:
        if( qskNearestFocusScope( focusItem ) != m_data->inputPanel )
        {
            m_data->inputItem = focusItem;
            compositionModel()->setInputItem( focusItem );
            inputItemChanged = true;
        }
    }

    if( inputItemChanged )
    {
        QInputMethodQueryEvent queryEvent( Qt::ImEnabled );
        if ( !QCoreApplication::sendEvent( m_data->inputItem, &queryEvent ) )
            return;

        if ( !queryEvent.value( Qt::ImEnabled ).toBool() )
        {
            hideInputPanel();
            return;
        }
    }

    update( Qt::InputMethodQuery( Qt::ImQueryAll & ~Qt::ImEnabled ) );
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

void QskInputContext::invokeAction( QInputMethod::Action action, int cursorPosition )
{
    auto model = compositionModel();

    switch ( static_cast< QskVirtualKeyboard::Action >( action ) )
    {
        case QskVirtualKeyboard::Compose:
        {
            model->composeKey( static_cast< Qt::Key >( cursorPosition ) );
            break;
        }
        case QskVirtualKeyboard::SelectGroup:
        {
            model->setGroupIndex( cursorPosition );
            break;
        }
        case QskVirtualKeyboard::SelectCandidate:
        {
            model->commitCandidate( cursorPosition );

            if ( m_data->inputPanel )
                m_data->inputPanel->setPreeditCandidates( QVector< QString >() );

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

    m_data->inputPanel->setPreeditCandidates( candidates );
}

void QskInputContext::setInputPanel( QskVirtualKeyboard* inputPanel )
{
    if ( m_data->inputPanel == inputPanel )
        return;

    auto model = compositionModel();

    if ( m_data->inputPanel )
    {
        m_data->inputPanel->disconnect( this );

        if ( model )
            model->disconnect( m_data->inputPanel );
    }

    m_data->inputPanel = inputPanel;

    if ( inputPanel )
    {
        connect( inputPanel, &QskVirtualKeyboard::visibleChanged,
            this, &QPlatformInputContext::emitInputPanelVisibleChanged );

        connect( inputPanel, &QskVirtualKeyboard::keyboardRectChanged,
            this, &QPlatformInputContext::emitKeyboardRectChanged );

        connect( inputPanel, &QskVirtualKeyboard::localeChanged,
            this, &QPlatformInputContext::emitLocaleChanged );

        if ( model )
        {
            inputPanel->setCandidateBarVisible(
                model->supportsSuggestions() );

            connect( model, &QskInputCompositionModel::groupsChanged,
                inputPanel, &QskVirtualKeyboard::setPreeditGroups );
        }
    }
}

#include "moc_QskInputContext.cpp"
