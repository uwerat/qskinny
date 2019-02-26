/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef MY_SKIN_H
#define MY_SKIN_H

#include <QskSkinFactory.h>

class MySkinFactory : public QskSkinFactory
{
    Q_OBJECT
    using Inherited = QskSkinFactory;

  public:
    QStringList skinNames() const override;
    QskSkin* createSkin( const QString& skinName ) override;
};

#endif
