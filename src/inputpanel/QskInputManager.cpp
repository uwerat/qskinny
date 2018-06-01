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

        panel->setEngine( m_data->engine );
        panel->setLocale( locale );
        panel->attachInputItem( item );
    }
    else
    {
        panel->setEngine( nullptr );
        panel->attachInputItem( nullptr );
    }
}

void QskInputManager::processInputMethodQueries(
    Qt::InputMethodQueries queries )
{
    if ( auto panel = qobject_cast< QskInputPanel* >( m_data->panel ) )
        panel->processInputMethodQueries( queries );
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
        if ( panel->hasInputProxy() )
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

    connect( panel, &QskInputPanel::done,
        this, &QskInputManager::applyInput, Qt::UniqueConnection );

    connect( panel, &QskInputPanel::textEntered,
        this, &QskInputManager::applyText, Qt::UniqueConnection );

    connect( panel, &QskInputPanel::keyEntered,
        this, &QskInputManager::applyKey, Qt::UniqueConnection );

    return panel;
}

QskInputEngine* QskInputManager::createEngine()
{
    return new QskInputEngine();
}

void QskInputManager::updateEngine( const QLocale& locale )
{
    auto context = QskInputContext::instance();

    if ( m_data->engine == nullptr)
    {
        auto engine = createEngine();
        if ( engine->parent() == nullptr )
            engine->setParent( this );

        m_data->predictorLocale = locale;
        engine->setPredictor( context->textPredictor( locale ) );

        m_data->engine = engine;
    }
    else
    {
        if ( m_data->predictorLocale != locale )
        {
            m_data->predictorLocale = locale;
            m_data->engine->setPredictor( context->textPredictor( locale ) );
        }
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
        if ( panel->hasInputProxy() )
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
    auto item = inputProxy();
    if ( item == nullptr )
        item = inputItem();

    qskSendText( item, text, isFinal );
}

void QskInputManager::applyKey( int key )
{
    // control keys like left/right

    auto item = inputProxy();
    if ( item == nullptr )
        item = inputItem();

    qskSendKey( item, key );
}

#include "moc_QskInputManager.cpp"
