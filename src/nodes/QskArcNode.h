/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_ARC_NODE_H
#define QSK_ARC_NODE_H

#include "QskShapeNode.h"

class QskArcMetrics;
class QskGradient;
class QskShadowMetrics;

/*
    For the moment a QPainterPath/QskShapeNode.
    But we can do better by creatig vertex lists manually
    like what is done by the box renderer. TODO ...
 */
class QSK_EXPORT QskArcNode : public QskShapeNode
{
  public:
    QskArcNode();
    ~QskArcNode() override;

    void setArcData( const QRectF&, const QskArcMetrics&, const QskGradient& );

    void setArcData( const QRectF&, const QskArcMetrics&,
        qreal borderWidth, const QColor& borderColor, const QskGradient& );

    void setArcData( const QRectF&, const QskArcMetrics&,
        qreal borderWidth, const QColor& borderColor, const QskGradient&,
        const QColor& shadowColor, const QskShadowMetrics&);
};

#endif
