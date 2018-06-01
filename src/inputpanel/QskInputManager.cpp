/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskInputManager.h"
#include "QskInputPanel.h"
#include "QskInputEngine.h"
#include "QskInputContext.h"
#include <QPointer>

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
    return new QskInputPanel();
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
