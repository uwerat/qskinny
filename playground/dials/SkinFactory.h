/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
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
