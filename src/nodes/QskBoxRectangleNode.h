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

    void updateBox( const QRectF&,
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics&,
        const QskBoxBorderColors&, const QskGradient& );

    void updateBorder( const QRectF&,
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics&,
        const QskBoxBorderColors& );

    void updateFilling( const QRectF& rect, const QskGradient& );

    void updateFilling( const QRectF& rect,
        const QskBoxShapeMetrics&, const QskGradient& );

    void updateFilling( const QRectF& rect, const QskBoxShapeMetrics&,
        const QskBoxBorderMetrics&, const QskGradient& );

    /*
        If true border/filling can be rendered together into the same geometry.
        This should not make much difference as the scene graph batches geometries
        for the same material anyway.
     */
    static bool isCombinedGeometrySupported( const QskGradient& );

  private:
    Q_DECLARE_PRIVATE( QskBoxRectangleNode )
};

#endif
