/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_SKIN_MANAGER_H
#define QSK_SKIN_MANAGER_H

#include "QskGlobal.h"
#include "QskSkin.h"

#include <qobject.h>
#include <memory>

class QskSkinFactory;
class QskAnimationHint;

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
    void unregisterFactories();

    QStringList skinNames() const;

    QskSkin* createSkin( const QString& skinName,
        QskSkin::ColorScheme = QskSkin::UnknownScheme ) const;

    void setSkin( QskSkin* );
    QskSkin* skin();

    QskSkin* setSkin( const QString& );
    QString skinName() const;

    void setTransitionHint( const QskAnimationHint& );
    QskAnimationHint transitionHint() const;

  Q_SIGNALS:
    void skinChanged( QskSkin* );
    void colorSchemeChanged( QskSkin::ColorScheme );

  protected:
    QskSkinManager();
    ~QskSkinManager() override;

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
