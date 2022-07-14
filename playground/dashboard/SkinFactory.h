/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include <QskSkinFactory.h>

class SkinFactory : public QskSkinFactory
{
    Q_OBJECT

  public:
    QStringList skinNames() const override;
    QskSkin* createSkin( const QString& ) override;
};
