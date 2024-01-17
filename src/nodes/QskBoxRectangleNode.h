/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_BOX_RECTANGLE_NODE_H
#define QSK_BOX_RECTANGLE_NODE_H

#include "QskGlobal.h"
#include "QskFillNode.h"

class QskBoxShapeMetrics;
class QskBoxBorderMetrics;
class QskBoxBorderColors;
class QskGradient;

class QskBoxRectangleNodePrivate;

class QSK_EXPORT QskBoxRectangleNode : public QskFillNode
{
    using Inherited = QskFillNode;

  public:
    QskBoxRectangleNode();
    ~QskBoxRectangleNode() override;

    void updateNode( const QRectF&,
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics&,
        const QskBoxBorderColors&, const QskGradient& );

    void updateNode( const QRectF& rect, const QskGradient& );

    void updateNode( const QRectF& rect,
        const QskBoxShapeMetrics&, const QskGradient& );

  private:
    Q_DECLARE_PRIVATE( QskBoxRectangleNode )
};

#endif
