/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_STROKE_NODE_H
#define QSK_STROKE_NODE_H

#include "QskGlobal.h"
#include <qsgnode.h>

class QPen;
class QPainterPath;

class QskStrokeNodePrivate;

class QSK_EXPORT QskStrokeNode : public QSGGeometryNode
{
  public:
    QskStrokeNode();

    void updateNode( const QPainterPath&, const QTransform&, const QPen& );

  private:
    Q_DECLARE_PRIVATE( QskStrokeNode )
};

#endif
