/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SKIN_FACTORY_H
#define QSK_SKIN_FACTORY_H

#include "QskGlobal.h"
#include <qobject.h>

class QskSkin;

class QSK_EXPORT QskSkinFactory : public QObject
{
    Q_OBJECT

  public:
    QskSkinFactory( QObject* parent = nullptr );
    ~QskSkinFactory() override;

    virtual QStringList skinNames() const = 0;
    virtual QskSkin* createSkin( const QString& skinName ) = 0;
};

#define QskSkinFactoryIID "org.qskinny.Qsk.QskSkinFactory/1.0"
Q_DECLARE_INTERFACE( QskSkinFactory, QskSkinFactoryIID )

#endif
