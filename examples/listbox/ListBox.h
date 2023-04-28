/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include <QskSimpleListBox.h>

class ListBox : public QskSimpleListBox
{
    using Inherited = QskSimpleListBox;

  public:
    explicit ListBox( QQuickItem* parent = nullptr );

  private:
    void populate();
};