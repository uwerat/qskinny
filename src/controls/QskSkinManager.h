/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SKIN_MANAGER_H
#define QSK_SKIN_MANAGER_H

#include "QskGlobal.h"

#include <qobject.h>
#include <memory>

class QskSkin;
class QskSkinFactory;

#if defined( qskSkinManager )
#undef qskSkinManager
#endif

#define qskSkinManager QskSkinManager::instance()

class QSK_EXPORT QskSkinManager : public QObject
{
    Q_OBJECT

  public:
    static QskSkinManager* instance();

    void addPluginPath( const QString& );
    void removePluginPath( const QString& );

    void setPluginPaths( const QStringList& );
    QStringList pluginPaths() const;

    void registerFactory( const QString& factoryId, QskSkinFactory* );
    void unregisterFactory( const QString& factoryId );

    QStringList skinNames() const;

    QskSkin* createSkin( const QString& skinName ) const;

  protected:
    QskSkinManager();
    ~QskSkinManager() override;

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
