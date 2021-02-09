/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskMainQml.h"
#include <QskSkinManager.h>
#include <QskSkin.h>

static void qskRegisterConverter()
{
    QMetaType::registerConverter< int, QskSetupFlagsQml >();
}

Q_CONSTRUCTOR_FUNCTION( qskRegisterConverter )

QskMain::QskMain( QObject* parent )
    : QObject( parent )
{
    // how to supress warnings about a missing skinListChanged
    // as we don't have it ??

    connect( setup(), &QskSetup::skinChanged,
        this, &QskMain::skinChanged, Qt::QueuedConnection );

    connect( setup(), &QskSetup::itemUpdateFlagsChanged,
        this, &QskMain::itemUpdateFlagsChanged, Qt::QueuedConnection );
}

QStringList QskMain::skinList() const
{
    auto manager = QskSkinManager::instance();
    return manager ? manager->skinNames() : QStringList();
}

QQmlListProperty< QObject > QskMain::data()
{
#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
    using SizeType = qsizetype;
#else
    using SizeType = int;
#endif

    return QQmlListProperty< QObject >(
        this, nullptr,
        []( QQmlListProperty< QObject >* property, QObject* value )
        {
            auto main = static_cast< QskMain* >( property->object );
            main->m_data.append( value );
        },
        []( QQmlListProperty< QObject >* property )
        {
            auto main = static_cast< const QskMain* >( property->object );
            return static_cast< SizeType >( main->m_data.count() );
        },
        []( QQmlListProperty< QObject >* property, SizeType index )
        {
            auto main = static_cast< const QskMain* >( property->object );
            return main->m_data.at( index );
        },
        []( QQmlListProperty< QObject >* property )
        {
            auto main = static_cast< QskMain* >( property->object );
            main->m_data.clear();
        }
    );
}

#include "moc_QskMainQml.cpp"
