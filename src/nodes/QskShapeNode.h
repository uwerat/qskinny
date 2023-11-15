/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_SHAPE_NODE_H
#define QSK_SHAPE_NODE_H

#include "QskGlobal.h"
#include "QskFillNode.h"

class QskGradient;
class QColor;
class QPainterPath;

class QskShapeNodePrivate;

class QSK_EXPORT QskShapeNode : public QskFillNode
{
    using Inherited = QskFillNode;

  public:
    QskShapeNode();
    ~QskShapeNode() override;

    void updateNode( const QPainterPath&, const QTransform&,
        const QRectF&, const QskGradient& );

  private:
    Q_DECLARE_PRIVATE( QskShapeNode )
};

#endif
