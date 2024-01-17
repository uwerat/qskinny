/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_SQUIEK_SKIN_FACTORY_H
#define QSK_SQUIEK_SKIN_FACTORY_H

#include "QskSquiekGlobal.h"
#include <QskSkinFactory.h>

class QSK_SQUIEK_EXPORT QskSquiekSkinFactory : public QskSkinFactory
{
    Q_OBJECT

#if defined( QSK_SQUIEK_MAKEDLL )
    Q_PLUGIN_METADATA( IID QskSkinFactoryIID FILE "metadata.json" )
    Q_INTERFACES( QskSkinFactory )
#endif

  public:
    QskSquiekSkinFactory( QObject* parent = nullptr );
    ~QskSquiekSkinFactory() override;

    QStringList skinNames() const override;
    QskSkin* createSkin( const QString& skinName ) override;
};

#endif
