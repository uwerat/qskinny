/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_STROKE_NODE_H
#define QSK_STROKE_NODE_H

#include "QskGlobal.h"
#include "QskFillNode.h"

class QPen;
class QPainterPath;
class QPolygonF;

class QSK_EXPORT QskStrokeNode : public QskFillNode
{
    using Inherited = QskFillNode;

  public:
    QskStrokeNode();
    ~QskStrokeNode() override;

    void updatePath( const QPainterPath&, const QPen& );
    void updatePath( const QPainterPath&, const QTransform&, const QPen& );
};

#endif
