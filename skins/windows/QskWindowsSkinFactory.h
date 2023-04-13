/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_WINDOWS_SKIN_FACTORY_H
#define QSK_WINDOWS_SKIN_FACTORY_H

#include "QskWindowsGlobal.h"
#include <QskSkinFactory.h>

class QSK_WINDOWS_EXPORT QskWindowsSkinFactory : public QskSkinFactory
{
    Q_OBJECT
#if defined( QSK_Windows_MAKEDLL )
    Q_PLUGIN_METADATA( IID QskSkinFactoryIID FILE "metadata.json" )
    Q_INTERFACES( QskSkinFactory )
#endif

  public:
    QskWindowsSkinFactory( QObject* parent = nullptr );
    ~QskWindowsSkinFactory() override;

    QStringList skinNames() const override;
    QskSkin* createSkin( const QString& skinName ) override;
};

#endif
