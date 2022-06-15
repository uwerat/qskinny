/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SHADED_BOX_NODE_H
#define QSK_SHADED_BOX_NODE_H

#include "QskGlobal.h"
#include "QskBoxNode.h"

class QskBoxShadowNode;
class QskShadowMetrics;

class QSK_EXPORT QskShadedBoxNode : public QSGNode
{
  public:
    QskShadedBoxNode();
    ~QskShadedBoxNode() override;

    void setBoxData( const QRectF&,
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics&,
        const QskBoxBorderColors&, const QskGradient&,
        const QskShadowMetrics&, const QColor& shadowColor );

  private:
    QskBoxNode m_boxNode;
    QskBoxShadowNode* m_shadowNode = nullptr;
};

#endif
