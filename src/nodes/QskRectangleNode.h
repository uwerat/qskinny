/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_RECTANGLE_NODE_H
#define QSK_RECTANGLE_NODE_H

#include "QskGlobal.h"
#include <qsgnode.h>

class QskGradient;
class QskBoxShapeMetrics;
class QskRectangleNodePrivate;

/*
    QskRectangleNode is for rounded rectangles without a border.
    Depending on the type of gradient it uses a different
    material/geometry combination.
 */
class QSK_EXPORT QskRectangleNode : public QSGGeometryNode
{
  public:
    QskRectangleNode();
    ~QskRectangleNode() override;

    void updateNode( const QRectF&, const QskGradient& );
    void updateNode( const QRectF&, const QskBoxShapeMetrics&, const QskGradient& );

  private:
    Q_DECLARE_PRIVATE( QskRectangleNode )
};

#endif
