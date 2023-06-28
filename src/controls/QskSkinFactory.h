/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_SKIN_FACTORY_H
#define QSK_SKIN_FACTORY_H

#include "QskGlobal.h"
#include "QskSkin.h"

#include <qobject.h>

class QSK_EXPORT QskSkinFactory : public QObject
{
    Q_OBJECT

  public:
    QskSkinFactory( QObject* parent = nullptr );
    ~QskSkinFactory() override;

    virtual QVector< QskSkin::SkinInfo > skins() const = 0;
    virtual QskSkin* createSkin( QskSkin::SkinInfo ) = 0;
};

#define QskSkinFactoryIID "org.qskinny.Qsk.QskSkinFactory/1.0"
Q_DECLARE_INTERFACE( QskSkinFactory, QskSkinFactoryIID )

#endif
