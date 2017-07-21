/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SKIN_FACTORY_H
#define QSK_SKIN_FACTORY_H

#include "QskGlobal.h"
#include <QObject>

class QskSkin;
class QStringList;

class QSK_EXPORT QskSkinFactory : public QObject
{
    Q_OBJECT

public:
    QskSkinFactory( QObject* parent = nullptr );
    virtual ~QskSkinFactory();

    virtual QStringList skinNames() const = 0;
    virtual QskSkin* createSkin( const QString& skinName ) = 0;
};

namespace Qsk
{
    QSK_EXPORT void registerSkinFactory( const QString& id, QskSkinFactory* );
    QSK_EXPORT QskSkinFactory* skinFactory( const QString& id );

    QSK_EXPORT QStringList skinNames();
    QSK_EXPORT QskSkin* createSkin( const QString& skinName );
}

#endif
