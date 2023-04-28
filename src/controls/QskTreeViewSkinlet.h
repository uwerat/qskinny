/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_TREE_VIEW_SKINLET_H
#define QSK_TREE_VIEW_SKINLET_H

#include "QskListViewSkinlet.h"

class QSK_EXPORT QskTreeViewSkinlet : public QskListViewSkinlet
{
    Q_GADGET

    using Inherited = QskListViewSkinlet;

  public:
    Q_INVOKABLE QskTreeViewSkinlet( QskSkin* = nullptr );
};

#endif
