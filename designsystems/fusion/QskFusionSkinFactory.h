/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_FUSION_SKIN_FACTORY_H
#define QSK_FUSION_SKIN_FACTORY_H

#include "QskFusionGlobal.h"
#include <QskSkinFactory.h>

class QSK_FUSION_EXPORT QskFusionSkinFactory : public QskSkinFactory
{
    Q_OBJECT
#if defined( QSK_FUSION_MAKEDLL )
    Q_PLUGIN_METADATA( IID QskSkinFactoryIID FILE "metadata.json" )
    Q_INTERFACES( QskSkinFactory )
#endif

  public:
    QskFusionSkinFactory( QObject* parent = nullptr );
    ~QskFusionSkinFactory() override;

    QStringList skinNames() const override;
    QskSkin* createSkin( const QString& skinName ) override;
};

#endif
