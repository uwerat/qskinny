/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskInputManager.h"
#include "QskInputPanel.h"
#include "QskInputEngine.h"
#include "QskInputContext.h"

#include <QPointer>
#include <QTextCharFormat>

static inline QQuickItem* qskReceiverItem( const QskInputManager* manager )
{
    auto item = manager->inputProxy();
    return item ? item : manager->inputItem();
}

static inline void qskSendReplaceText( QQuickItem* receiver, const QString& text )
{
    if ( receiver == nullptr )
        return;

    QInputMethodEvent::Attribute attribute(
        QInputMethodEvent::Selection, 0, 32767, QVariant() );

    QInputMethodEvent event1( QString(), { attribute } );
    QCoreApplication::sendEvent( receiver, &event1 );

    QInputMethodEvent event2;
    event2.setCommitString( text );

    QCoreApplication::sendEvent( receiver, &event2 );
}

static inline void qskSendText( QQuickItem* receiver,
    const QString& text, bool isFinal )
{
    if ( receiver == nullptr )
        return;

    if ( isFinal )
    {
        QInputMethodEvent event;
        event.setCommitString( text );

        QCoreApplication::sendEvent( receiver, &event );
    }
    else
    {
        QTextCharFormat format;
        format.setFontUnderline( true );

        const QInputMethodEvent::Attribute attribute(
            QInputMethodEvent::TextFormat, 0, text.length(), format );

        QInputMethodEvent event( text, { attribute } );

        QCoreApplication::sendEvent( receiver, &event );
    }
}

static inline void qskSendKey( QQuickItem* receiver, int key )
{
    if ( receiver == nullptr )
        return;

    QKeyEvent keyPress( QEvent::KeyPress, key, Qt::NoModifier );
    QCoreApplication::sendEvent( receiver, &keyPress );

    QKeyEvent keyRelease( QEvent::KeyRelease, key, Qt::NoModifier );
    QCoreApplication::sendEvent( receiver, &keyRelease );
}

class QskInputManager::PrivateData
{
public:
    QPointer< QskInputEngine > engine;
    QPointer< QskControl > panel;

    QLocale predictorLocale;
    Qt::InputMethodHints inputHints = 0;
};

QskInputManager::QskInputManager( QObject* parent ):
    Inherited( parent ),
    m_data( new PrivateData() )
{
}

QskInputManager::~QskInputManager()
{
}

void QskInputManager::attachInputItem( QQuickItem* item )
{
    auto panel = qobject_cast< QskInputPanel* >( m_data->panel );
    if ( panel == nullptr )
        return;

    if ( item )
    {
        auto context = QskInputContext::instance();
        const auto locale = context->locale();

        updateEngine( locale );
        m_data->engine->reset();

        panel->setLocale( locale );
        panel->attachInputItem( item );

        Qt::InputMethodQueries queries = Qt::ImQueryAll;
        queries &= ~Qt::ImEnabled;

        processInputMethodQueries( queries );
    }
    else
    {
        panel->attachInputItem( nullptr );
    }
}

void QskInputManager::processInputMethodQueries(
    Qt::InputMethodQueries queries )
{
    if ( queries & Qt::ImHints )
    {
        m_data->inputHints = 0;

        if ( auto item = inputItem() )
        {
            QInputMethodQueryEvent event( Qt::ImHints );
            QCoreApplication::sendEvent( item, &event );

            m_data->inputHints = static_cast< Qt::InputMethodHints >(
                event.value( Qt::ImHints ).toInt() );
        }

        updatePanel();
    }
}

QskControl* QskInputManager::panel( bool doCreate )
{
    if ( m_data->panel == nullptr && doCreate )
    {
        m_data->panel = createPanel();

        connect( m_data->panel, &QskControl::localeChanged,
            this, &QskInputManager::updatePredictor, Qt::UniqueConnection );
    }

    return m_data->panel;
}

Qt::Alignment QskInputManager::panelAlignment() const
{
    if ( auto panel = qobject_cast< QskInputPanel* >( m_data->panel ) )
    {
        if ( panel->panelHints() & QskInputPanel::InputProxy )
        {
            /*
                When the panel has an input proxy we don't need to see
                the input item and can put the panel in the center
             */

            return Qt::AlignVCenter;
        }
    }

    return Qt::AlignBottom;
}

QskControl* QskInputManager::createPanel()
{
    auto panel = new QskInputPanel();

    connect( panel, &QskInputPanel::keySelected,
        this, &QskInputManager::commitKey, Qt::UniqueConnection );

    connect( panel, &QskInputPanel::predictiveTextSelected,
        this, &QskInputManager::commitPredictiveText, Qt::UniqueConnection );

    return panel;
}

QskInputEngine* QskInputManager::createEngine()
{
    return new QskInputEngine();
}

void QskInputManager::updateEngine( const QLocale& locale )
{
    bool updatePredictor;

    if ( m_data->engine == nullptr)
    {
        auto engine = createEngine();
        if ( engine->parent() == nullptr )
            engine->setParent( this );

        connect( engine, &QskInputEngine::predictionChanged,
            this, &QskInputManager::updatePrediction );

        m_data->engine = engine;
        updatePredictor = true;
    }
    else
    {
        updatePredictor = ( locale != m_data->predictorLocale );
    }

    if ( updatePredictor )
    {
        auto context = QskInputContext::instance();

        m_data->predictorLocale = locale;
        m_data->engine->setPredictor( context->textPredictor( locale ) );
        updatePanel();
    }
}

void QskInputManager::updatePredictor()
{
    if ( m_data->panel && m_data->engine )
    {
        auto context = QskInputContext::instance();
        const auto locale = context->locale();

        if ( m_data->predictorLocale != locale )
            m_data->engine->setPredictor( context->textPredictor( locale ) );
    }
}

void QskInputManager::updatePanel()
{
    if ( auto panel = qobject_cast< QskInputPanel* >( m_data->panel ) )
    {
        const auto mask = Qt::ImhNoPredictiveText
            | Qt::ImhExclusiveInputMask | Qt::ImhHiddenText;

        panel->setPanelHint( QskInputPanel::Prediction,
            m_data->engine->predictor() && !( m_data->inputHints & mask ) );
    }
}

QQuickItem* QskInputManager::inputItem() const
{
    if ( auto panel = qobject_cast< QskInputPanel* >( m_data->panel ) )
        return panel->attachedInputItem();

    return nullptr;
}

QQuickItem* QskInputManager::inputProxy() const
{
    if ( auto panel = qobject_cast< QskInputPanel* >( m_data->panel ) )
    {
        if ( panel->panelHints() & QskInputPanel::InputProxy )
            return panel->inputProxy();
    }

    return nullptr;
}

void QskInputManager::applyInput( bool success )
{
    auto item = inputItem();
    if ( item == nullptr )
        return;

    if ( success )
    {
        if ( auto proxy = inputProxy() )
        {
            const auto value = proxy->property( "text" );
            if ( value.canConvert<QString>() )
                qskSendReplaceText( item, value.toString() );
        }
    }

    qskSendKey( item, success ? Qt::Key_Return : Qt::Key_Escape );
}

void QskInputManager::applyText( const QString& text, bool isFinal )
{
    qskSendText( qskReceiverItem( this ), text, isFinal );
}

void QskInputManager::applyKey( int key )
{
    // control keys like left/right
    qskSendKey( qskReceiverItem( this ), key );
}

void QskInputManager::commitPredictiveText( int index )
{
    if ( auto panel = qobject_cast< QskInputPanel* >( m_data->panel ) )
    {
        panel->setPrediction( QStringList() );

        const QString text = m_data->engine->predictiveText( index );

        m_data->engine->reset();
        applyText( text, true );
    }
}

void QskInputManager::updatePrediction()
{
    if ( auto panel = qobject_cast< QskInputPanel* >( m_data->panel ) )
        panel->setPrediction( m_data->engine->prediction() );
}

void QskInputManager::commitKey( int key )
{
    auto panel = qobject_cast< QskInputPanel* >( m_data->panel );
    if ( panel == nullptr )
        return;

    int spaceLeft = -1;

    if ( !( m_data->inputHints & Qt::ImhMultiLine ) )
    {
        QInputMethodQueryEvent event1( Qt::ImMaximumTextLength );
        QCoreApplication::sendEvent( panel->attachedInputItem(), &event1 );

        const int maxChars = event1.value( Qt::ImMaximumTextLength ).toInt();
        if ( maxChars >= 0 )
        {
            QInputMethodQueryEvent event2( Qt::ImSurroundingText );
            QCoreApplication::sendEvent( qskReceiverItem( this ), &event2 );

            const auto text = event2.value( Qt::ImSurroundingText ).toString();
            spaceLeft = maxChars - text.length();
        }
    }

    const auto result = m_data->engine->processKey(
        key, m_data->inputHints, spaceLeft );

    if ( result.key )
    {
        switch( result.key )
        {
            case Qt::Key_Return:
            {
                applyInput( true );
                break;
            }
            case Qt::Key_Escape:
            {
                applyInput( false );
                break;
            }
            default:
            {
                applyKey( result.key );
            }
        }
    }
    else if ( !result.text.isEmpty() )
    {
        applyText( result.text, result.isFinal );
    }
}

#include "moc_QskInputManager.cpp"
