/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_ARC_RENDER_NODE_H
#define QSK_ARC_RENDER_NODE_H

#include "QskGlobal.h"
#include <qsgnode.h>

class QskGradient;
class QskArcMetrics;

class QskArcRenderNodePrivate;

class QSK_EXPORT QskArcRenderNode : public QSGGeometryNode
{
    using Inherited = QSGGeometryNode;

  public:
    QskArcRenderNode();

    void updateNode( const QRectF&, const QskArcMetrics&, const QskGradient& );
    void updateNode( const QRectF&, const QskArcMetrics&, qreal borderWidth,
        const QColor& borderColor );

    void updateNode( const QRectF&, const QskArcMetrics&, qreal borderWidth,
        const QColor& borderColor, const QskGradient& );

  private:
    Q_DECLARE_PRIVATE( QskArcRenderNode )
};

#endif
