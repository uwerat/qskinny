/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <QskSkinFactory.h>

class MySkinFactory : public QskSkinFactory
{
    Q_OBJECT
    using Inherited = QskSkinFactory;

  public:
    QStringList skinNames() const override;
    QskSkin* createSkin( const QString& skinName ) override;
};
