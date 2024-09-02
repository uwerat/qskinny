/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_ARC_RENDER_NODE_H
#define QSK_ARC_RENDER_NODE_H

#include "QskGlobal.h"
#include "QskFillNode.h"

class QskGradient;
class QskArcMetrics;

class QskArcRenderNodePrivate;

class QSK_EXPORT QskArcRenderNode : public QskFillNode
{
    using Inherited = QskFillNode;

  public:
    QskArcRenderNode();
    ~QskArcRenderNode() override;

    void updateNode( const QRectF&, const QskArcMetrics&, const QskGradient& );

    void updateNode( const QRectF&, const QskArcMetrics&,
        qreal borderWidth, const QColor& borderColor );

    void updateNode( const QRectF&, const QskArcMetrics&, bool radial,
        qreal borderWidth, const QColor& borderColor, const QskGradient& );

  private:
    Q_DECLARE_PRIVATE( QskArcRenderNode )
};

#endif
