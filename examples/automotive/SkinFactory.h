/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef SKIN_FACTORY_H
#define SKIN_FACTORY_H

#include <QskSkinFactory.h>

class SkinFactory : public QskSkinFactory
{
    Q_OBJECT

  public:
    enum GraphicRoles
    {
        // to be visible on header/footer
        Indicator,
    };

    QStringList skinNames() const override;
    QskSkin* createSkin( const QString& ) override;

  public Q_SLOTS:
    void rotateSkin();
};

#endif
