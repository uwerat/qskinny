/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <QskSkinFactory.h>

class SkinFactory : public QskSkinFactory
{
    Q_OBJECT

  public:
    QVector< QskSkin::SkinInfo > skins() const override;
    QskSkin* createSkin( QskSkin::SkinInfo ) override;
};
