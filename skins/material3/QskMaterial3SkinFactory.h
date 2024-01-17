/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_MATERIAL_SKIN_FACTORY_H
#define QSK_MATERIAL_SKIN_FACTORY_H

#include "QskMaterial3Global.h"
#include <QskSkinFactory.h>

class QSK_MATERIAL3_EXPORT QskMaterial3SkinFactory : public QskSkinFactory
{
    Q_OBJECT
#if defined( QSK_MATERIAL3_MAKEDLL )
    Q_PLUGIN_METADATA( IID QskSkinFactoryIID FILE "metadata.json" )
    Q_INTERFACES( QskSkinFactory )
#endif

  public:
    QskMaterial3SkinFactory( QObject* parent = nullptr );
    ~QskMaterial3SkinFactory() override;

    QStringList skinNames() const override;
    QskSkin* createSkin( const QString& skinName ) override;
};

#endif
