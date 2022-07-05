/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef QSK_MATERIAL_SKIN_FACTORY_H
#define QSK_MATERIAL_SKIN_FACTORY_H

#include "QskMaterialGlobal.h"
#include <QskSkinFactory.h>

class QSK_MATERIAL_EXPORT QskMaterialSkinFactory : public QskSkinFactory
{
    Q_OBJECT

    Q_PLUGIN_METADATA( IID QskSkinFactoryIID FILE "metadata.json" )
    Q_INTERFACES( QskSkinFactory )

  public:
    QskMaterialSkinFactory( QObject* parent = nullptr );
    ~QskMaterialSkinFactory() override;

    QStringList skinNames() const override;
    QskSkin* createSkin( const QString& skinName ) override;
};

#endif
