/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <QskLinearBox.h>
#include <functional>

class ButtonBox : public QskLinearBox
{
  public:
    ButtonBox( QQuickItem* parent = nullptr );

    void addButton( const QString& text,
        std::function< void() > func, bool autoRepeat = false );
};
