/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSkinManager.h"
#include "QskSkinFactory.h"

#include <qdir.h>
#include <qglobalstatic.h>
#include <qjsonarray.h>
#include <qjsonobject.h>
#include <qmap.h>
#include <qpluginloader.h>
#include <qpointer.h>
#include <qset.h>

static inline QStringList qskSplitPath( const QString& s )
{
    const auto separator = QDir::listSeparator();

#if QT_VERSION >= QT_VERSION_CHECK( 5, 15, 0 )
    return s.split( separator, Qt::SkipEmptyParts );
#else
    return s.split( separator, QString::SkipEmptyParts );
#endif
}

/*
    We could use QFactoryLoader, but as it is again a "private" class
    and does a couple of hardcoded things we don't need ( like always resolving
    from the application library path ) we prefer having our own code.
 */

namespace
{
    class SkinManager final : public QskSkinManager
    {
    };
}

Q_GLOBAL_STATIC( SkinManager, qskGlobalSkinManager )

static QStringList qskPathList( const char* envName )
{
    const auto env = qgetenv( envName );
    if ( env.isEmpty() )
        return QStringList();

    return qskSplitPath( QFile::decodeName( env ) );
}

static inline QString qskResolvedPath( const QString& path )
{
    return QDir( path ).canonicalPath();
}

namespace
{
    class FactoryLoader final : public QPluginLoader
    {
      public:
        FactoryLoader( QObject* parent = nullptr )
            : QPluginLoader( parent )
        {
        }

        bool setPlugin( const QString& fileName )
        {
            QPluginLoader::setFileName( fileName );

            /*
                FactoryId and names of the skins can be found in the metadata
                without having to load the plugin itself
             */

            const QLatin1String TokenInterfaceId( "IID" );
            const QLatin1String TokenData( "MetaData" );
            const QLatin1String TokenFactoryId( "FactoryId" );
            const QLatin1String TokenSkins( "Skins" );

            const QLatin1String InterfaceId( QskSkinFactoryIID );

            const auto pluginData = metaData();
            if ( pluginData.value( TokenInterfaceId ) == InterfaceId )
            {
                const auto factoryData = pluginData.value( TokenData ).toObject();

                m_factoryId = factoryData.value( TokenFactoryId ).toString().toLower();
                if ( m_factoryId.isEmpty() )
                {
                    // Creating a dummy factory id
                    static int i = 0;
                    m_factoryId = QStringLiteral( "skin_factory_" ) + QString::number( i++ );
                }

                const auto skinNames = factoryData.value( TokenSkins ).toArray();

                for ( const auto& name : skinNames )
                    m_skinNames += name.toString().toLower();
            }

            return !m_skinNames.isEmpty();
        }

        inline QString factoryId() const
        {
            return m_factoryId;
        }

        inline QskSkinFactory* factory()
        {
            auto factory = qobject_cast< QskSkinFactory* >( QPluginLoader::instance() );
            if ( factory )
            {
                factory->setParent( nullptr );
                factory->setObjectName( m_factoryId );
            }

            return factory;
        }

        inline QStringList skinNames() const
        {
            return m_skinNames;
        }

      private:
        void setFileName( const QString& ) = delete;
        QObject* instance() = delete;

        QString m_factoryId;
        QStringList m_skinNames;
    };

    class FactoryMap
    {
      private:
        class Data
        {
          public:
            Data()
                : loader( nullptr )
            {
            }

            ~Data()
            {
                reset();
            }

            void reset()
            {
                if ( factory && factory->parent() == nullptr )
                    delete factory;

                factory = nullptr;

                delete loader;
                loader = nullptr;
            }

            FactoryLoader* loader;
            QPointer< QskSkinFactory > factory;
        };

      public:
        FactoryMap()
            : m_isValid( false )
        {
        }

        void reset()
        {
            m_skinNames.clear();
            m_skinMap.clear();
            m_factoryMap.clear();
        }

        QskSkinFactory* factory( const QString& skinName )
        {
            if ( !m_isValid )
                rebuild();

            const auto it = m_skinMap.constFind( skinName );
            if ( it != m_skinMap.constEnd() )
            {
                auto it2 = m_factoryMap.find( it.value() );
                if ( it2 != m_factoryMap.end() )
                {
                    auto& data = it2.value();
                    if ( ( data.factory == nullptr ) && data.loader != nullptr )
                        data.factory = data.loader->factory();

                    return data.factory;
                }
            }

            return nullptr;
        }

        QStringList skinNames() const
        {
            if ( !m_isValid )
                const_cast< FactoryMap* >( this )->rebuild();

            return m_skinMap.keys();
        }

        QStringList skinNames( const QString& factoryId ) const
        {
            const auto it = m_factoryMap.constFind( factoryId );
            if ( it != m_factoryMap.constEnd() )
            {
                const auto& data = it.value();

                if ( data.factory )
                    return data.factory->skinNames();

                if ( data.loader )
                    return data.loader->skinNames();
            }

            return QStringList();
        }

        void insertFactory( FactoryLoader* loader )
        {
            auto& data = m_factoryMap[ loader->factoryId() ];

            if ( data.loader != loader )
            {
                data.reset();
                data.loader = loader;

                m_skinMap.clear();
                m_isValid = false;
            }
        }

        void insertFactory( const QString& factoryId, QskSkinFactory* factory )
        {
            auto& data = m_factoryMap[ factoryId ];

            if ( data.factory != factory )
            {
                data.reset();
                data.factory = factory;

                m_skinMap.clear();
                m_skinNames.clear();
                m_isValid = false;
            }
        }

        void removeFactory( const QString& factoryId )
        {
            const auto it = m_factoryMap.find( factoryId );
            if ( it == m_factoryMap.end() )
                return;

            m_factoryMap.erase( it );

            if ( m_isValid )
            {
                for ( auto it = m_skinMap.constBegin();
                    it != m_skinMap.constEnd(); ++it )
                {
                    if ( it.key() == factoryId )
                    {
                        m_isValid = false;
                        break;
                    }
                }

                if ( !m_isValid )
                {
                    m_skinNames.clear();
                    m_skinMap.clear();
                }
            }
        }

        inline bool hasFactory( const QString& factoryId ) const
        {
            return m_factoryMap.contains( factoryId );
        }

      private:
        void rebuild()
        {
            m_skinMap.clear();

            // first we try all factories, that have been added manually
            for ( auto it = m_factoryMap.begin(); it != m_factoryMap.end(); ++it )
            {
                const auto& data = it.value();

                if ( data.loader == nullptr && data.factory )
                    rebuild( it.key(), data.factory->skinNames() );
            }

            // all factories from plugins are following
            for ( auto it = m_factoryMap.begin(); it != m_factoryMap.end(); ++it )
            {
                const auto& data = it.value();
                if ( data.loader )
                    rebuild( it.key(), data.loader->skinNames() );
            }

            m_skinNames = m_skinMap.keys();
            m_isValid = true;
        }

        void rebuild( const QString& factoryId, const QStringList& skinNames )
        {
            for ( const auto& name : skinNames )
            {
                if ( !m_skinMap.contains( name ) )
                    m_skinMap.insert( name, factoryId );
            }
        }

        QMap< QString, Data > m_factoryMap; // factoryId -> data
        QMap< QString, QString > m_skinMap; // skinName -> factoryId
        QStringList m_skinNames;

        bool m_isValid;
    };
}

class QskSkinManager::PrivateData
{
  public:
    PrivateData()
        : pluginsRegistered( false )
    {
    }

    inline void ensurePlugins()
    {
        if ( !pluginsRegistered )
        {
            for ( const auto& path : qskAsConst( pluginPaths ) )
                registerPlugins( path + QStringLiteral( "/skins" ) );

            pluginsRegistered = true;
        }
    }

    void registerPlugins( const QString& path )
    {
        /*
            We only detect plugins, but don't load them before being needed.
            Static plugins are not supported as QskSkinmanager::registerFactory
            offers a better solution for this use case.
         */

        QDir dir( path );

        FactoryLoader* loader = nullptr;

        const auto dirEntries = dir.entryList( QDir::Files );

        for ( const auto& fileName : dirEntries )
        {
            if ( loader == nullptr )
                loader = new FactoryLoader();

            bool ok = loader->setPlugin( dir.absoluteFilePath( fileName ) );
            if ( ok && !factoryMap.hasFactory( loader->factoryId() ) )
            {
                factoryMap.insertFactory( loader );
                loader = nullptr;
            }
        }

        delete loader;
    }

  public:
    QStringList pluginPaths;
    FactoryMap factoryMap;

    bool pluginsRegistered : 1;
};

QskSkinManager* QskSkinManager::instance()
{
    return qskGlobalSkinManager;
}

QskSkinManager::QskSkinManager()
    : m_data( new PrivateData() )
{
    setPluginPaths( qskPathList( "QSK_PLUGIN_PATH" ) +
        qskPathList( "QT_PLUGIN_PATH" ) );
}

QskSkinManager::~QskSkinManager()
{
}

void QskSkinManager::addPluginPath( const QString& path )
{
    const auto pluginPath = qskResolvedPath( path );

    if ( !pluginPath.isEmpty() && !pluginPath.contains( pluginPath ) )
    {
        m_data->pluginPaths += pluginPath;

        if ( m_data->pluginsRegistered )
            m_data->registerPlugins( pluginPath );
    }
}

void QskSkinManager::removePluginPath( const QString& path )
{
    const auto pluginPath = qskResolvedPath( path );

    if ( m_data->pluginPaths.removeOne( pluginPath ) )
    {
        if ( m_data->pluginsRegistered )
        {
            m_data->factoryMap.reset();
            m_data->pluginsRegistered = false;
        }
    }
}

void QskSkinManager::setPluginPaths( const QStringList& paths )
{
    m_data->pluginPaths.clear();

    QSet< QString > pathSet; // checking for duplicates
    QStringList pluginPaths;

    for ( const auto& path : paths )
    {
        const auto pluginPath = qskResolvedPath( path );
        if ( !pluginPath.isEmpty() && !pathSet.contains( pluginPath ) )
        {
            pathSet += pluginPath;
            pluginPaths += pluginPath;
        }
    }

    if ( pluginPaths != m_data->pluginPaths )
    {
        m_data->pluginPaths = pluginPaths;
        m_data->factoryMap.reset();
        m_data->pluginsRegistered = false;
    }
}

QStringList QskSkinManager::pluginPaths() const
{
    return m_data->pluginPaths;
}

void QskSkinManager::registerFactory(
    const QString& factoryId, QskSkinFactory* factory )
{
    if ( factoryId.isEmpty() || factory == nullptr )
        return;

    /*
        Manually registered factories always come first, and we don't need
        to check the plugins here.
     */

    m_data->factoryMap.insertFactory( factoryId.toLower(), factory );
}

void QskSkinManager::unregisterFactory( const QString& factoryId )
{
    if ( factoryId.isEmpty() )
        return;

    /*
        As this call might be about a factory from a plugin, we need
        to know about them here.
     */

    m_data->ensurePlugins();
    m_data->factoryMap.removeFactory( factoryId.toLower() );
}

QStringList QskSkinManager::skinNames() const
{
    m_data->ensurePlugins();
    return m_data->factoryMap.skinNames();
}

QskSkin* QskSkinManager::createSkin( const QString& skinName ) const
{
    m_data->ensurePlugins();

    auto& map = m_data->factoryMap;

    auto name = skinName;

    auto factory = map.factory( name );
    if ( factory == nullptr )
    {
        /*
            Once the Fusion skin has been implemented it will be used
            as fallback. For the moment we implement
            another stupid fallback. TODO ...
         */

        const auto names = map.skinNames();
        if ( !names.isEmpty() )
        {
            name = names.last();
            factory = map.factory( name );
        }
    }

    return factory ? factory->createSkin( name ) : nullptr;
}

#include "moc_QskSkinManager.cpp"
