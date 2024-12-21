/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <QskSymbolProvider.h>

class SymbolProvider : public QskSymbolProvider
{
  public:
    SymbolProvider( QObject* parent = nullptr );

    void updateFont();
};
