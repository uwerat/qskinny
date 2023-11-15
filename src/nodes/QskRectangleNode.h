/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_RECTANGLE_NODE_H
#define QSK_RECTANGLE_NODE_H

#include "QskGlobal.h"
#include "QskFillNode.h"

class QskGradient;
class QskBoxShapeMetrics;
class QskRectangleNodePrivate;

/*
    QskRectangleNode is for rounded rectangles without a border.
 */
class QSK_EXPORT QskRectangleNode : public QskFillNode
{
    using Inherited = QskFillNode;

  public:
    QskRectangleNode();
    ~QskRectangleNode() override;

    void updateNode( const QRectF&, const QskGradient& );
    void updateNode( const QRectF&, const QskBoxShapeMetrics&, const QskGradient& );

  private:
    Q_DECLARE_PRIVATE( QskRectangleNode )
};

#endif
