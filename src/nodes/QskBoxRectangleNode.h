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

class QQuickWindow;

class QSK_EXPORT QskBoxRectangleNode : public QskFillNode
{
    using Inherited = QskFillNode;

  public:
    QskBoxRectangleNode();
    ~QskBoxRectangleNode() override;

    void updateBox( const QQuickWindow*, const QRectF&,
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics&,
        const QskBoxBorderColors&, const QskGradient& );

    void updateBorder( const QQuickWindow*, const QRectF&,
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics&,
        const QskBoxBorderColors& );

    void updateFilling( const QQuickWindow*, const QRectF&, const QskGradient& );

    void updateFilling( const QQuickWindow*, const QRectF&,
        const QskBoxShapeMetrics&, const QskGradient& );

    void updateFilling( const QQuickWindow*, const QRectF&,
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics&, const QskGradient& );

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
