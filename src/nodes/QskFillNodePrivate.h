/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_FILL_NODE_PRIVATE_H
#define QSK_FILL_NODE_PRIVATE_H

#include "QskGlobal.h"

QSK_QT_PRIVATE_BEGIN
#include <private/qsgnode_p.h>
QSK_QT_PRIVATE_END

class QskFillNode;

class QskFillNodePrivate : public QSGGeometryNodePrivate
{
  public:
    inline QskFillNodePrivate()
        : geometry( QSGGeometry::defaultAttributes_ColoredPoint2D(), 0 )
    {
    }

  private:
    friend class QskFillNode;

    QSGGeometry geometry;
    QskFillNode::Coloring coloring = QskFillNode::Polychrome;
};

#endif
