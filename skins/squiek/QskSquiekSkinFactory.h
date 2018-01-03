/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef QSK_SQUIEK_SKIN_FACTORY_H
#define QSK_SQUIEK_SKIN_FACTORY_H

#include <QskSkinFactory.h>

class QSK_EXPORT QskSquiekSkinFactory : public QskSkinFactory
{
    Q_OBJECT

    Q_PLUGIN_METADATA( IID QskSkinFactoryIID FILE "metadata.json" )
    Q_INTERFACES( QskSkinFactory )

public:
    QskSquiekSkinFactory( QObject* parent = nullptr );
    virtual ~QskSquiekSkinFactory();

    virtual QStringList skinNames() const override;
    virtual QskSkin* createSkin( const QString& skinName ) override;
};

#endif
