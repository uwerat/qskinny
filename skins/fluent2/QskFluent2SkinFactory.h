/******************************************************************************
 * QSkinny - Copyright (C) 2023 Edelhirsch Software GmbH
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_FLUENT2_SKIN_FACTORY_H
#define QSK_FLUENT2_SKIN_FACTORY_H

#include "QskFluent2Global.h"
#include <QskSkinFactory.h>

class QSK_FLUENT2_EXPORT QskFluent2SkinFactory : public QskSkinFactory
{
    Q_OBJECT
#if defined( QSK_FLUENT2_MAKEDLL )
    Q_PLUGIN_METADATA( IID QskSkinFactoryIID FILE "metadata.json" )
    Q_INTERFACES( QskSkinFactory )
#endif

  public:
    QskFluent2SkinFactory( QObject* parent = nullptr );
    ~QskFluent2SkinFactory() override;

    QVector< QskSkin::SkinInfo > skins() const override;
    QskSkin* createSkin( QskSkin::SkinInfo ) override;
};

#endif
